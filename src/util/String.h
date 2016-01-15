#ifndef __StarFishString__
#define __StarFishString__

namespace StarFish {

typedef std::basic_string<char, std::char_traits<char>, gc_allocator<char> > ASCIIString;
typedef std::basic_string<char32_t, std::char_traits<char32_t>, gc_allocator<char32_t> > UTF32String;
class StringDataASCII;
class String;

class String {
public:
    static String* emptyString;
    static String* spaceString;

    static String* fromUTF8(const char* src);
    static String* fromUTF8(const char* src, size_t len);
    static String* createASCIIString(const char* src);

    ASCIIString* asASCIIString()
    {
        STARFISH_ASSERT(m_isASCIIString);
        return (ASCIIString*)((size_t)this + sizeof(size_t));
    }

    UTF32String* asUTF32String()
    {
        STARFISH_ASSERT(!m_isASCIIString);
        return (UTF32String*)((size_t)this + sizeof(size_t));
    }

    const char* utf8Data();

    bool equals(String* str)
    {
        if (*asASCIIString() == *str->asASCIIString()) {
            return true;
        }
        return false;
    }

    bool equals(const char* str)
    {
        if (*asASCIIString() == str) {
            return true;
        }
        return false;
    }

    size_t length()
    {
        if (m_isASCIIString) {
            return asASCIIString()->length();
        } else {
            return asUTF32String()->length();
        }
    }

    char32_t charAt(size_t idx)
    {
        if (m_isASCIIString) {
            return (*asASCIIString())[idx];
        } else {
            return (*asUTF32String())[idx];
        }
    }

    char32_t operator[](size_t idx)
    {
        return charAt(idx);
    }

    size_t indexOf(char32_t ch)
    {
        for (size_t i = 0; i < length(); i ++) {
            if (charAt(i) == ch) {
                return i;
            }
        }
        return SIZE_MAX;
    }

    static inline bool isASCIISpace(char32_t c) { return c <= ' ' && (c == ' ' || (c <= 0xD && c >= 0x9)); }
    static inline bool isSpaceOrNewline(char32_t c)
    {
        // Use isASCIISpace() for basic Latin-1.
        // This will include newlines, which aren't included in Unicode DirWS.
        return c <= 0x7F ? isASCIISpace(c) : false; /* : u_charDirection(c) == U_WHITE_SPACE_NEUTRAL;*/
    }

    bool containsOnlyWhitespace()
    {
        for (size_t i = 0; i < length(); i ++) {
            if (!isASCIISpace(charAt(i))) {
                return false;
            }
        }
        return true;
    }

    String* substring(size_t pos, size_t len);

protected:
    String()
    {
        m_isASCIIString = true;
    }

    size_t m_isASCIIString;
};

class StringDataASCII : public String, public ASCIIString, public gc {
public:
    StringDataASCII(ASCIIString&& str)
        : ASCIIString(str)
    {

    }

    StringDataASCII(const char* str)
        : ASCIIString(str)
    {
    }

    StringDataASCII(const char* str, size_t len)
        : ASCIIString(str, &str[len])
    {
    }
};

class StringDataUTF32 : public String, public UTF32String, public gc {
public:
    StringDataUTF32(const char32_t* str)
        : UTF32String(str)
    {
    }
};

inline String* String::fromUTF8(const char* src, size_t len)
{
    for (unsigned i = 0; i < len; i ++) {
        if (src[i] < 0) {
            STARFISH_ASSERT_NOT_REACHED();
        }
    }
    return new StringDataASCII(src, len);
}

inline String* String::fromUTF8(const char* str)
{
    const char* p = str;
    while (*p) {
        if (*p < 0) {
            // TODO create utf32 string
            STARFISH_ASSERT_NOT_REACHED();
        }
        p++;
    }

    return new StringDataASCII(str);
}

inline String* String::createASCIIString(const char* str)
{
    return new StringDataASCII(str);
}

inline const char* String::utf8Data()
{
    STARFISH_ASSERT(m_isASCIIString);
    return asASCIIString()->data();
}

inline String* String::substring(size_t pos, size_t len)
{
    if (m_isASCIIString) {
        return new StringDataASCII(std::move(asASCIIString()->substr(pos, len)));
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

}
#endif
