/* Склонение падежей русских имён фамилий и отчеств. Вы задаёте начальное имя
 * в именительном падеже, а получаете в нужном вам.
 *
 * Использование
 *
 *   Склонение в дательном падеже
 *  
 *  cout << Petrovich::Instance()->Firstname("Иван", "dative", "male");       => Ивану
 *  cout << Petrovich::Instance()->Middlename("Сергеевич", "dative", "male"); => Сергеевичу
 *  cout << Petrovich::Instance()->Lastname("Воронов", "dative", "male");     => Воронову
 *  cout << Petrovich::Instance()->Fullname("Воронов Иван Сергеевич" "dative");      
 *
 *
 * Если пол не известен, его можно определить по отчеству, при помощи метода DetectGender
 *
 *  gender = Petrovich::Instance()->DetectGender("Сергеевич")  => "male"
 * 
 * Возможные падежи
 *
 *  "nominative" - именительный
 *  "genitive" - родительный
 *  "dative" - дательный
 *  "accusative" - винительный
 *  "instrumental" - творительный
 *  "prepositional" - предложный
 */
 
#ifndef PETROVICH_H
#define PETROVICH_H
#include <vector>
#include <utility>
#include "yaml-cpp/yaml.h"

enum kCase  {
  NOMINATIVE,    // именительный
  GENITIVE,      // родительный
  DATIVE,        // дательный
  ACCUSATIVE,    // винительный
  INSTRUMENTAL,  // творительный
  PREPOSITIONAL  // предложный
};

enum kGender  {
  NONE,
  MALE,
  FEMALE,
  ANDROGYNOUS
};

inline kCase StringToCase(std::string str_case) {
  if (str_case == "nominative" || str_case == "именительный") 
    return NOMINATIVE;  
  if (str_case == "genitive" || str_case == "родительный") 
    return GENITIVE;   
  if (str_case == "dative" || str_case == "дательный") 
    return DATIVE;   
  if (str_case == "accusative" || str_case == "винительный") 
    return ACCUSATIVE;   
  if (str_case == "instrumental" || str_case == "творительный") 
    return INSTRUMENTAL;  
  if (str_case == "prepositional" || str_case == "предложный") 
    return PREPOSITIONAL;  
}

inline kGender StringToGender(std::string str_gender) {
  if (str_gender == "") return NONE;
    if (str_gender == "male" || str_gender == "m" || str_gender == "м" || 
                                          str_gender == "мужской") return MALE;  
  if (str_gender == "female" || str_gender == "f" || str_gender == "ж" ||
                                        str_gender == "женский") return FEMALE;   
  if (str_gender == "androgynous") return ANDROGYNOUS;        
}
    
inline std::string GenderToString(kGender gender) {
  switch (gender)  {
    case NONE: return "";
      case MALE: return "male";  
    case FEMALE: return "female";   
    case ANDROGYNOUS: return "androgynous";   
  }
}

inline std::string Downcase (std::string str) {
  for (char& c:str)
    if ( c >= 'A' && c <= 'Z')
      c -= 'A' - 'a';
  return str;
}
    
inline std::vector<std::string> SplitString(std::string str, char c)  {
  std::vector<std::string> res;
  int start = 0;
  for (int i = 0; i < str.length(); ++i) 
    if (str[i] == c)  {
      res.push_back(str.substr(start, i - start));
      start = i + 1;
      ++i;
    }
  res.push_back(str.substr(start));
  return res;
}
    
inline std::string JoinStrings (std::vector<std::string> strs, char c) {
  std::string str = "";
    for(const std::string& s: strs) 
      str += c + s;
  return str.substr(1);
}

const YAML::Node rules = YAML::LoadFile("rules.yml");

class Petrovich  {
protected:
  // Набор методов для нахождения и применения правил к имени, фамилии и отчеству.
  class Rules  { 
  public:
    std::string Firstname(std::string, kCase, kGender, const YAML::Node&);
      std::string Middlename(std::string, kCase, kGender, const YAML::Node&);
      std::string Lastname(std::string, kCase, kGender, const YAML::Node&);
    static Rules* Instance();
  protected:
    bool Match(std::string, kGender, const YAML::Node&, bool,
                                                     std::vector<std::string>); 
    std::string Inflect(std::string, kCase, kGender, const YAML::Node&);
    void Apply(std::string&, kCase,YAML::Node*);
    void FindAndApply(std::string&, kCase, kGender, const YAML::Node&, 
                   std::vector<std::pair<std::string, bool>>* features = NULL);
    YAML::Node* FindFor(std::string, kGender, const YAML::Node&, 
                   std::vector<std::pair<std::string, bool>>* features = NULL);
    YAML::Node* Find(std::string, kGender, const YAML::Node&, bool, 
                                                     std::vector<std::string>);
    std::string ModificatorFor(kCase, const YAML::Node*);
    std::vector<std::string> ExtractTags(std::vector<std::pair<std::string, 
                                                      bool>>* features = NULL);
    bool TagsAllow(std::vector<std::string>, const YAML::Node& rule_tags);
    Rules(){};
    ~Rules(){};
    static Rules* self_;
  };
  Petrovich(){};
  ~Petrovich(){};
  static Petrovich* self_;
public: 
  static Petrovich* Instance();
  std::string Lastname(std::string, std::string, 
                                          std::string ggender = "androgynous");
  std::string Firstname(std::string, std::string, 
                                          std::string ggender = "androgynous");
  std::string Middlename(std::string, std::string,
                                                     std::string ggender = "");
  std::string Fullname(std::string, std::string, std::string ggender = "");
  std::string getGender();
  std::string DetectGender(std::string);
};


#endif
