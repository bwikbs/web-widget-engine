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

    ASCIIString* asASCIIString() const
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

    bool equals(const String* str) const
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

    String* toUpper();
    bool isASCIIString() { return m_isASCIIString; }
    String* concat(String* str);

protected:
    const char* utf8DataSlowCase();
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
    StringDataUTF32(UTF32String&& str)
        : UTF32String(str)
    {
        m_isASCIIString = false;
    }
    StringDataUTF32(const char* src, size_t len);
    StringDataUTF32(const char32_t* str)
        : UTF32String(str)
    {
        m_isASCIIString = false;
    }
};

inline String* String::fromUTF8(const char* src, size_t len)
{
    for (unsigned i = 0; i < len; i ++) {
        if (src[i] < 0) {
            return new StringDataUTF32(src, len);
        }
    }
    return new StringDataASCII(src, len);
}

inline String* String::fromUTF8(const char* str)
{
    const char* p = str;
    while (*p) {
        if (*p < 0) {
            return new StringDataUTF32(str, strlen(str));
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
    if (m_isASCIIString) {
        return asASCIIString()->data();
    } else {
        return utf8DataSlowCase();
    }
}

inline String* String::substring(size_t pos, size_t len)
{
    if (m_isASCIIString) {
        return new StringDataASCII(std::move(asASCIIString()->substr(pos, len)));
    } else {
        return new StringDataUTF32(std::move(asUTF32String()->substr(pos, len)));
    }
}

inline String* String::toUpper()
{
    if (m_isASCIIString) {
        ASCIIString str = *asASCIIString();
        std::transform(str.begin(), str.end(),str.begin(), ::toupper);
        return new StringDataASCII(std::move(str));
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

inline String* String::concat(String* str)
{
    if(isASCIIString() && str->isASCIIString()) {
        ASCIIString s = *asASCIIString() + *(str->asASCIIString());
        return new StringDataASCII(std::move(s));
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

}
#endif
