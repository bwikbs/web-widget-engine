#include "StarFishConfig.h"
#include "String.h"
#include <sstream>

namespace StarFish {

String* String::emptyString = String::createASCIIString("");
String* String::spaceString = String::createASCIIString(" ");

size_t utf8ToUtf32(const char* UTF8, char32_t& uc)
{
    size_t tRequiredSize = 0;

    uc = 0x0000;

    // ASCII byte
    if (0 == (UTF8[0] & 0x80)) {
        uc = UTF8[0];
        tRequiredSize = 1;
    } else {
        // Start byte for 2byte
        if (0xC0 == (UTF8[0] & 0xE0)
            && 0x80 == (UTF8[1] & 0xC0)) {
            uc += (UTF8[0] & 0x1F) << 6;
            uc += (UTF8[1] & 0x3F) << 0;
            tRequiredSize = 2;
        } else { // Start byte for 3byte
            if (0xE0 == (UTF8[0] & 0xE0)
                && 0x80 == (UTF8[1] & 0xC0)
                && 0x80 == (UTF8[2] & 0xC0)) {
                uc += (UTF8[0] & 0x1F) << 12;
                uc += (UTF8[1] & 0x3F) << 6;
                uc += (UTF8[2] & 0x3F) << 0;
                tRequiredSize = 3;
            } else {
                // TODO implement 4-byte case
                // Invalid case
                tRequiredSize = 1;
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        }
    }

    return tRequiredSize;
}

size_t utf32ToUtf8(char32_t uc, char* UTF8)
{
    size_t tRequiredSize = 0;

    if (uc <= 0x7f) {
        if (NULL != UTF8) {
            UTF8[0] = (char)uc;
            UTF8[1] = (char)'\0';
        }
        tRequiredSize = 1;
    } else if (uc <= 0x7ff) {
        if (NULL != UTF8) {
            UTF8[0] = (char)(0xc0 + uc / (0x01 << 6));
            UTF8[1] = (char)(0x80 + uc % (0x01 << 6));
            UTF8[2] = (char)'\0';
        }
        tRequiredSize = 2;
    } else if (uc <= 0xffff) {
        if (NULL != UTF8) {
            UTF8[0] = (char)(0xe0 + uc / (0x01 << 12));
            UTF8[1] = (char)(0x80 + uc / (0x01 << 6) % (0x01 << 6));
            UTF8[2] = (char)(0x80 + uc % (0x01 << 6));
            UTF8[3] = (char)'\0';
        }
        tRequiredSize = 3;
    } else {
        // TODO
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    return tRequiredSize;
}

const char* utf32ToUtf8(const char32_t* t, const size_t& len, size_t* bufferSize = NULL)
{
    unsigned strLength = 0;
    char buffer[MB_CUR_MAX];
    for (size_t i = 0; i < len; i++) {
        int length = utf32ToUtf8(t[i], buffer);
        strLength += length;
    }

    char* result = (char*)GC_MALLOC_ATOMIC(strLength + 1);
    if (bufferSize)
        *bufferSize = strLength + 1;
    unsigned currentPosition = 0;

    for (size_t i = 0; i < len; i++) {
        int length = utf32ToUtf8(t[i], buffer);
        memcpy(&result[currentPosition], buffer, length);
        currentPosition += length;
    }
    result[strLength] = 0;

    return result;
}

StringDataUTF32::StringDataUTF32(const char* src, size_t len)
{
    m_isASCIIString = false;
    const char* end = src + len;
    while (end != src) {
        char32_t c;
        src += utf8ToUtf32(src, c);
        UTF32String::operator+=(c);
    }
}

const char* String::utf8DataSlowCase()
{
    STARFISH_ASSERT(!m_isASCIIString);
    return utf32ToUtf8(asUTF32String()->data(), asUTF32String()->length());
}

String* String::fromUTF8(const char* src, size_t len)
{
    for (unsigned i = 0; i < len; i ++) {
        if (src[i] < 0) {
            return new StringDataUTF32(src, len);
        }
    }
    return new StringDataASCII(src, len);
}

String* String::fromUTF8(const char* str)
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

String* String::createASCIIString(const char* str)
{
    return new StringDataASCII(str);
}

const char* String::utf8Data()
{
    if (m_isASCIIString) {
        return asASCIIString()->data();
    } else {
        return utf8DataSlowCase();
    }
}

String* String::substring(size_t pos, size_t len)
{
    if (m_isASCIIString) {
        return new StringDataASCII(std::move(asASCIIString()->substr(pos, len)));
    } else {
        return new StringDataUTF32(std::move(asUTF32String()->substr(pos, len)));
    }
}

String* String::toUpper()
{
    if (m_isASCIIString) {
        ASCIIString str = *asASCIIString();
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return new StringDataASCII(std::move(str));
    } else {
        UTF32String str = *asUTF32String();
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return new StringDataUTF32(std::move(str));
    }
}

String* String::toLower()
{
    if (m_isASCIIString) {
        ASCIIString str = *asASCIIString();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return new StringDataASCII(std::move(str));
    } else {
        UTF32String str = *asUTF32String();
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return new StringDataUTF32(std::move(str));
    }
}

String* String::concat(String* str)
{
    if (isASCIIString() && str->isASCIIString()) {
        ASCIIString s = *asASCIIString() + *(str->asASCIIString());
        return new StringDataASCII(std::move(s));
    } else {
        UTF32String a = toUTF32String();
        UTF32String b = toUTF32String();
        a = a + b;
        return new StringDataUTF32(std::move(a));
    }
}

String* String::fromFloat(float f)
{
    if (f == std::floor(f))
        return String::fromUTF8(std::to_string((int)std::floor(f)).c_str());

    return String::fromUTF8(std::to_string(f).c_str());
}

void String::split(char delim, Vector& tokens)
{
    if (m_isASCIIString) {
        std::stringstream ss;
        ss << utf8Data();
        std::string item;
        while (std::getline(ss, item, delim)) {
            tokens.push_back(String::fromUTF8(item.c_str()));
        }
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

String* String::trim()
{
    size_t first = 0;
    size_t last = 0;
    if (length()) {
        last = length() - 1;

        for (size_t i = 0; i < length(); i ++) {
            if (!String::isSpaceOrNewline(charAt(i))) {
                first = i;
                break;
            }
        }

        do {
            if (!String::isSpaceOrNewline(charAt(last)))
                break;
        } while (last--);
    }

    return substring(first, (last - first + 1));
}

std::vector<String*, gc_allocator<String*> > String::tokenize(const char* tokens, size_t tokensLength)
{
    std::vector<String*, gc_allocator<String*> > result;
    const char* data = utf8Data();
    size_t length = strlen(data);

    std::string str;
    for (size_t i = 0; i < length; i++) {
        char c = data[i];
        bool isToken = false;
        for (size_t j = 0; j < tokensLength; j++) {
            if (c == tokens[j]) {
                isToken = true;
                break;
            }
        }

        if (isToken) {
            result.push_back(String::fromUTF8(str.data(), str.length()));
            str.clear();
        } else {
            str += c;
        }
    }

    if (str.length()) {
        result.push_back(String::fromUTF8(str.data(), str.length()));
    }

    return result;
}


UTF32String String::toUTF32String()
{
    if (m_isASCIIString) {
        UTF32String str;
        const ASCIIString& src = *asASCIIString();
        size_t len = src.length();
        for (size_t i = 0; i < len; i ++) {
            str.push_back(src[i]);
        }
        return str;
    } else {
        return *asUTF32String();
    }
}

bool String::equals(const String* str) const
{
    size_t lenA = length();
    size_t lenB = str->length();
    if (lenA == lenB) {
        bool aa = isASCIIString();
        bool bb = str->isASCIIString();
        if (aa && bb) {
            return stringEqual(asASCIIString()->data(), str->asASCIIString()->data(), lenA);
        } else if (aa && !bb) {
            return stringEqual(str->asUTF32String()->data(), asASCIIString()->data(), lenA);
        } else if (!aa && bb) {
            return stringEqual(asUTF32String()->data(), str->asASCIIString()->data(), lenA);
        } else {
            return stringEqual(asUTF32String()->data(), str->asUTF32String()->data(), lenA);
        }
    }
    return false;
}


}
