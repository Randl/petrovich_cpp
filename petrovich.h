/* ��������� ������� ������� ��� ������� � �������. �� ������ ��������� ���
 * � ������������ ������, � ��������� � ������ ���.
 *
 * �������������
 *
 *   ��������� � ��������� ������
 *  
 *  cout << Petrovich::Instance()->Firstname("����", "dative", "male");       => �����
 *  cout << Petrovich::Instance()->Middlename("���������", "dative", "male"); => ����������
 *  cout << Petrovich::Instance()->Lastname("�������", "dative", "male");     => ��������
 *  cout << Petrovich::Instance()->Fullname("������� ���� ���������" "dative");      
 *
 *
 * ���� ��� �� ��������, ��� ����� ���������� �� ��������, ��� ������ ������ DetectGender
 *
 *  gender = Petrovich::Instance()->DetectGender("���������")  => "male"
 * 
 * ��������� ������
 *
 *  "nominative" - ������������
 *  "genitive" - �����������
 *  "dative" - ���������
 *  "accusative" - �����������
 *  "instrumental" - ������������
 *  "prepositional" - ����������
 */
 
#ifndef PETROVICH_H
#define PETROVICH_H
#include <vector>
#include <utility>
#include "yaml-cpp/yaml.h"

enum kCase  {
  NOMINATIVE,    // ������������
  GENITIVE,      // �����������
  DATIVE,        // ���������
  ACCUSATIVE,    // �����������
  INSTRUMENTAL,  // ������������
  PREPOSITIONAL  // ����������
};

enum kGender  {
  NONE,
  MALE,
  FEMALE,
  ANDROGYNOUS
};

inline kCase StringToCase(std::string str_case) {
  if (str_case == "nominative" || str_case == "������������") 
    return NOMINATIVE;  
  if (str_case == "genitive" || str_case == "�����������") 
    return GENITIVE;   
  if (str_case == "dative" || str_case == "���������") 
    return DATIVE;   
  if (str_case == "accusative" || str_case == "�����������") 
    return ACCUSATIVE;   
  if (str_case == "instrumental" || str_case == "������������") 
    return INSTRUMENTAL;  
  if (str_case == "prepositional" || str_case == "����������") 
    return PREPOSITIONAL;  
}

inline kGender StringToGender(std::string str_gender) {
  if (str_gender == "") return NONE;
    if (str_gender == "male" || str_gender == "m" || str_gender == "�" || 
                                          str_gender == "�������") return MALE;  
  if (str_gender == "female" || str_gender == "f" || str_gender == "�" ||
                                        str_gender == "�������") return FEMALE;   
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
  // ����� ������� ��� ���������� � ���������� ������ � �����, ������� � ��������.
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
