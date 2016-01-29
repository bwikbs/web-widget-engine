#ifndef __StarFishQualifiedName__
#define __StarFishQualifiedName__

namespace StarFish {

class StarFish;

class QualifiedName {
    friend class StarFish;
    friend class StaticStrings;
    QualifiedName()
    {
        m_string = nullptr;
    }
    QualifiedName(String* str)
    {
        m_string = str;
    }
public:
    static QualifiedName fromString(StarFish* sf, String* str);
    static QualifiedName fromString(StarFish* sf, const char* str);

    static QualifiedName emptyQualifiedName()
    {
        return QualifiedName(String::emptyString);
    }


    bool operator==(const QualifiedName& src)
    {
        return m_string == src.m_string;
    }

    String* string()
    {
        return m_string;
    }

    operator String*()
    {
        return m_string;
    }

protected:
    String* m_string;
    // TODO add namespace..
};

}
#endif
