#ifndef __StarFishCSSParser__
#define __StarFishCSSParser__

#include "style/Style.h"

namespace StarFish {

class CSSTokenizer: public gc {
    //<ident-token>, <function-token>, <at-keyword-token>, <hash-token>, <string-token>, <bad-string-token>, <url-token>, <bad-url-token>, <delim-token>, <number-token>, <percentage-token>, <dimension-token>, <unicode-range-token>, <include-match-token>, <dash-match-token>, <prefix-match-token>, <suffix-match-token>, <substring-match-token>, <column-token>, <whitespace-token>, <CDO-token>, <CDC-token>, <colon-token>, <semicolon-token>, <comma-token>, <[-token>, <]-token>, <(-token>, <)-token>, <{-token>, and <}-token>
    enum TokenType {
    };

public:
    static bool isDigit(char c)
    {
        if (c >= '0' && c <= '9')
            return true;
        return false;
    }

    static bool isAlpha(char c)
    {
        if (c >= 'a' && c <='z')
            return true;
        return false;
    }

    static bool isNameStart(char c)
    {
        if (isAlpha(c)) return true;
        if (c == '_') return true;
        //TODO
        return false;
    }

    static bool isNameChar(char c)
    {
        if (isNameStart(c)) return true;
        if (isDigit(c)) return true;
        if (c == '-') return true;
        return false;
    }

    static bool isUnitChar(char c)
    {
        if (c == '%') return true;
        return false;
    }
};

class CSSPropertyParser : public gc {
public:
    CSSPropertyParser(char* value)
        : m_startPos(value)
        , m_endPos(value + strlen(value))
        , m_curPos(value)
    {
    }

    bool consumeNumber()
    {
        float res = 0;
        bool sign = true; // +
        char* cur = m_curPos;
        if (*cur == '-') {
            sign = false;
            cur++;
        } else if (*cur == '+') {
            sign = true;
            cur++;
        }
        while (CSSTokenizer::isDigit(*cur) && cur < m_endPos) {
            res = res*10 + (*cur - '0');
            cur++;
        }
        if (cur == m_curPos)
            return false;
        if (*cur == '.' && cur < m_endPos) {
            cur++;
            int pt = 10;
            while (CSSTokenizer::isDigit(*cur) && cur < m_endPos) {
                res += (float)(*cur - '0')/pt;
                pt *= 10;
                cur++;
            }
        }

        m_curPos = cur;
        if (!sign) res *= (-1);
        m_parsedNumber = res;
        return true;
    }

    float parsedNumber() { return m_parsedNumber; }

    bool consumeString()
    {
        int len = 0;
        for (char* cur = m_curPos; cur < m_endPos; cur++, len++) {
            if (!(CSSTokenizer::isNameChar(*cur) || CSSTokenizer::isUnitChar(*cur)))
                break;
        }
        m_parsedString = String::fromUTF8(m_curPos, len);
        m_curPos += len;
        return true;
    }

    String* parsedString() { return m_parsedString; }

    bool isEnd() {
        return (m_curPos == m_endPos);
    }

    static bool assureLength(const char* token, bool allowNegative)
    {
        CSSPropertyParser* parser = new CSSPropertyParser((char*) token);
        if (!parser->consumeNumber()) return false;
        float num = parser->parsedNumber();
        if (!allowNegative && num < 0) return false;
        if (parser->consumeString()) {
            String* str = parser->parsedString();
            if (str->equals("px")
                || str->equals("em")
                || str->equals("ex")
                || str->equals("in")
                || str->equals("cm")
                || str->equals("mm")
                || str->equals("pt")
                || str->equals("pc"))
                return parser->isEnd();
            return false;
        } else {
            // After a zero length, the unit identifier is optional
            if (num == 0) return true;
            return false;
        }
        return true;
    }

    static bool assurePercent(const char* token, bool allowNegative)
    {
        CSSPropertyParser* parser = new CSSPropertyParser((char*) token);
        if (!parser->consumeNumber()) return false;
        float num = parser->parsedNumber();
        if (!allowNegative && num < 0) return false;
        if (parser->consumeString()) {
            String* str = parser->parsedString();
            if (str->equals("%"))
                return parser->isEnd();
            return false;
        } else {
            // After a zero length, the unit identifier is optional
            if (num == 0) return true;
            return false;
        }
        return true;
    }

    static bool assureNumber(const char* token, bool allowNegative)
    {
        CSSPropertyParser* parser = new CSSPropertyParser((char*) token);
        if (!parser->consumeNumber()) return false;
        float num = parser->parsedNumber();
        if (!allowNegative && num < 0) return false;
        return parser->isEnd();
    }

    static bool assureInteger(const char* token, bool allowNegative)
    {
        CSSPropertyParser* parser = new CSSPropertyParser((char*) token);
        if (!parser->consumeNumber()) return false;
        float num = parser->parsedNumber();
        if (num != std::floor(num)) return false;
        return parser->isEnd();
    }

    static bool assureColor(const char* token)
    {
        if (strcmp(token, "red") == 0 ||
            strcmp(token, "green") == 0 ||
            strcmp(token, "yellow") == 0 ||
            strcmp(token, "blue") == 0 ||
            strcmp(token, "purple") == 0 ||
            strcmp(token, "black") == 0)
            return true;
        return false;
    }

    static bool assureEssential(const char* token)
    {
        // initial || inherit || none || empty string
        if (strcmp(token, "initial") == 0 ||
            strcmp(token, "inherit") == 0)
            return true;
        return false;
    }

    static char* getNextSingleValue(char* str) {
        char* next = NULL;
        while( (next = strchr(str, ' ')) ) {
            if (next == str) {
                str++;
                continue;
            }
            *next = '\0';
            return str;
        }
        return str;
    }

    bool findNextValueKind(char separator, CSSStyleValuePair::ValueKind* kind) {
        if (m_endPos <= m_curPos) return false;
        char* nextSep = strchr(m_curPos, separator);
        if (nextSep == NULL)
            nextSep = m_curPos + strlen(m_curPos);
        if (strncmp(m_curPos, "auto", 4) == 0) {
            *kind = CSSStyleValuePair::ValueKind::Auto;
        } else if (strncmp(nextSep-1, "%", 1) == 0) {
            *kind = CSSStyleValuePair::ValueKind::Percentage;
            float f;
            sscanf(m_curPos, "%f%%", &f);
            f = f / 100.f;
            m_parsedFloatValue = f;
        } else if (strncmp(nextSep-2, "px", 1) == 0) {
            *kind = CSSStyleValuePair::ValueKind::Length;
            float f;
            sscanf(m_curPos, "%fpx", &f);
            m_parsedFloatValue = f;
        }
        m_curPos = nextSep + 1;
        return true;
    }

    float parsedFloatValue() {
        return m_parsedFloatValue;
    }

    char* m_startPos;
    char* m_endPos;
    char* m_curPos;
    float m_parsedFloatValue;

    float m_parsedNumber;
    String* m_parsedString;
};

}

#endif
