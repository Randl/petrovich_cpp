#include "petrovich.h"

Petrovich* Petrovich::self_ = NULL;
Petrovich::Rules* Petrovich::Rules::self_ = NULL;

std::string Petrovich::Lastname(std::string name, std::string gcase,
                                                        std::string ggender)  { 
  return Rules::Instance()->Lastname(name, StringToCase(gcase), 
                                               StringToGender(ggender), rules);
}

std::string Petrovich::Firstname(std::string name, std::string gcase, 
                                                        std::string ggender)  { 
  return Rules::Instance()->Firstname(name, StringToCase(gcase), 
                                               StringToGender(ggender), rules);
}
  
std::string Petrovich::Middlename(std::string name, std::string gcase, 
                                                        std::string ggender)  {
  if (ggender == "") 
    ggender = DetectGender(name);
  return Rules::Instance()->Middlename(name, StringToCase(gcase), 
                                               StringToGender(ggender), rules);
}
  
std::string Petrovich::Fullname(std::string name, std::string gcase, 
                                                         std::string ggender) {
  std::vector<std::string> fullname = SplitString(name, ' ');
  if (ggender == "") 
    ggender = DetectGender(fullname[2]);
  return Lastname(fullname[0], gcase, ggender) + " " + 
         Firstname(fullname[1], gcase, ggender) + " " + 
         Middlename(fullname[2], gcase, ggender);
}


/* Определение пола по отчеству
 *
 *   DetectGender("Алексеевич")  => "male"
 *
 * Если пол не был определён, метод возвращает значение "androgynous"
 *
 *   DetectGender("блаблабла")  => "androgynous"
 */
std::string Petrovich::DetectGender(std::string midname)  {
  if (midname.substr(midname.length() - 2) == "ич") return "male"; 
  if (midname.substr(midname.length() - 2) == "на") return "female";
  return "androgynous";
}
    
std::string Petrovich::Rules::Firstname(std::string name, kCase gcase, 
                                   kGender ggender, const YAML::Node& rules)  {    
                     return Inflect(name, gcase, ggender, rules["firstname"]);}
std::string Petrovich::Rules::Middlename(std::string name, kCase gcase,
                                   kGender ggender, const YAML::Node& rules)  {    
                    return Inflect(name, gcase, ggender, rules["middlename"]);}
std::string Petrovich::Rules::Lastname(std::string name, kCase gcase, 
                                   kGender ggender, const YAML::Node& rules)  {
                       return Inflect(name, gcase, ggender, rules["lastname"]);}

/* Известно несколько типов признаков, которые влияют на процесс поиска.
 *
 * Признак +first_word+ указывает, что данное слово является первым словом
 * в составном слове. Например, в двойной русской фамилии Иванов-Сидоров.
 */
bool Petrovich::Rules::Match(std::string name, kGender ggender, 
 const YAML::Node& rule, bool match_whole_word, std::vector<std::string> tags){
  if (!TagsAllow(tags, rule["tags"])) return false;
  if (rule["gender"].as<std::string>() == "male" && ggender == FEMALE 
          || rule["gender"].as<std::string>() == "female" && ggender != FEMALE)
    return false;

  name = Downcase(name);
  for (auto chars: rule["test"])  {  
    std::string test;
    if (match_whole_word || name.length() < chars.as<std::string>().length())
        test = name;
    else
        test = name.substr(name.length() - chars.as<std::string>().length()); 
    if (test == chars.as<std::string>()) return true;
  }

  return false;
}

std::string Petrovich::Rules::Inflect(std::string name, kCase gcase, 
                                   kGender ggender, const YAML::Node& rules)  {
  int i = 0;

  std::vector<std::string> parts = SplitString(name, '-');

  for(std::string& part: parts)  {
    bool first_word = ++i == 1 && parts.size() > 1;
    std::vector<std::pair<std::string, bool>> 
	             tmp({std::pair<std::string, bool>("first_word", first_word)});
    FindAndApply(part, gcase, ggender, rules, &tmp);
  }

  return JoinStrings(parts, '-');
}

// Применить правило
void Petrovich::Rules::Apply(std::string& name, kCase gcase, YAML::Node* rule){
  for (const char& c:ModificatorFor(gcase, rule))
    switch (c) {
      case '.': break;
      case '-': name = name.substr(0, name.length() - 1); break;
      default: name += c; break;
    }
}

// Найти правило и применить к имени с учетом склонения
void Petrovich::Rules::FindAndApply(std::string& name, kCase gcase, 
                                kGender ggender, const YAML::Node& rules, 
                        std::vector<std::pair<std::string, bool>>* features)  {
  YAML::Node* rule = FindFor(name, ggender, rules, features);
  Apply(name, gcase, rule);
  // Если не найдено правило для имени, возвращаем неизмененное имя.
}

// Найти подходящее правило в исключениях или суффиксах
YAML::Node* Petrovich::Rules::FindFor(std::string name, kGender ggender,
 const YAML::Node& rules, std::vector<std::pair<std::string, bool>>* features){
  std::vector<std::string> tags = ExtractTags(features);

  // Сначала пытаемся найти исключения
  if (rules["exceptions"] != NULL)  {
    YAML::Node* p = Find(name, ggender, rules["exceptions"], true, tags);
    if (p != NULL)
        return p;
  }

  /* Не получилось, ищем в суффиксах. Если не получилось найти и в них,
   * возвращаем неизмененное имя.
   */
  return Find(name, ggender, rules["suffixes"], false, tags);
}

// Найти подходящее правило в конкретном списке правил
YAML::Node* Petrovich::Rules::Find(std::string name, kGender ggender,
 const YAML::Node& rules, bool match_whole_word, std::vector<std::string> tags){
  for (auto rule: rules)  
    if (Match(name, ggender, rule, match_whole_word, tags)) 
        return &rule;
  return NULL;
}

// Получить модификатор из указанного правиля для указанного склонения
std::string Petrovich::Rules::ModificatorFor(kCase gcase, 
                                                     const YAML::Node* rule)  {
  if (rule == NULL)  return ".";
  switch (gcase)  {
    case NOMINATIVE: ".";
    case GENITIVE: return (*rule)["mods"][0].as<std::string>();
    case DATIVE: return (*rule)["mods"][1].as<std::string>();
    case ACCUSATIVE: return (*rule)["mods"][2].as<std::string>();
    case INSTRUMENTAL: return (*rule)["mods"][3].as<std::string>();
    case PREPOSITIONAL: return (*rule)["mods"][4].as<std::string>();
  }
}

// Преобразование +{a: true, b: false, c: true}+ в +%w(a c)+.
std::vector<std::string> Petrovich::Rules::ExtractTags
                       (std::vector<std::pair<std::string, bool>>* features)  {
  std::vector<std::string> tags;
  if (features == NULL)
    return std::vector<std::string>();
  for(const std::pair<std::string, bool>& feature: *features)
    if (feature.second == true)  
        tags.push_back(feature.first);
  return tags;
}

/* Правило не подходит только в том случае, если оно содержит больше
 * тегов, чем требуется для данного слова.
 */
bool Petrovich::Rules::TagsAllow(std::vector<std::string> tags, 
                                                   const YAML::Node& rule_tags)  
                                {  return rule_tags.size() - tags.size() >= 0;}

Petrovich* Petrovich::Instance()  {
  if(!self_) self_ = new Petrovich();
  return self_;
}   
 
Petrovich::Rules* Petrovich::Rules::Instance()  {
  if(!self_) self_ = new Rules();
  return self_;
}                              
