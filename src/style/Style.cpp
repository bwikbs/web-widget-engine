#include "StarFishConfig.h"
#include "Style.h"


namespace StarFish {

bool startsWith(const char* base, const char* str)
{
    return (strstr(base, str) - base) == 0;
}

bool endsWith(const char* base, const char* str)
{
    int blen = strlen(base);
    int slen = strlen(str);
    return (blen >= slen) && (0 == strcmp(base + blen - slen, str));
}

#define VALUE_IS_STRING(str) \
    (strcmp(value,str)) == 0

#define VALUE_IS_INHERIT() \
    VALUE_IS_STRING("inherit")

void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value)
{
    if (endsWith(value, "%")) {
        float f;
        sscanf(value, "%f%%", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Percentage;
        ret.m_value.m_floatValue = f;
    } else if (endsWith(value, "px")) {
        float f;
        sscanf(value, "%fpx", &f);
        f = f / 100.f;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(f);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

CSSStyleValuePair CSSStyleValuePair::fromString(const char* key, const char* value)
{
    CSSStyleValuePair ret;
    if (strcmp(key, "display") == 0) {
        // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Display;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::DisplayValueKind;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_STRING("block")) {
            ret.m_value.m_display = DisplayValue::BlockDisplayValue;
        } else if (VALUE_IS_STRING("inline")) {
            ret.m_value.m_display = DisplayValue::InlineDisplayValue;
        } else if (VALUE_IS_STRING("none")) {
            ret.m_value.m_display = DisplayValue::NoneDisplayValue;
        }else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "width") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Width;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "height") == 0) {
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Height;
    } else if (strcmp(key, "font-size") == 0) {
        // absolute-size | relative-size | length | percentage | inherit // initial value -> medium
        //        X      |       X       |   O    |    O       |    O
        ret.m_keyKind = CSSStyleValuePair::KeyKind::FontSize;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "color") == 0) {
        // color | inherit // initial value -> depends on user agent
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Color;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else {
        STARFISH_LOG_ERROR("CSSStyleValuePair::fromString -> unsupport key = %s\n", key);
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    return ret;
}

}
