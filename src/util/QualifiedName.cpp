#include "StarFishConfig.h"
#include "QualifiedName.h"
#include "StarFish.h"

namespace StarFish {

QualifiedName QualifiedName::fromString(StarFish* sf, String* str)
{
    return fromString(sf, str->utf8Data());
}

QualifiedName QualifiedName::fromString(StarFish* sf, const char* str)
{
    std::string data = str;
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    std::unordered_map<std::string, QualifiedName,
        std::hash<std::string>, std::equal_to<std::string>, gc_allocator<std::pair<std::string, QualifiedName>>>& map = sf->staticStrings()->m_staticStringMap;
    auto iter = map.find(data);

    if (iter != map.end()) {
        return iter->second;
    }

    String* s = String::fromUTF8(data.c_str());
    QualifiedName name(s);
    map.insert(std::make_pair(data, name));

    return name;
}


}
