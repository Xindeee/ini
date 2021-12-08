#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>

static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}


static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}


static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}
std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); } 
    );
    return s;
}
class IniFile {
    std::map<
        std::string,
        std::map<std::string, std::string>
    > _data;
    std::string path;
    bool isOk;

public:
    bool getStatus() { return isOk; }
    IniFile() {
        isOk = false;
    }
    IniFile(std::string pathIn) {
        path = pathIn;
        std::ifstream file(path);
        if (file.is_open()) {
            isOk = true;
            std::string str;
            bool wasSegment = false;
            std::map<std::string, std::string> trashMap;
            std::map<std::string, std::string>* focusedMap = nullptr;
            while (!file.eof()) {
                std::getline(file, str);
                trim(str);
                if (str.size() == 0) {
                    //std::cout << "no ini format" << std::endl;
                    continue;
                }
                if (str[0] == '[' && str[str.size() - 1] == ']') {
                    str = str.substr(1, str.size() - 2);
                    if (_data.find(str) != _data.end()) {
                        std::cout << "ini format are corrupted" << std::endl;
                        isOk = false;
                        break;
                    }
                    else {
                        _data.insert(std::make_pair(str, std::map<std::string, std::string>()));
                        focusedMap = &(_data[str]);
                        wasSegment = true;
                    }
                }
                else {
                    int indexEq = str.find('=');
                    if (indexEq < str.size()) {
                        std::string varStr = str.substr(0, indexEq);
                        trim(varStr);
                        std::string valStr = str.substr(indexEq + 1);
                        trim(valStr);
                        if (wasSegment) {
                            focusedMap->insert(std::make_pair(varStr, valStr));
                        }
                    }
                }
            }
            file.close();
        }
        else {
            std::cout << "File is not exists" << std::endl;
            return;
        }
    }

    void save() {
        if (isOk) {
            std::ofstream file(path);
            if (file.is_open()){
                for (auto iter = _data.begin();iter != _data.end();iter++) {
                    file << '[' << (*iter).first << ']' << std::endl;
                    std::map<std::string, std::string>& currentSection = (*iter).second;
                    
                    for(auto iter1=currentSection.begin();iter1!=currentSection.end();iter1++){
                        file << (*iter1).first << '=' << (*iter1).second << std::endl;
                    }
                }
                file.close();
            }
        }
    }

    /**
     * Считывает из секции section
     * значение ключа key
     * Если не удалось считать - возвращает default value
     *
     * Определить для std::string, int, float, bool
     */
    template<typename T>
    T read(std::string section, std::string key, T defaultValue = T{}) const {
        if (_data.find(section)!=_data.end()) {
            if (_data.at(section).find(key)!=_data.at(section).end()) {
                return defaultValue; //T(_data.at(section).at(key));
            }
            else {
                return defaultValue;
            }
        }
        else {
            return defaultValue;
        }
    }

    template<>
    int read<int>(std::string section, std::string key, int defaultValue) const {
        if (_data.find(section) != _data.end()) {
            if (_data.at(section).find(key) != _data.at(section).end()) {
                return std::stoi(_data.at(section).at(key));
            }
            else {
                return defaultValue;
            }
        }
        else {
            return defaultValue;
        }
    }

    template<>
    bool read<bool>(std::string section, std::string key, bool defaultValue) const {
        if (_data.find(section) != _data.end()) {
            if (_data.at(section).find(key) != _data.at(section).end()) {
                std::string temp = _data.at(section).at(key);
                temp = str_tolower(temp);
                if (temp == "true" || temp == "yes" || temp == "on" || temp == "y" || temp == "1") {
                    return true;
                }
                else {
                    return false;
                }               
            }
            else {
                return defaultValue;
            }
        }
        else {
            return defaultValue;
        }
    }

    template<>
    float read<float>(std::string section, std::string key, float defaultValue) const {
        if (_data.find(section) != _data.end()) {
            if (_data.at(section).find(key) != _data.at(section).end()) {
                return std::stof(_data.at(section).at(key));
            }
            else {
                return defaultValue;
            }
        }
        else {
            return defaultValue;
        }
    }

    template<>
    std::string read<std::string>(std::string section, std::string key, std::string defaultValue) const {
        if (_data.find(section) != _data.end()) {
            if (_data.at(section).find(key) != _data.at(section).end()) {
                return _data.at(section).at(key);
            }
            else {
                return defaultValue;
            }
        }
        else {
            return defaultValue;
        }
    }

    /**
     * В ключ key из секции section
     * записывает значение value
     *
     * Определить для std::string, int, float, bool
     */
    template<typename T>
    void write(std::string section, std::string key, T value) {
        if (_data.find(section) == _data.end()) {
            _data.insert(std::make_pair(section,std::map<std::string,std::string>()));
        }
        if (_data[section].find(key) == _data[section].end()) {
            //_data[section].insert(std::make_pair(key, std::string(value)));
        }
        else {
            //_data[section][key] = std::string(value);
        }
    }

