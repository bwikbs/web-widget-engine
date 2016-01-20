#ifndef __StarFishCSSParser__
#define __StarFishCSSParser__

#include "style/Style.h"

namespace StarFish {

class CSSTokenizer: public gc {
    //<ident-token>, <function-token>, <at-keyword-token>, <hash-token>, <string-token>, <bad-string-token>, <url-token>, <bad-url-token>, <delim-token>, <number-token>, <percentage-token>, <dimension-token>, <unicode-range-token>, <include-match-token>, <dash-match-token>, <prefix-match-token>, <suffix-match-token>, <substring-match-token>, <column-token>, <whitespace-token>, <CDO-token>, <CDC-token>, <colon-token>, <semicolon-token>, <comma-token>, <[-token>, <]-token>, <(-token>, <)-token>, <{-token>, and <}-token>
    enum TokenType {
    };
};

class CSSPropertyParser : public gc {
public:
    CSSPropertyParser(char* value):
        m_startPos(value),
        m_endPos(value + strlen(value)),
        m_curPos(value) {
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
};

}

#endif
