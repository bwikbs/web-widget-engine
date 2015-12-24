#ifndef __StarFishString__
#define __StarFishString__

namespace StarFish {

typedef std::basic_string<char, std::char_traits<char>, gc_allocator<char> > ASCIIString;
class StringDataASCII;
class String;

class String {
public:
    static String* createASCIIString(const char* src);

    ASCIIString* asASCIIString()
    {
        ASSERT(m_isASCIIString);
        return (ASCIIString*)((size_t)this + sizeof(size_t));
    }

    const char* utf8Data();

    bool equals(String* str)
    {
        if (*asASCIIString() == *str->asASCIIString()) {
            return true;
        }
        return false;
    }
protected:
    String()
    {
        m_isASCIIString = true;
    }

    size_t m_isASCIIString;
};

class StringDataASCII : public String, public ASCIIString, public gc {
public:
    StringDataASCII(const char* str)
        : ASCIIString(str)
    {
    }
};


inline String* String::createASCIIString(const char* str)
{
    return new StringDataASCII(str);
}

inline const char* String::utf8Data()
{
    ASSERT(m_isASCIIString);
    return asASCIIString()->data();
}

}
#endif