    template<>
    void write<std::string>(std::string section, std::string key,std::string value) {
        if (_data.find(section) == _data.end()) {
            _data.insert(std::make_pair(section, std::map<std::string, std::string>()));
        }
        if (_data[section].find(key) == _data[section].end()) {
            _data[section].insert(std::make_pair(key, value));
        }
        else {
            _data[section][key] = value;
        }
    }

    template<>
    void write<bool>(std::string section, std::string key, bool value) {
        if (_data.find(section) == _data.end()) {
            _data.insert(std::make_pair(section, std::map<std::string, std::string>()));
        }
        if (_data[section].find(key) == _data[section].end()) {
            _data[section].insert(std::make_pair(key, std::to_string(value)));
        }
        else {
            _data[section][key] = std::to_string(value);
        }
    }

    template<>
    void write<int>(std::string section, std::string key, int value) {
        if (_data.find(section) == _data.end()) {
            _data.insert(std::make_pair(section, std::map<std::string, std::string>()));
        }
        if (_data[section].find(key) == _data[section].end()) {
            _data[section].insert(std::make_pair(key, std::to_string(value)));
        }
        else {
            _data[section][key] = std::to_string(value);
        }
    }

    template<>
    void write<float>(std::string section, std::string key, float value) {
        if (_data.find(section) == _data.end()) {
            _data.insert(std::make_pair(section, std::map<std::string, std::string>()));
        }
        if (_data[section].find(key) == _data[section].end()) {
            _data[section].insert(std::make_pair(key, std::to_string(value)));
        }
        else {
            _data[section][key] = std::to_string(value);
        }
    }

    /**
     * Проверяет, существует ли секция section
     */
    bool sectionExists(std::string section) const{
        if (_data.find(section) != _data.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    /**
     * Проверяет, существует ли ключ key в секции section
     */
    bool keyExists(std::string section, std::string key) const{
        if (_data.find(section) != _data.end()) {
            if (_data.at(section).find(key) != _data.at(section).end()) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        } 
    }

    /**
     * Возвращает существующие секции
     */
    std::vector<std::string> sections() const{
        std::vector<std::string> sec(_data.size());
        int index = 0;
        for (auto iter = _data.begin();iter != _data.end();iter++,index++) {
            sec[index]=(*iter).first;
        }
        return sec;
    }

    /**
     * Возвращает существующие ключи в секции section
     */
    std::vector<std::string> keys(std::string section) const{
        
        if (_data.find(section)!=_data.end()) {
            const std::map<std::string, std::string>& ds = _data.at(section);
            std::vector<std::string> ks(ds.size());
            int index = 0;
            for (auto iter = ds.begin();iter != ds.end();iter++, index++) {
                ks[index] = (*iter).first;
            }
            return ks;
        }
        else {
            return std::vector<std::string>();
        }
        
    }
};

// true -  true, True, TRUE, on, On, ON, yes, Yes, YES, y, Y, 1
// false - остальные

/** config.ini - input
[general]
windowWidth=1080
windowHeight=720
fullscreen=Yes

[player]
x=100
y=200
name=John Doe
**/

int main(int argc, char** argv) {
    IniFile cfg("config.ini");
    assert(cfg.getStatus() == true);
    assert(cfg.read<bool>("general", "fullscreen") == true);

    assert(cfg.read<std::string>("player", "name") == "Ivan Petrov Vasilevishcyhhsh");
    assert(cfg.read<int>("player", "x") == 100);
    assert(cfg.read<int>("player", "y") == 200);

    assert(cfg.read<int>("player", "z") == 0);
    assert(cfg.read<int>("player", "z", 500) == 500);

    assert(cfg.sectionExists("general") == true);
    assert(cfg.sectionExists("player") == true);
    assert(cfg.sectionExists("AAAAAA") != true);

    assert(cfg.keyExists("general", "windowWidth") == true);
    assert(cfg.keyExists("general", "windowHeight") == true);
    assert(cfg.keyExists("general", "windowDepth") == false);
    assert(cfg.keyExists("asdasdadasd", "windowWidth") == false);

    //cfg.sections() == {"general", "player"}
    std::vector<std::string> a=cfg.sections();
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;

    //cfg.keys("general") == {"windowWidth", "windowHeight", "fullscreen"};
    a = cfg.keys("general");
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;

    cfg.write<std::string>("player", "name2", "Sokolov Igor' Sergeevich");
    cfg.write<bool>("player", "bool", false);
    cfg.write<int>("newSection", "value2", 354);
     a = cfg.sections();
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;
    a = cfg.keys("general");
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;
    a = cfg.keys("player");
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;
    a = cfg.keys("newSection");
    for (int i = 0;i < a.size();i++) {
        std::cout << a[i] << ' ';
    }
    std::cout << std::endl;



    cfg.save();

}

/** config.ini - output
[general]
windowWidth=1080
windowHeight=720
fullscreen=Yes

[player]
x=100
y=200
name=Ivan Petrov Vasilevishcyhhsh
bool=true

[newSection]
value=137
**/