#ifndef __StarFishCSSParser__
#define __StarFishCSSParser__

namespace StarFish {

class CSSPropertyParser : public gc {
public:
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
};

}

#endif
