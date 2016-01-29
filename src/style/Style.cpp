#include "StarFishConfig.h"
#include "Style.h"
#include "ComputedStyle.h"
#include "layout/Frame.h"
#include "dom/Element.h"
#include "dom/Document.h"
#include "CSSParser.h"
#include "dom/DOMTokenList.h"

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

bool endsWithNumber(const char* str)
{
    const char* lastCh = (str + strlen(str) - 1);
    return *lastCh >= '0' && *lastCh <= '9';
}

#define VALUE_IS_STRING(str) \
    (strcmp(value,str)) == 0

#define VALUE_IS_INHERIT() \
    VALUE_IS_STRING("inherit")

#define VALUE_IS_INITIAL() \
    VALUE_IS_STRING("initial")

#define VALUE_IS_NONE() \
    VALUE_IS_STRING("none")

CSSLength parseCSSLength(const char* value)
{
    float f;
    if (endsWith(value, "px")) {
        sscanf(value, "%fpx", &f);
        return CSSLength(f);
    } else if (endsWith(value, "em")) {
        sscanf(value, "%fem", &f);
        return CSSLength(CSSLength::Kind::EM, f);
    } else if (endsWith(value, "ex")) {
        sscanf(value, "%fex", &f);
        return CSSLength(CSSLength::Kind::EX, f);
    } else if (endsWith(value, "in")) {
        sscanf(value, "%fin", &f);
        return CSSLength(CSSLength::Kind::IN, f);
    } else if (endsWith(value, "cm")) {
        sscanf(value, "%fcm", &f);
        return CSSLength(CSSLength::Kind::CM, f);
    } else if (endsWith(value, "mm")) {
        sscanf(value, "%fmm", &f);
        return CSSLength(CSSLength::Kind::MM, f);
    } else if (endsWith(value, "pt")) {
        sscanf(value, "%fpt", &f);
        return CSSLength(CSSLength::Kind::PT, f);
    } else if (endsWith(value, "pc")) {
        sscanf(value, "%fpc", &f);
        return CSSLength(CSSLength::Kind::PC, f);
    } else if (strcmp(value, "0")) {
        return CSSLength(0.0);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}
void parseLength(CSSStyleValuePair& ret, const char* value)
{
    float f;
    if (endsWith(value, "px")) {
        sscanf(value, "%fpx", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(f);
    } else if (endsWith(value, "em")) {
        sscanf(value, "%fem", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::EM, f);
    } else if (endsWith(value, "ex")) {
        sscanf(value, "%fex", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::EX, f);
    } else if (endsWith(value, "in")) {
        sscanf(value, "%fin", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::IN, f);
    } else if (endsWith(value, "cm")) {
        sscanf(value, "%fcm", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::CM, f);
    } else if (endsWith(value, "mm")) {
        sscanf(value, "%fmm", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::MM, f);
    } else if (endsWith(value, "pt")) {
        sscanf(value, "%fpt", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::PT, f);
    } else if (endsWith(value, "pc")) {
        sscanf(value, "%fpc", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(CSSLength::Kind::PC, f);
    } else if (strcmp(value, "0")) {
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(0.0);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value)
{
    if (endsWith(value, "%")) {
        float f;
        sscanf(value, "%f%%", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Percentage;
        f = f / 100.f;
        ret.m_value.m_floatValue = f;
    } else {
        parseLength(ret, value);
    }
}

const char* removeWhiteSpace(const char* s) {
    size_t length = strlen(s);
    std::string str;
    for (size_t i = 0; i < length; i++) {
        if (s[i] != ' ') {
            str += s[i];
        }
    }

    return str.c_str();
}

String* parseUrl(String* value)
{
    const char* str = value->utf8Data();

    if (startsWith(str, "url(") && endsWith(str, ")")) {
        const char* trimStr = removeWhiteSpace(str);
        size_t pathlen = strlen(trimStr);
        String* ret;

        if(startsWith(trimStr, "url(\"") || startsWith(trimStr, "url('")) {
            ret = String::fromUTF8(trimStr)->substring(5, pathlen - 7);
        } else {
            ret = String::fromUTF8(trimStr)->substring(4, pathlen - 5);
        }
        return ret;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return 0;
    }
}

static const int strictFontSizeTable[8][8] =
{
        { 9,    9,     9,     9,    11,    14,    18,    27 },
        { 9,    9,     9,    10,    12,    15,    20,    30 },
        { 9,    9,    10,    11,    13,    17,    22,    33 },
        { 9,    9,    10,    12,    14,    18,    24,    36 },
        { 9,   10,    12,    13,    16,    20,    26,    39 }, // fixed font default (13)
        { 9,   10,    12,    14,    17,    21,    28,    42 },
        { 9,   10,    13,    15,    18,    23,    30,    45 },
        { 9,   10,    13,    16,    18,    24,    32,    48 }  // proportional font default (16)
};
static const float fontSizeFactors[8] = { 0.60f, 0.75f, 0.89f, 1.0f, 1.2f, 1.5f, 2.0f, 3.0f };

const int fontSizeTableMax = 16;
const int fontSizeTableMin = 9;

Length parseAbsoluteFontSize(int col) {
    int mediumSize = DEFAULT_FONT_SIZE;
    int row = -1;
    if (mediumSize >= fontSizeTableMin && mediumSize <= fontSizeTableMax)
        row =  mediumSize - fontSizeTableMin;

    if (row >= 0)
        return Length(Length::Fixed, strictFontSizeTable[row][col]);
    else
        return Length(Length::Fixed, fontSizeFactors[col] * mediumSize);
}

Length convertValueToLength(CSSStyleValuePair::ValueKind kind, CSSStyleValuePair::ValueData data)
{
    if (kind == CSSStyleValuePair::ValueKind::Auto)
        return Length();
    else if (kind == CSSStyleValuePair::ValueKind::Length)
        return data.m_length.toLength();
    else if (kind == CSSStyleValuePair::ValueKind::Percentage)
        return Length(Length::Percent, data.m_floatValue);
    else
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
}

Length convertPercentOrNumberToLength(CSSStyleValuePair::ValueKind kind, CSSStyleValuePair::ValueData data)
{
    if (kind == CSSStyleValuePair::ValueKind::Percentage)
        return Length(Length::Percent, data.m_floatValue);
    else if (kind == CSSStyleValuePair::ValueKind::Number)
        return Length(Length::Fixed, data.m_floatValue);
    else
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
}

void CSSStyleValuePair::setValueColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        // TODO check string has right color string
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueDirection(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // <ltr> | rtl | inherit
    // TODO add initial
    m_keyKind = CSSStyleValuePair::KeyKind::Direction;
    m_valueKind = CSSStyleValuePair::ValueKind::DirectionValueKind;
    if (VALUE_IS_STRING("ltr")) {
        m_value.m_direction = DirectionValue::LtrDirectionValue;
    } else if (VALUE_IS_STRING("rtl")) {
        m_value.m_direction = DirectionValue::RtlDirectionValue;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValueWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Width;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;

    if (VALUE_IS_STRING("auto")) {
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueHeight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Height;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;

    if (VALUE_IS_STRING("auto")) {
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueFontSize(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // TODO add initial
    // absolute-size | relative-size | length | percentage | inherit // initial value -> medium
    //        O      |       O       |   O    |    O       |    O
    m_keyKind = CSSStyleValuePair::KeyKind::FontSize;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    }  else if (VALUE_IS_STRING("xx-small")) {
        m_valueKind = CSSStyleValuePair::ValueKind::XXSmallFontSizeValueKind;
    } else if (VALUE_IS_STRING("x-small")) {
        m_valueKind = CSSStyleValuePair::ValueKind::XSmallFontSizeValueKind;
    } else if (VALUE_IS_STRING("small")) {
        m_valueKind = CSSStyleValuePair::ValueKind::SmallFontSizeValueKind;
    } else if (VALUE_IS_STRING("medium")) {
        m_valueKind = CSSStyleValuePair::ValueKind::MediumFontSizeValueKind;
    } else if (VALUE_IS_STRING("large")) {
        m_valueKind = CSSStyleValuePair::ValueKind::LargeFontSizeValueKind;
    } else if (VALUE_IS_STRING("x-large")) {
        m_valueKind = CSSStyleValuePair::ValueKind::XLargeFontSizeValueKind;
    } else if (VALUE_IS_STRING("xx-large")) {
        m_valueKind = CSSStyleValuePair::ValueKind::XXLargeFontSizeValueKind;
    } else if (VALUE_IS_STRING("larger")) {
        m_valueKind = CSSStyleValuePair::ValueKind::LargerFontSizeValueKind;
    } else if (VALUE_IS_STRING("smaller")) {
        m_valueKind = CSSStyleValuePair::ValueKind::SmallerFontSizeValueKind;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueFontStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // <normal> | italic | oblique | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::FontStyle;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("normal")) {
        m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
        m_value.m_fontStyle = FontStyleValue::NormalFontStyleValue;
    } else if (VALUE_IS_STRING("italic")) {
        m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
        m_value.m_fontStyle = FontStyleValue::ItalicFontStyleValue;
    } else if (VALUE_IS_STRING("oblique")) {
        m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
        m_value.m_fontStyle = FontStyleValue::ObliqueFontStyleValue;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValueDisplay(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Display;
    m_valueKind = CSSStyleValuePair::ValueKind::DisplayValueKind;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if(VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("block")) {
        m_value.m_display = DisplayValue::BlockDisplayValue;
    } else if (VALUE_IS_STRING("inline")) {
        m_value.m_display = DisplayValue::InlineDisplayValue;
    } else if (VALUE_IS_STRING("inline-block")) {
        m_value.m_display = DisplayValue::InlineBlockDisplayValue;
    } else if (VALUE_IS_STRING("none")) {
        m_value.m_display = DisplayValue::NoneDisplayValue;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValuePosition(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // <static> | relative | absolute | fixed | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Position;
    m_valueKind = CSSStyleValuePair::ValueKind::PositionValueKind;

    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("static")) {
        m_value.m_position = PositionValue::StaticPositionValue;
    } else if (VALUE_IS_STRING("relative")) {
        m_value.m_position = PositionValue::RelativePositionValue;
    } else if (VALUE_IS_STRING("absolute")) {
        m_value.m_position = PositionValue::AbsolutePositionValue;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValueTextDecoration(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // none | [ underline || overline || line-through || blink ] | inherit // Initial value -> none
    m_keyKind = CSSStyleValuePair::KeyKind::TextDecoration;
    m_valueKind = CSSStyleValuePair::ValueKind::TextDecorationKind;

    if(VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_NONE()) {
        m_valueKind = CSSStyleValuePair::ValueKind::None;
        m_value.m_textDecoration = TextDecorationValue::NoneTextDecorationValue;
    } else if (VALUE_IS_STRING("underline")) {
        m_value.m_textDecoration = TextDecorationValue::UnderLineTextDecorationValue;
    } else if (VALUE_IS_STRING("overline")) {
        m_value.m_textDecoration = TextDecorationValue::OverLineTextDecorationValue;
    } else if (VALUE_IS_STRING("line-through")) {
        m_value.m_textDecoration = TextDecorationValue::LineThroughTextDecorationValue;
    } else if (VALUE_IS_STRING("blink")) {
        m_value.m_textDecoration = TextDecorationValue::BlinkTextDecorationValue;
    }
}

void CSSStyleValuePair::setValueBackgroundColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    setValueColor(tokens);
}

void CSSStyleValuePair::setValueBackgroundImage(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* value = tokens->at(0)->utf8Data();

        if (VALUE_IS_NONE()) {
            m_valueKind = CSSStyleValuePair::ValueKind::None;
        } else if (VALUE_IS_INITIAL()) {
            m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (tokens->size() == 2) {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;

        String* sum = tokens->at(0);
        m_value.m_stringValue = sum->concat(tokens->at(1));
    } else if (tokens->size() == 3) {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;

        String* sum = tokens->at(0);
        m_value.m_stringValue = sum->concat(tokens->at(1))->concat(tokens->at(2));

    }
}

void CSSStyleValuePair::setValuePercentageOrLength(const char* value)
{
    if (VALUE_IS_STRING("auto")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Auto;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        parsePercentageOrLength(*this, value);
    }
}

void CSSStyleValuePair::setValueLetterSpacing(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // normal | length | inherit
    const char* value = tokens->at(0)->utf8Data();
    m_keyKind = CSSStyleValuePair::KeyKind::LetterSpacing;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("normal")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Normal;
    } else {
        parseLength(*this, value);
    }
}

void CSSStyleValuePair::setValueLineHeight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // <normal> | number | length | percentage | inherit
    const char* value = tokens->at(0)->utf8Data();
    m_valueKind = CSSStyleValuePair::ValueKind::Normal;
    if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("inherit")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_STRING("normal")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Normal;
    } else {
        char* pEnd;
        double d = strtod (value, &pEnd);
        if (pEnd == value + strlen(value)) {
            m_valueKind = CSSStyleValuePair::ValueKind::Number;
            m_value.m_floatValue = d;
        } else {
            parsePercentageOrLength(*this, value);
        }
    }
}

void CSSStyleValuePair::setValuePaddingTop(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    setValuePercentageOrLength(value);
}

void CSSStyleValuePair::setValuePaddingRight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    setValuePercentageOrLength(value);
}

void CSSStyleValuePair::setValuePaddingBottom(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    setValuePercentageOrLength(value);
}

void CSSStyleValuePair::setValuePaddingLeft(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    setValuePercentageOrLength(value);
}

void CSSStyleValuePair::setValueMarginTop(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    setValuePercentageOrLength(value);
}

void CSSStyleValuePair::setValueMarginRight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    setValueMarginTop(tokens);
}

void CSSStyleValuePair::setValueMarginBottom(std::vector<String*, gc_allocator<String*>>* tokens)
{
    setValueMarginTop(tokens);
}

void CSSStyleValuePair::setValueMarginLeft(std::vector<String*, gc_allocator<String*>>* tokens)
{
    setValueMarginTop(tokens);
}

void CSSStyleValuePair::setValueTop(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Top;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;

    if (VALUE_IS_STRING("auto")) {
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueBottom(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Bottom;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;

    if (VALUE_IS_STRING("auto")) {
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueLeft(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Left;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;
    if (VALUE_IS_STRING("auto")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Auto;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueRight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // length | percentage | <auto> | inherit
    m_keyKind = CSSStyleValuePair::KeyKind::Right;
    m_valueKind = CSSStyleValuePair::ValueKind::Auto;

    if (VALUE_IS_STRING("auto")) {
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueBorderImageSlice(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // number | percentage {1,4} && fill?
    m_keyKind = CSSStyleValuePair::KeyKind::BorderImageSlice;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
        m_value.m_multiValue = new ValueList();
        for (unsigned int i = 0; i < tokens->size(); i++) {
            const char* currentToken = tokens->at(i)->toLower()->utf8Data();
            if (startsWith(currentToken, "fill")) {
                m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::StringValueKind, {0});
            } else if (endsWith(currentToken, "%")) {
                float f;
                sscanf(currentToken, "%f%%", &f);
                m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::Percentage, {.m_floatValue = (f / 100.f)});
            } else {
                char* pEnd;
                double d = strtod (currentToken, &pEnd);
                STARFISH_ASSERT(pEnd == currentToken + tokens->at(i)->length());
                m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::Number, {.m_floatValue = (float)d});
            }
        }
    }
}

void CSSStyleValuePair::setValueBorderImageSource(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // none | <image>
    m_keyKind = CSSStyleValuePair::KeyKind::BorderImageSource;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if(VALUE_IS_STRING("none")) {
        m_valueKind = CSSStyleValuePair::ValueKind::None;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueBorderImageRepeat(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // <stretch> | repeat | round | space {1,2}
    m_keyKind = CSSStyleValuePair::KeyKind::BorderImageRepeat;
    if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
        m_value.m_multiValue = new ValueList();

        // TODO: find better way to parse axis data
        // 1) parse X-axis data
        if (startsWith(value, "stretch")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = StretchValue});
        } else if (startsWith(value, "repeat")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RepeatValue});
        } else if (startsWith(value, "round")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RoundValue});
        } else if (startsWith(value, "space")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = SpaceValue});
        }
        // 2) parse Y-axis data
        if (endsWith(value, "stretch")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = StretchValue});
        } else if (endsWith(value, "repeat")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RepeatValue});
        } else if (endsWith(value, "round")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RoundValue});
        } else if (endsWith(value, "space")) {
            m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = SpaceValue});
        }
    }
}

void CSSStyleValuePair::setValueBorderImageWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // [length | percentage | number | auto] {1, 4}
    m_keyKind = CSSStyleValuePair::KeyKind::BorderImageWidth;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
        ValueList* values = new ValueList();
        for (unsigned int i = 0; i < tokens->size(); i++) {
            const char* currentToken = tokens->at(i)->utf8Data();
            if (strcmp(currentToken, "auto") == 0) {
                values->append(CSSStyleValuePair::ValueKind::Auto, {0});
            } else if (endsWith(currentToken, "%")) {
                float f;
                sscanf(currentToken, "%f%%", &f);
                values->append(CSSStyleValuePair::ValueKind::Percentage, {.m_floatValue = (f / 100.f)});
            } else if (endsWithNumber(currentToken)) {
                char* pEnd;
                double d = strtod (currentToken, &pEnd);
                STARFISH_ASSERT(pEnd == currentToken + tokens->at(i)->length());
                values->append(CSSStyleValuePair::ValueKind::Number, {.m_floatValue = (float)d});
            } else {
                CSSStyleValuePair::ValueData data = {.m_length = parseCSSLength(currentToken)};
                values->append(CSSStyleValuePair::ValueKind::Length, data);
            }
        }
        m_value.m_multiValue = values;
    }
}

void CSSStyleValuePair::setValueBorderTopColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // color | transparent | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("transparent")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueBorderRightColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // color | transparent | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("transparent")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueBorderBottomColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // color | transparent | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("transparent")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueBorderLeftColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // color | transparent | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("transparent")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
    } else {
        m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
        m_value.m_stringValue = String::fromUTF8(value);
    }
}

void CSSStyleValuePair::setValueBorderTopStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-style(<none> | solid) | inherit
    m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("solid")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
    }
}

void CSSStyleValuePair::setValueBorderRightStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-style(<none> | solid) | inherit
    m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("solid")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
    }
}

void CSSStyleValuePair::setValueBorderBottomStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-style(<none> | solid) | inherit
    m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("solid")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
    }
}

void CSSStyleValuePair::setValueBorderLeftStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-style(<none> | solid) | inherit
    m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("solid")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
    }
}

void CSSStyleValuePair::setValueBorderTopWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-width(thin | <medium> | thick | length) | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("thin")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
    } else if (VALUE_IS_STRING("medium")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
    } else if (VALUE_IS_STRING("thick")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueBorderRightWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-width(thin | <medium> | thick | length) | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("thin")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
    } else if (VALUE_IS_STRING("medium")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
    } else if (VALUE_IS_STRING("thick")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueBorderBottomWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-width(thin | <medium> | thick | length) | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("thin")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
    } else if (VALUE_IS_STRING("medium")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
    } else if (VALUE_IS_STRING("thick")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueBorderLeftWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    // border-width(thin | <medium> | thick | length) | inherit
    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("thin")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
    } else if (VALUE_IS_STRING("medium")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
    } else if (VALUE_IS_STRING("thick")) {
        m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
    } else {
        setValuePercentageOrLength(value);
    }
}

void CSSStyleValuePair::setValueTextAlign(std::vector<String*, gc_allocator<String*>>* tokens)
{

    const char* value = tokens->at(0)->utf8Data();
    m_keyKind = CSSStyleValuePair::KeyKind::TextAlign;
    m_valueKind = CSSStyleValuePair::ValueKind::Inherit;

    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    }  else if (VALUE_IS_STRING("left")) {
        m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
        m_value.m_textAlign = TextAlignValue::LeftTextAlignValue;
    } else if (VALUE_IS_STRING("center")) {
        m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
        m_value.m_textAlign = TextAlignValue::CenterTextAlignValue;
    } else if (VALUE_IS_STRING("right")) {
        m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
        m_value.m_textAlign = TextAlignValue::RightTextAlignValue;
    } else if (VALUE_IS_STRING("justify")) {
        m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
        m_value.m_textAlign = TextAlignValue::JustifyTextAlignValue;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValueVisibility(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();

    m_keyKind = CSSStyleValuePair::KeyKind::Visibility;
    m_valueKind = CSSStyleValuePair::ValueKind::VisibilityKind;

    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("visible")) {
        m_value.m_visibility = VisibilityValue::VisibleVisibilityValue;
    } else if (VALUE_IS_STRING("hidden")){
        m_value.m_visibility = VisibilityValue::HiddenVisibilityValue;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

void CSSStyleValuePair::setValueOpacity(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();

    m_keyKind = CSSStyleValuePair::KeyKind::Opacity;
    m_valueKind = CSSStyleValuePair::ValueKind::Number;

    if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else {
        sscanf(value, "%f%%", &m_value.m_floatValue);
        if(m_value.m_floatValue>1.0)
            m_value.m_floatValue=1.0;
    }
}

void CSSStyleValuePair::setValueOverflowX(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();

    m_keyKind = CSSStyleValuePair::KeyKind::OverflowX;
    m_valueKind = CSSStyleValuePair::ValueKind::OverflowValueKind;

    if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_STRING("auto") || VALUE_IS_STRING("scroll")) {
        // Not Supported!!
    } else if (VALUE_IS_STRING("visible")) {
        m_value.m_overflowX = OverflowValue::VisibleOverflow;
    } else if (VALUE_IS_STRING("hidden")){
        m_value.m_overflowX = OverflowValue::HiddenOverflow;
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}
void CSSStyleValuePair::setValueZIndex(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    if (VALUE_IS_STRING("auto")) {
        m_valueKind = CSSStyleValuePair::ValueKind::Auto;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else {
        // TODO check string has right color string
        m_valueKind = CSSStyleValuePair::ValueKind::Number;
        sscanf(value, "%f%%", &m_value.m_floatValue);
    }
}

void CSSStyleValuePair::setValueVerticalAlign(std::vector<String*, gc_allocator<String*>>* tokens)
{
    const char* value = tokens->at(0)->utf8Data();
    m_keyKind = CSSStyleValuePair::KeyKind::VerticalAlign;
    if (VALUE_IS_INITIAL()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Initial;
    } else if (VALUE_IS_INHERIT()) {
        m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    } else if (VALUE_IS_STRING("baseline")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::BaselineVAlignValue;
    } else if (VALUE_IS_STRING("sub")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::SubVAlignValue;
    } else if (VALUE_IS_STRING("super")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::SuperVAlignValue;
    } else if (VALUE_IS_STRING("top")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::TopVAlignValue;
    } else if (VALUE_IS_STRING("text-top")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::TextTopVAlignValue;
    } else if (VALUE_IS_STRING("middle")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::MiddleVAlignValue;
    } else if (VALUE_IS_STRING("bottom")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::BottomVAlignValue;
    } else if (VALUE_IS_STRING("text-bottom")) {
        m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
        m_value.m_verticalAlign = VerticalAlignValue::TextBottomVAlignValue;
    } else {
        setValuePercentageOrLength(value);
    }
}

CSSStyleValuePair CSSStyleValuePair::fromString(const char* key, const char* value)
{
    CSSStyleValuePair ret;
    std::vector<String*, gc_allocator<String*>> tokens;
    DOMTokenList::tokenize(&tokens, String::fromUTF8(value));
    if (strcmp(key, "display") == 0) {
        ret.setValueDisplay(&tokens);
    } else if (strcmp(key, "position") == 0) {
        ret.setValuePosition(&tokens);
    } else if (strcmp(key, "width") == 0) {
        ret.setValueWidth(&tokens);
    } else if (strcmp(key, "height") == 0) {
        ret.setValueHeight(&tokens);
    } else if (strcmp(key, "font-size") == 0) {
        ret.setValueFontSize(&tokens);
    } else if (strcmp(key, "color") == 0) {
        // color | inherit // initial value -> depends on user agent
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Color;
        ret.setValueColor(&tokens);
    } else if (strcmp(key, "background-color") == 0) {
        // color | <transparent> | inherit
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundColor;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            // TODO: Consider CSS Colors (Hexadecimal/etc. colors)
            //       Check the value has right color strings
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "background-image") == 0) {
        // uri | <none> | inherit
    	ret.setKeyKind(CSSStyleValuePair::KeyKind::BackgroundImage);
        ret.setValueBackgroundImage(&tokens);
    } else if (strcmp(key, "vertical-align") == 0) {
        // <baseline> | sub | super | top | text-top | middle | bottom | text-bottom | percentage | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::VerticalAlign;
        if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_STRING("baseline")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::BaselineVAlignValue;
        } else if (VALUE_IS_STRING("sub")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::SubVAlignValue;
        } else if (VALUE_IS_STRING("super")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::SuperVAlignValue;
        } else if (VALUE_IS_STRING("top")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::TopVAlignValue;
        } else if (VALUE_IS_STRING("text-top")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::TextTopVAlignValue;
        } else if (VALUE_IS_STRING("middle")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::MiddleVAlignValue;
        } else if (VALUE_IS_STRING("bottom")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::BottomVAlignValue;
        } else if (VALUE_IS_STRING("text-bottom")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::VerticalAlignValueKind;
            ret.m_value.m_verticalAlign = VerticalAlignValue::TextBottomVAlignValue;
        } else {
            parsePercentageOrLength(ret, value)
;        }
    } else if (strcmp(key, "text-align") == 0) {
        // left | right | center | justify | <inherit>
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::TextAlign;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        }  else if (VALUE_IS_STRING("left")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::LeftTextAlignValue;
        } else if (VALUE_IS_STRING("center")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::CenterTextAlignValue;
        } else if (VALUE_IS_STRING("right")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::RightTextAlignValue;
        } else if (VALUE_IS_STRING("justify")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::JustifyTextAlignValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "text-decoration") == 0) {
        ret.setValueTextDecoration(&tokens);
    } else if (strcmp(key, "letter-spacing") == 0) {
        // normal | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::LetterSpacing;
        ret.setValueLetterSpacing(&tokens);
    } else if (strcmp(key, "text-overflow") == 0) {
        // <clip> | ellipsis | string
        ret.m_keyKind = CSSStyleValuePair::KeyKind::TextOverflow;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("clip")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextOverflowValueKind;
            ret.m_value.m_textOverflow = TextOverflowValue::ClipTextOverflowValue;
        } else if (VALUE_IS_STRING("ellipsis")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextOverflowValueKind;
            ret.m_value.m_textOverflow = TextOverflowValue::EllipsisTextOverflowValue;
        } else {
            // string: not supported
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "white-space") == 0) {
        //TODO
    } else if (strcmp(key, "font-style") == 0) {
        ret.setValueFontStyle(&tokens);
    } else if (strcmp(key, "direction") == 0) {
        ret.setValueDirection(&tokens);
    } else if (strcmp(key, "background-size") == 0) {
        // [length | percentage | auto]{1,2} | cover | contain // initial value -> auto
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundSize;
        if (VALUE_IS_STRING("contain")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Contain;
        } else if (VALUE_IS_STRING("cover")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Cover;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
            CSSPropertyParser* parser = new CSSPropertyParser((char*) value);
            //NOTE: CSS 2.1 does not support layering multiple background images(for comma-separated)
            ValueList* values = new ValueList(ValueList::Separator::SpaceSeparator);
            CSSStyleValuePair::ValueKind kind;
            while (parser->findNextValueKind(' ', &kind)) {
                if (kind == CSSStyleValuePair::ValueKind::Auto) {
                    values->append(kind, {0});
                } else if (kind == CSSStyleValuePair::ValueKind::Percentage) {
                    values->append(kind, {.m_floatValue = parser->parsedFloatValue()});
                } else if (kind == CSSStyleValuePair::ValueKind::Length) {
                    CSSStyleValuePair::ValueData data = {.m_length = CSSLength(parser->parsedFloatValue())};
                    values->append(kind, data);
                }
            }
            ret.m_value.m_multiValue = values;
        }
    } else if (strcmp(key, "background-repeat-x") == 0) {
        // repeat | no-repeat | initial | inherit // initial value -> repeat
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundRepeatX;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("no-repeat")) {
            ret.m_value.m_backgroundRepeatX = BackgroundRepeatValue::NoRepeatRepeatValue;
        } else if (VALUE_IS_STRING("repeat") || VALUE_IS_INITIAL()) {
            ret.m_value.m_backgroundRepeatX = BackgroundRepeatValue::RepeatRepeatValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "background-repeat-y") == 0) {
        // repeat | no-repeat | initial | inherit // initial value -> repeat
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundRepeatY;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("no-repeat")) {
            ret.m_value.m_backgroundRepeatY = BackgroundRepeatValue::NoRepeatRepeatValue;
        } else if (VALUE_IS_STRING("repeat") || VALUE_IS_INITIAL()) {
            ret.m_value.m_backgroundRepeatY = BackgroundRepeatValue::RepeatRepeatValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "top") == 0) {
        ret.setValueTop(&tokens);
    } else if (strcmp(key, "right") == 0) {
        ret.setValueRight(&tokens);
    } else if (strcmp(key, "bottom") == 0) {
        ret.setValueBottom(&tokens);
    } else if (strcmp(key, "left") == 0) {
        ret.setValueLeft(&tokens);
    } else if (strcmp(key, "border-top-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopColor;
        ret.setValueBorderTopColor(&tokens);
    } else if (strcmp(key, "border-right-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightColor;
        ret.setValueBorderRightColor(&tokens);
    } else if (strcmp(key, "border-bottom-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomColor;
        ret.setValueBorderBottomColor(&tokens);
    } else if (strcmp(key, "border-left-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftColor;
        ret.setValueBorderLeftColor(&tokens);
    } else if (strcmp(key, "border-image-repeat") == 0) {
        ret.setValueBorderImageRepeat(&tokens);
    } else if (strcmp(key, "border-image-width") == 0) {
        ret.setValueBorderImageWidth(&tokens);
    } else if (strcmp(key, "border-image-slice") == 0) {
        ret.setValueBorderImageSlice(&tokens);
    } else if (strcmp(key, "border-image-source") == 0) {
        ret.setValueBorderImageSource(&tokens);
    } else if (strcmp(key, "border-top-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopStyle;
        ret.setValueBorderTopStyle(&tokens);
    } else if (strcmp(key, "border-right-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightStyle;
        ret.setValueBorderRightStyle(&tokens);
    } else if (strcmp(key, "border-bottom-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomStyle;
        ret.setValueBorderBottomStyle(&tokens);
    } else if (strcmp(key, "border-left-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftStyle;
        ret.setValueBorderLeftStyle(&tokens);
    } else if (strcmp(key, "border-top-width") == 0) {
        // border-width(thin | <medium> | thick) | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopWidth;
        ret.setValueBorderTopWidth(&tokens);
    } else if (strcmp(key, "border-right-width") == 0) {
        // border-width(thin | <medium> | thick) | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightWidth;
        ret.setValueBorderRightWidth(&tokens);
    } else if (strcmp(key, "border-bottom-width") == 0) {
        // border-width(thin | <medium> | thick) | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomWidth;
        ret.setValueBorderBottomWidth(&tokens);
    } else if (strcmp(key, "border-left-width") == 0) {
        // border-width(thin | <medium> | thick) | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftWidth;
        ret.setValueBorderLeftWidth(&tokens);
    } else if (strcmp(key, "line-height") == 0) {
        // <normal> | number | length | percentage | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::LineHeight;
        ret.setValueLineHeight(&tokens);
    } else if (strcmp(key, "margin-top") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginTop;
        ret.setValueMarginTop(&tokens);
    } else if (strcmp(key, "margin-bottom") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginBottom;
        ret.setValueMarginBottom(&tokens);
    } else if (strcmp(key, "margin-left") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginLeft;
        ret.setValueMarginLeft(&tokens);
    } else if (strcmp(key, "margin-right") == 0) {
        // length | percentage | auto | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginRight;
        ret.setValueMarginRight(&tokens);
    } else if (strcmp(key, "padding-top") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingTop;
        ret.setValuePaddingTop(&tokens);
    } else if (strcmp(key, "padding-right") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingRight;
        ret.setValuePaddingRight(&tokens);
    } else if (strcmp(key, "padding-bottom") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingBottom;
        ret.setValuePaddingBottom(&tokens);
    } else if (strcmp(key, "padding-left") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingLeft;
        ret.setValuePaddingLeft(&tokens);
    } else if (strcmp(key, "opacity") == 0) {
        // alphavalue | inherit <1>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Opacity;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Number;

        if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            sscanf(value, "%f%%", &ret.m_value.m_floatValue);
        }
    } else if (strcmp(key, "overflow-x") == 0) {
        // visible | hidden | scroll(X) | auto(X) | inherit // initial value -> visible
        ret.m_keyKind = CSSStyleValuePair::KeyKind::OverflowX;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::OverflowValueKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("auto") || VALUE_IS_STRING("scroll")) {
            // Not Supported!!
        } else if (VALUE_IS_STRING("visible")) {
            ret.m_value.m_overflowX = OverflowValue::VisibleOverflow;
        } else if (VALUE_IS_STRING("hidden")){
            ret.m_value.m_overflowX = OverflowValue::HiddenOverflow;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "overflow-y") == 0) {
/*        // visible | hidden | scroll(X) | auto(X) | inherit // initial value -> visible
        ret.m_keyKind = CSSStyleValuePair::KeyKind::OverflowY;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::OverflowValueKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("auto") || VALUE_IS_STRING("scroll")) {
            // Not Supported!!
        } else if (VALUE_IS_STRING("visible")) {
            ret.m_value.m_overflowY = OverflowValue::VisibleOverflow;
        } else if (VALUE_IS_STRING("hidden")){
            ret.m_value.m_overflowY = OverflowValue::HiddenOverflow;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
*/
    } else if (strcmp(key, "visibility") == 0) {
        // visible | hidden | collapse | inherit // initial value -> visible
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Visibility;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::VisibilityKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("visible")) {
            ret.m_value.m_visibility = VisibilityValue::VisibleVisibilityValue;
        } else if (VALUE_IS_STRING("hidden")){
            ret.m_value.m_visibility = VisibilityValue::HiddenVisibilityValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "z-index") == 0) {
        ret.m_keyKind = CSSStyleValuePair::KeyKind::ZIndex;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL() || VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else {
            char* pEnd;
            double d = strtod (value, &pEnd);
            STARFISH_ASSERT(pEnd == value + strlen(value));
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Number;
            ret.m_value.m_floatValue = d;
        }
    } else {
        STARFISH_LOG_ERROR("CSSStyleValuePair::fromString -> unsupport key = %s\n", key);
        //STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    return ret;
}

String* CSSStyleValuePair::toString()
{
    switch (keyKind()) {
        case Color:
        case BackgroundColor:
        {
            if (m_valueKind == CSSStyleValuePair::ValueKind::Auto)
                return String::fromUTF8("auto");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::Initial)
                return String::fromUTF8("initial");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::Inherit)
                return String::fromUTF8("inherit");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::StringValueKind)
                return stringValue();
            break;
        }
        case BackgroundImage:
        	if (m_valueKind == CSSStyleValuePair::ValueKind::None) {
        		return String::fromUTF8("none");
        	} else if (m_valueKind == CSSStyleValuePair::ValueKind::Inherit) {
        		return String::fromUTF8("initial");
        	} else if (m_valueKind == CSSStyleValuePair::ValueKind::Initial) {
        		return String::fromUTF8("inherit");
    		} else if (m_valueKind == CSSStyleValuePair::ValueKind::StringValueKind) {
    			return stringValue();
    		} else {
    			STARFISH_RELEASE_ASSERT_NOT_REACHED();
    		}
        	break;
        case Direction: {
            switch(directionValue()) {
                case LtrDirectionValue:
                    return String::fromUTF8("ltr");
                case RtlDirectionValue:
                    return String::fromUTF8("rtl");
                default:
                    return String::emptyString;
            }
        }
        case LetterSpacing:
            if (m_valueKind == CSSStyleValuePair::ValueKind::Normal) {
                return String::fromUTF8("normal");
            } else {
                return valueToString();
            }
            break;
        case LineHeight:
            if (m_valueKind == CSSStyleValuePair::ValueKind::Normal) {
                return String::fromUTF8("normal");
            } else {
                return valueToString();
            }
            break;
        case MarginTop:
        case MarginRight:
        case MarginBottom:
        case MarginLeft:
            return lengthOrPercentageToString();
        case Top:
        case Bottom:
        case Left:
        case Right:
            return lengthOrPercentageToString();
        case Height:
        case Width:
            return lengthOrPercentageToString();

        case Position: {
            switch(positionValue()) {
                case PositionValue::StaticPositionValue:
                    return String::fromUTF8("static");
                case PositionValue::RelativePositionValue:
                    return String::fromUTF8("relative");
                case PositionValue::AbsolutePositionValue:
                    return String::fromUTF8("absolute");
                case PositionValue::FixedPositionValue:
                    return String::fromUTF8("fixed");
                default:
                    return String::emptyString;
            }
        }
        case TextDecoration: {
            switch(textDecoration()) {
                case TextDecorationValue::NoneTextDecorationValue:
                    return String::fromUTF8("none");
                case TextDecorationValue::UnderLineTextDecorationValue:
                    return String::fromUTF8("underline");
                case TextDecorationValue::OverLineTextDecorationValue:
                    return String::fromUTF8("overline");
                case TextDecorationValue::LineThroughTextDecorationValue:
                    return String::fromUTF8("line-through");
                case TextDecorationValue::BlinkTextDecorationValue:
                    return String::fromUTF8("blink");
                default:
                    return String::emptyString;
            }
        }
        case FontSize: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::XXSmallFontSizeValueKind:
                    return String::fromUTF8("xx-small");
                case CSSStyleValuePair::ValueKind::XSmallFontSizeValueKind:
                    return String::fromUTF8("x-small");
                case CSSStyleValuePair::ValueKind::SmallFontSizeValueKind:
                    return String::fromUTF8("small");
                case CSSStyleValuePair::ValueKind::MediumFontSizeValueKind:
                    return String::fromUTF8("medium");
                case CSSStyleValuePair::ValueKind::LargeFontSizeValueKind:
                    return String::fromUTF8("large");
                case CSSStyleValuePair::ValueKind::XLargeFontSizeValueKind:
                    return String::fromUTF8("x-large");
                case CSSStyleValuePair::ValueKind::XXLargeFontSizeValueKind:
                    return String::fromUTF8("xx-large");
                case CSSStyleValuePair::ValueKind::LargerFontSizeValueKind:
                    return String::fromUTF8("larger");
                case CSSStyleValuePair::ValueKind::SmallerFontSizeValueKind:
                    return String::fromUTF8("smaller");
                default:
                    return lengthOrPercentageToString();
            }
        }
        case FontStyle: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::FontStyleValueKind:
                    switch(fontStyleValue()) {
                    case FontStyleValue::NormalFontStyleValue:
                        return String::fromUTF8("normal");
                    case FontStyleValue::ItalicFontStyleValue:
                        return String::fromUTF8("italic");
                    case FontStyleValue::ObliqueFontStyleValue:
                        return String::fromUTF8("oblique");
                    }
                default:
                    return lengthOrPercentageToString();
            }
        }
        case Display: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::DisplayValueKind:
                    switch(displayValue()) {
                    case DisplayValue::InlineDisplayValue:
                        return String::fromUTF8("inline");
                    case DisplayValue::BlockDisplayValue:
                        return String::fromUTF8("block");
                    case DisplayValue::InlineBlockDisplayValue:
                        return String::fromUTF8("inline-block");
                    case DisplayValue::NoneDisplayValue:
                        return String::fromUTF8("none");
                    }
                default:
                    return lengthOrPercentageToString();
            }
        }
        case BorderImageRepeat: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::ValueListKind:
                {
                    ValueList* values = multiValue();
                    String* s = String::emptyString;
                    for (unsigned int i = 0; i < values->size(); i++) {
                        STARFISH_ASSERT(values->getValueKindAtIndex(i) == BorderImageRepeatValueKind);
                        switch(values->getValueAtIndex(i).m_borderImageRepeat) {
                            case BorderImageRepeatValue::StretchValue :
                                s = s->concat(String::fromUTF8("stretch"));
                                break;
                            case BorderImageRepeatValue::RepeatValue :
                                s = s->concat(String::fromUTF8("repeat"));
                                break;
                            case BorderImageRepeatValue::RoundValue :
                                s = s->concat(String::fromUTF8("round"));
                                break;
                            case BorderImageRepeatValue::SpaceValue :
                                s = s->concat(String::fromUTF8("space"));
                                break;
                            default:
                                STARFISH_RELEASE_ASSERT_NOT_REACHED();
                        }
                        if (i != values->size() - 1)
                            s = s->concat(String::fromUTF8(" "));
                    }
                    return s;
                }
                default:
                    //initial or inherit
                    return lengthOrPercentageToString();
            }
        }
        case BorderImageSlice: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::ValueListKind:
                {
                    ValueList* values = multiValue();
                    String* s = String::emptyString;
                    for (unsigned int i = 0; i < values->size(); i++) {
                        String* newstr;
                        if (values->getValueKindAtIndex(i) == CSSStyleValuePair::ValueKind::StringValueKind)
                            newstr = String::fromUTF8("fill");
                        else
                            newstr = valueToString(values->getValueKindAtIndex(i),
                                                   values->getValueAtIndex(i));
                        s = s->concat(newstr);
                        if (i != values->size() - 1)
                            s = s->concat(String::fromUTF8(" "));
                    }
                    return s;
                }
                default:
                    //initial or inherit
                    return lengthOrPercentageToString();
            }
        }
        case BorderImageSource: {
            //TODO(june0cho)
        }
        case BorderImageWidth: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::ValueListKind:
                {
                    ValueList* values = multiValue();
                    String* s = String::emptyString;
                    for (unsigned int i = 0; i < values->size(); i++) {
                        String* newstr = valueToString(values->getValueKindAtIndex(i),
                                                       values->getValueAtIndex(i));
                        s = s->concat(newstr);
                        if (i != values->size() - 1)
                            s = s->concat(String::fromUTF8(" "));
                    }
                    return s;
                }
                default:
                    return lengthOrPercentageToString();
            }
        }
        case BorderTopColor:
        case BorderRightColor:
        case BorderBottomColor:
        case BorderLeftColor: {
            switch(valueKind()) {
            case CSSStyleValuePair::ValueKind::Initial:
                return String::fromUTF8("initial");
            case CSSStyleValuePair::ValueKind::Transparent:
                return String::fromUTF8("transparent");
            case CSSStyleValuePair::ValueKind::StringValueKind:
                return stringValue();
            case CSSStyleValuePair::ValueKind::Inherit:
                return String::fromUTF8("inherit");
            default:
                break;
            }
        }
        case BorderTopStyle:
        case BorderRightStyle:
        case BorderBottomStyle:
        case BorderLeftStyle: {
            switch(valueKind()) {
            case CSSStyleValuePair::ValueKind::Initial:
                return String::fromUTF8("initial");
            case CSSStyleValuePair::ValueKind::BorderNone:
                return String::fromUTF8("none");
            case CSSStyleValuePair::ValueKind::BorderSolid:
                return String::fromUTF8("solid");
            case CSSStyleValuePair::ValueKind::Inherit:
                return String::fromUTF8("inherit");
            default:
                break;
            }
        }
        case BorderTopWidth:
        case BorderBottomWidth:
        case BorderRightWidth:
        case BorderLeftWidth: {
            switch(valueKind()) {
            case CSSStyleValuePair::ValueKind::Initial:
                return String::fromUTF8("initial");
            case CSSStyleValuePair::ValueKind::BorderThin:
                return String::fromUTF8("thin");
            case CSSStyleValuePair::ValueKind::BorderMedium:
                return String::fromUTF8("medium");
            case CSSStyleValuePair::ValueKind::BorderThick:
                return String::fromUTF8("thick");
            case CSSStyleValuePair::ValueKind::Inherit:
                return String::fromUTF8("inherit");
            default:
                return lengthOrPercentageToString();
            }
        }
        case TextAlign: {
            switch(textAlignValue()) {
                case TextAlignValue::NamelessTextAlignValue:
                    return String::fromUTF8("left");
                case TextAlignValue::LeftTextAlignValue:
                    return String::fromUTF8("left");
                case TextAlignValue::RightTextAlignValue:
                    return String::fromUTF8("right");
                case TextAlignValue::CenterTextAlignValue:
                    return String::fromUTF8("center");
                case TextAlignValue::JustifyTextAlignValue:
                    return String::fromUTF8("justify");
                default:
                    return String::emptyString;
            }
        }
        case Visibility: {
            switch(visibility()) {
                case VisibilityValue::VisibleVisibilityValue:
                    return String::fromUTF8("visible");
                case VisibilityValue::HiddenVisibilityValue:
                    return String::fromUTF8("hidden");
                default:
                    return String::emptyString;
            }
        }
        case Opacity: {
            return String::fromUTF8(std::to_string(numberValue()).c_str());
        }
        case OverflowX: {
            switch(overflowValueX()) {
                case OverflowValue::VisibleOverflow:
                    return String::fromUTF8("visible");
                case OverflowValue::HiddenOverflow:
                    return String::fromUTF8("hidden");
                default:
                    return String::emptyString;
            }
        }
        case ZIndex:
        {
            if (m_valueKind == CSSStyleValuePair::ValueKind::Auto)
                return String::fromUTF8("auto");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::Initial)
                return String::fromUTF8("initial");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::Inherit)
                return String::fromUTF8("inherit");
            else if (m_valueKind == CSSStyleValuePair::ValueKind::Number)
                return String::fromUTF8(std::to_string((int) numberValue()).c_str());
            break;
        }
        case VerticalAlign: {
            switch(valueKind()) {
                case CSSStyleValuePair::ValueKind::VerticalAlignValueKind:
                    switch(verticalAlignValue()) {
                        case VerticalAlignValue::BaselineVAlignValue:
                            return String::fromUTF8("baseline");
                        case VerticalAlignValue::SubVAlignValue:
                            return String::fromUTF8("sub");
                        case VerticalAlignValue::SuperVAlignValue:
                            return String::fromUTF8("super");
                        case VerticalAlignValue::TopVAlignValue:
                            return String::fromUTF8("top");
                        case VerticalAlignValue::TextTopVAlignValue:
                            return String::fromUTF8("text-top");
                        case VerticalAlignValue::MiddleVAlignValue:
                            return String::fromUTF8("middle");
                        case VerticalAlignValue::BottomVAlignValue:
                            return String::fromUTF8("bottom");
                        case VerticalAlignValue::TextBottomVAlignValue:
                            return String::fromUTF8("text-bottom");
                        case VerticalAlignValue::NumericVAlignValue:
                            // FIXME:mh.byun
                            return String::fromUTF8("number??");
                        default:
                            return String::emptyString;
                    }
                default:
                    return lengthOrPercentageToString();
            }
        }
        default: {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
            return nullptr;
        }
    }
    STARFISH_RELEASE_ASSERT_NOT_REACHED();
}

void CSSStyleValuePair::setLengthValue(const char* value)
{
    if (VALUE_IS_STRING("auto")) {
        setValueKind(CSSStyleValuePair::ValueKind::Auto);
    } else if (VALUE_IS_INHERIT()) {
        setValueKind(CSSStyleValuePair::ValueKind::Inherit);
    } else if (VALUE_IS_INITIAL()) {
        setValueKind(CSSStyleValuePair::ValueKind::Initial);
    } else {
        parsePercentageOrLength(*this, value);
    }
}

String* CSSStyleDeclaration::Margin()
{
    String* sum;
    String* space = String::fromUTF8(" ");
    String* top = MarginTop();
    if (!top->equals(String::emptyString)) {
        String* right = MarginRight();
        if (!right->equals(String::emptyString)) {
            String* bottom = MarginBottom();
            if (!bottom->equals(String::emptyString)) {
                String* left = MarginLeft();
                if (!left->equals(String::emptyString)) {
                       sum = top;
                       return sum->concat(space)->concat(right)->concat(space)->concat(bottom)->concat(space)->concat(left);
                  }
            }
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setMargin(const char* value)
{
    std::vector<String*, gc_allocator<String*>> tokens;
    DOMTokenList::tokenize(&tokens, String::fromUTF8(value));
    if (checkInputErrorMargin(&tokens)) {
        unsigned len = tokens.size();
        if (len == 1) {
            tokens.push_back(tokens[0]);
            tokens.push_back(tokens[0]);
            tokens.push_back(tokens[0]);
        } else if (len == 2) {
            tokens.push_back(tokens[0]);
            tokens.push_back(tokens[1]);
        } else if (len == 3)
            tokens.push_back(tokens[1]);

        setMarginTop(tokens[0]->utf8Data());
        setMarginRight(tokens[1]->utf8Data());
        setMarginBottom(tokens[2]->utf8Data());
        setMarginLeft(tokens[3]->utf8Data());
    }
}

bool CSSStyleDeclaration::checkInputErrorColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // color | percentage | <auto> | inherit
    if (tokens->size() > 1)
        return false;
    const char* token = tokens->at(0)->utf8Data();
    if (!(CSSPropertyParser::assureColor(token) ||
        (strcmp(token, "initial") == 0) ||
        (strcmp(token, "inherit") == 0))) {
        return false;
    }
    return true;
}

bool CSSStyleDeclaration::checkInputErrorBackgroundColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorColor(tokens);
}

bool CSSStyleDeclaration::checkInputErrorLetterSpacing(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() != 1) return false;
    (*tokens)[0] = (*tokens)[0]->toLower();
    const char* value = (*tokens)[0]->utf8Data();
    return (VALUE_IS_STRING("normal")
            || CSSPropertyParser::assureEssential(value)
            || CSSPropertyParser::assureLength(value, false));
}

bool CSSStyleDeclaration::checkInputErrorLineHeight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() != 1) return false;
    (*tokens)[0] = (*tokens)[0]->toLower();
    const char* value = (*tokens)[0]->utf8Data();
    return (VALUE_IS_STRING("normal")
            || CSSPropertyParser::assureEssential(value)
            || CSSPropertyParser::assureLength(value, false)
            || CSSPropertyParser::assurePercent(value, false)
            || CSSPropertyParser::assureNumber(value, false));
}

bool CSSStyleDeclaration::checkInputErrorMarginTop(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorBackgroundImage(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = tokens->at(0)->utf8Data();
        size_t len = strlen(token);

        if(startsWith(token, "url(") && endsWith(token, ")")) {
            if((token[4] == '\"' || token[4] == '\'') && (token[4] != token[len-2])) {
                return false;
            }
            return true;
        } else if (CSSPropertyParser::assureEssential(token)) {
            return true;
        }
    } else if (tokens->size() == 2) {
        const char* token0 = tokens->at(0)->utf8Data();
        const char* token1 = tokens->at(1)->utf8Data();

        if(startsWith(token0, "url(") && endsWith(token1, ")")) {
            size_t len0 = strlen(token0);
            size_t len1 = strlen(token1);

            if(len0 > 4 && (token0[4] == '\"' || token0[4] == '\'') && (token0[4] != token0[len0-1])) {
                return false;
            } else if(len1 > 1 && (token1[0] == '\"' || token1[0] == '\'') && (token1[0] != token1[len1-2])) {
                return false;
            }
            return true;
        }
    } else if (tokens->size() == 3) {
        const char* token0 = tokens->at(0)->utf8Data();
        const char* token1 = tokens->at(1)->utf8Data();
        const char* token2 = tokens->at(2)->utf8Data();

        if((startsWith(token0, "url(") && strlen(token0) == 4) && (endsWith(token2, ")") && strlen(token2) == 1)) {
            size_t len1 = strlen(token1);

            if((token1[0] == '\"' || token1[0] == '\'') && (token1[0] != token1[len1-1])) {
                return false;
            }
            return true;
        }
    }

    return false;
}

bool CSSStyleDeclaration::checkInputErrorMarginRight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorMarginBottom(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorMarginLeft(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorMargin(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // length | percentage | <auto> | inherit
    for (unsigned i = 0; i < tokens->size(); i++) {
        const char* token = tokens->at(i)->utf8Data();
        if (!(CSSPropertyParser::assureLength(token, false) ||
            CSSPropertyParser::assurePercent(token, false) ||
            (strcmp(token, "auto") == 0) || (strcmp(token, "initial") == 0) || (strcmp(token, "inherit") == 0))) {
                return false;
        }
    }
    return true;
}

bool CSSStyleDeclaration::checkHavingOneTokenAndLengthOrPercentage(std::vector<String*, gc_allocator<String*>>* tokens, bool allowNegative)
{
    // length | percentage | <auto> | inherit
    if (tokens->size() == 1) {
        (*tokens)[0] = (*tokens)[0]->toLower();
        const char* token = (*tokens)[0]->utf8Data();
        if (CSSPropertyParser::assureLength(token, allowNegative) ||
            CSSPropertyParser::assurePercent(token, allowNegative) ||
            (strcmp(token, "auto") == 0) || (strcmp(token, "initial") == 0) || (strcmp(token, "inherit") == 0)) {
                return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorTop(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorBottom(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorLeft(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorRight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkHavingOneTokenAndLengthOrPercentage(tokens, true);
}

bool CSSStyleDeclaration::checkInputErrorDirection(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // <ltr> | rtl | inherit
    if (tokens->size() == 1) {
        (*tokens)[0] = (*tokens)[0]->toLower();
        const char* token = (*tokens)[0]->utf8Data();
        if ((strcmp(token, "ltr") == 0) ||
            (strcmp(token, "rtl") == 0) ||
            (strcmp(token, "inherit") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return true;
}

bool CSSStyleDeclaration::checkInputErrorHeight(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return true;
}

bool CSSStyleDeclaration::checkInputErrorFontSize(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "xx-small") == 0) ||
            (strcmp(token, "x-small") == 0) ||
            (strcmp(token, "small") == 0) ||
            (strcmp(token, "medium") == 0) ||
            (strcmp(token, "large") == 0) ||
            (strcmp(token, "x-large") == 0) ||
            (strcmp(token, "xx-large") == 0) ||
            (strcmp(token, "larger") == 0) ||
            (strcmp(token, "smaller") == 0) ||
            (strcmp(token, "inherit") == 0) ||
            (strcmp(token, "init") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorFontStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "normal") == 0) ||
            (strcmp(token, "italic") == 0) ||
            (strcmp(token, "oblique") == 0) ||
            (strcmp(token, "inherit") == 0) ||
            (strcmp(token, "init") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorDisplay(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "inline") == 0) ||
            (strcmp(token, "block") == 0) ||
            (strcmp(token, "inline-block") == 0) ||
            (strcmp(token, "none") == 0) ||
            (strcmp(token, "inherit") == 0) ||
            (strcmp(token, "init") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorPosition(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "static") == 0) ||
            (strcmp(token, "relative") == 0) ||
            (strcmp(token, "absolute") == 0) ||
            (strcmp(token, "fixed") == 0) ||
            (strcmp(token, "inherit") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorTextDecoration(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "none") == 0) ||
            (strcmp(token, "underline") == 0) ||
            (strcmp(token, "overline") == 0) ||
            (strcmp(token, "line-through") == 0) ||
            (strcmp(token, "blink") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorBorderImageRepeat(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1 || tokens->size() == 2) {
        for (unsigned int i = 0; i < tokens->size(); i++) {
            const char* token = (*tokens)[i]->toLower()->utf8Data();
            if ((strcmp(token, "stretch") == 0) ||
                (strcmp(token, "repeat") == 0) ||
                (strcmp(token, "round") == 0) ||
                (strcmp(token, "space") == 0)) {
                continue;
            } else {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorBorderImageSlice(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // number | percentage {1,4} && fill
    if (tokens->size() > 4) return false;
    for (unsigned int i = 0; i < tokens->size(); i++) {
        const char* currentToken = tokens->at(i)->toLower()->utf8Data();
        if (CSSPropertyParser::assurePercent(currentToken, false) ||
           CSSPropertyParser::assureNumber(currentToken, false))
            continue;
        else if (strcmp(currentToken, "fill") == 0) {
           if (i == tokens->size()-1) return true;
           else return false;
        } else {
            return false;
        }
    }
    return true;
}

bool CSSStyleDeclaration::checkInputErrorBorderImageSource(std::vector<String*, gc_allocator<String*>>* tokens)
{
    //TODO(june0cho)
    return true;
}

bool CSSStyleDeclaration::checkInputErrorBorderImageWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // [ <length> | <percentage> | <number> | auto ]{1,4}
    if (tokens->size() > 4) return false;
    for (unsigned int i = 0; i < tokens->size(); i++) {
        const char* currentToken = tokens->at(i)->toLower()->utf8Data();
        if (CSSPropertyParser::assureLength(currentToken, false) ||
           CSSPropertyParser::assurePercent(currentToken, false) ||
           CSSPropertyParser::assureNumber(currentToken, false) ||
           strcmp(currentToken, "auto") == 0) {
           continue;
        } else {
            return false;
        }
    }
    return true;
}

bool checkInputErrorBorderUnitColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // color | transparent | inherit
    if (tokens->size() == 1) {
        const char* token = tokens->at(0)->utf8Data();
        if (CSSPropertyParser::assureColor(token) ||
            (strcmp(token, "transparent") == 0) ||
            (strcmp(token, "inherit") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorBorderTopColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitColor(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderRightColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitColor(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderBottomColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitColor(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderLeftColor(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitColor(tokens);
}

bool checkInputErrorBorderUnitStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // border-style(<none> | solid) | inherit
    if (tokens->size() == 1) {
        const char* token = tokens->at(0)->utf8Data();
        if ((strcmp(token, "none") == 0) ||
            (strcmp(token, "solid") == 0) ||
            (strcmp(token, "inherit") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorBorderTopStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitStyle(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderRightStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitStyle(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderBottomStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitStyle(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderLeftStyle(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitStyle(tokens);
}

bool checkInputErrorBorderUnitWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    // border-width(thin | <medium> | thick) | length | inherit
    if (tokens->size() == 1) {
        const char* token = tokens->at(0)->utf8Data();
        if ((strcmp(token, "thin") == 0) ||
            (strcmp(token, "medium") == 0) ||
            (strcmp(token, "thick") == 0) ||
            (strcmp(token, "inherit") == 0) ||
            CSSPropertyParser::assureLength(token, false)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorBorderTopWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitWidth(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderRightWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitWidth(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderBottomWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitWidth(tokens);
}

bool CSSStyleDeclaration::checkInputErrorBorderLeftWidth(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return checkInputErrorBorderUnitWidth(tokens);
}

bool CSSStyleDeclaration::checkInputErrorTextAlign(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "left") == 0) ||
            (strcmp(token, "right") == 0) ||
            (strcmp(token, "center") == 0) ||
            (strcmp(token, "justify") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorVisibility(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "visible") == 0) ||
            (strcmp(token, "hidden") == 0)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorOpacity(std::vector<String*, gc_allocator<String*>>* tokens)
{
    return true;
}

bool CSSStyleDeclaration::checkInputErrorOverflowX(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "visible") == 0) ||
            (strcmp(token, "hidden") == 0)) {
            return true;
        }
    }
    return false;
}
bool CSSStyleDeclaration::checkInputErrorZIndex(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = tokens->at(0)->utf8Data();
        if ((strcmp(token, "auto") == 0) ||
            (strcmp(token, "initial") == 0) ||
            (strcmp(token, "inherit") == 0) ||
            CSSPropertyParser::assureInteger(token, false)) {
            return true;
        }
    }
    return false;
}

bool CSSStyleDeclaration::checkInputErrorVerticalAlign(std::vector<String*, gc_allocator<String*>>* tokens)
{
    if (tokens->size() == 1) {
        const char* token = (*tokens)[0]->toLower()->utf8Data();
        if ((strcmp(token, "baseline") == 0) ||
            (strcmp(token, "sub") == 0) ||
            (strcmp(token, "super") == 0) ||
            (strcmp(token, "top") == 0) ||
            (strcmp(token, "text-top") == 0) ||
            (strcmp(token, "middle") == 0) ||
            (strcmp(token, "bottom") == 0) ||
            (strcmp(token, "text-bottom") == 0)) {
            return true;
        }else if(CSSPropertyParser::assureLength(token, true)||CSSPropertyParser::assurePercent(token, true)){
            return true;
        }
    }
    return false;
}

ComputedStyle* StyleResolver::resolveDocumentStyle(StarFish* sf)
{
    ComputedStyle* ret = new ComputedStyle();
    ret->m_display = DisplayValue::BlockDisplayValue;
    ret->m_inheritedStyles.m_color = Color(0,0,0,255);
    // TODO implement ltr, rtl
    ret->m_inheritedStyles.m_textAlign = TextAlignValue::LeftTextAlignValue;
    ret->m_inheritedStyles.m_direction = DirectionValue::LtrDirectionValue;
    ret->loadResources(sf);
    return ret;
}

Color parseColor(String* str)
{
    const char* s = str->utf8Data();
    if (startsWith(s, "rgba")) {
        float r, g, b, a;
        sscanf(s, "rgba(%f,%f,%f,%f)", &r, &g, &b, &a);
        return Color(r, g, b, a*255);
    } else if (startsWith(s, "rgb")) {
        float r, g, b;
        sscanf(s, "rgb(%f,%f,%f)", &r, &g, &b);
        return Color(r, g, b, 255);
    } else {
        if (strcmp("black", s) == 0) {
            return Color(0, 0, 0, 255);
        } else if (strcmp("red", s) == 0) {
            return Color(255, 0, 0, 255);
        } else if (strcmp("green", s) == 0) {
            return Color(0, 128, 0, 255);
        } else if (strcmp("blue", s) == 0) {
            return Color(0, 0, 255, 255);
        } else if (strcmp("aqua", s) == 0) {
            return Color(0, 255, 255, 255);
        } else if (strcmp("fuchsia", s) == 0) {
            return Color(255, 0, 255, 255);
        } else if (strcmp("gray", s) == 0) {
            return Color(128, 128, 128, 255);
        } else if (strcmp("lime", s) == 0) {
            return Color(0, 255, 0, 255);
        } else if (strcmp("maroon", s) == 0) {
            return Color(128, 0, 0, 255);
        } else if (strcmp("navy", s) == 0) {
            return Color(0, 0, 128, 255);
        } else if (strcmp("olive", s) == 0) {
            return Color(128, 128, 0, 255);
        } else if (strcmp("orange", s) == 0) {
            return Color(255, 165, 0, 255);
        } else if (strcmp("yellow", s) == 0) {
            return Color(255, 255, 0, 255);
        } else if (strcmp("purple", s) == 0) {
            return Color(128, 0, 128, 255);
        } else if (strcmp("silver", s) == 0) {
            return Color(192, 192, 192, 255);
        } else if (strcmp("teal", s) == 0) {
            return Color(0, 128, 128, 255);
        } else if (strcmp("white", s) == 0) {
            return Color(255, 255, 255, 255);
        } else if (strcmp("transparent", s) == 0) {
            return Color(0, 0, 0, 0);
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }
}

ComputedStyle* StyleResolver::resolveStyle(Element* element, ComputedStyle* parent)
{
    ComputedStyle* ret = new ComputedStyle(parent);

    auto apply = [](std::vector<CSSStyleValuePair, gc_allocator<CSSStyleValuePair>>& cssValues, ComputedStyle* style, ComputedStyle* parentStyle) {
        for (unsigned k = 0; k < cssValues.size(); k ++) {
            switch (cssValues[k].keyKind()) {
            case CSSStyleValuePair::KeyKind::Display:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_display = parentStyle->m_display;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_display = DisplayValue::InlineDisplayValue;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::DisplayValueKind == cssValues[k].valueKind());
                    style->m_display = cssValues[k].displayValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::Position:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_position = parentStyle->m_position;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_position = PositionValue::StaticPositionValue;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::PositionValueKind == cssValues[k].valueKind());
                    style->m_position = cssValues[k].positionValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::Width:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_width = parentStyle->m_width;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_width = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_width = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->m_width = cssValues[k].lengthValue().toLength();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->m_width = Length(Length::Percent, cssValues[k].percentageValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Height:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_height = parentStyle->m_height;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_height = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_height = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->m_height = cssValues[k].lengthValue().toLength();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->m_height = Length(Length::Percent, cssValues[k].percentageValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Color:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_color = parentStyle->m_inheritedStyles.m_color;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_inheritedStyles.m_color = parseColor(String::fromUTF8("black"));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->m_inheritedStyles.m_color = parseColor(cssValues[k].stringValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::FontSize:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_fontSize = parentStyle->m_inheritedStyles.m_fontSize;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(3);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::XXSmallFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(0);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::XSmallFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(1);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::SmallFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(2);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::MediumFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(3);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::LargeFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(4);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::XLargeFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(5);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::XXLargeFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = parseAbsoluteFontSize(6);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::LargerFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = Length(Length::Fixed, parentStyle->m_inheritedStyles.m_fontSize.fixed() * 1.2f);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::SmallerFontSizeValueKind) {
                    style->m_inheritedStyles.m_fontSize = Length(Length::Fixed, parentStyle->m_inheritedStyles.m_fontSize.fixed() / 1.2f);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    float parentComputedFontSize = parentStyle->fontSize().fixed();
                    style->m_inheritedStyles.m_fontSize = Length(Length::Fixed, cssValues[k].percentageValue() * parentComputedFontSize);
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length);
                    style->m_inheritedStyles.m_fontSize = cssValues[k].lengthValue().toLength();
                    style->m_inheritedStyles.m_fontSize.changeToFixedIfNeeded(parentStyle->fontSize(), parentStyle->font());
                }
                break;
            case CSSStyleValuePair::KeyKind::FontStyle:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_fontStyle = parentStyle->m_inheritedStyles.m_fontStyle;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_inheritedStyles.m_fontStyle = FontStyleValue::NormalFontStyleValue;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::FontStyleValueKind);
                    style->m_inheritedStyles.m_fontStyle = cssValues[k].fontStyleValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::TextOverflow:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setTextOverflow(parentStyle->textOverflow());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setTextOverflow(TextOverflowValue::ClipTextOverflowValue);
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::TextOverflowValueKind);
                    style->setTextOverflow(cssValues[k].textOverflowValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::VerticalAlign:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setVerticalAlign(parentStyle->verticalAlign());
                    if (style->isNumericVerticalAlign()) style->setVerticalAlignLength(parentStyle->verticalAlignLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setVerticalAlign(ComputedStyle::initialVerticalAlign());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setVerticalAlignLength(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setVerticalAlignLength(Length(Length::Percent, cssValues[k].percentageValue()));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::VerticalAlignValueKind) {
                    STARFISH_ASSERT(cssValues[k].verticalAlignValue() != VerticalAlignValue::NumericVAlignValue);
                    style->setVerticalAlign(cssValues[k].verticalAlignValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::TextAlign:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    // NOTICE: Do not use getter of parent's textAlign here.
                    //         (to remove direction dependency)
                    style->setTextAlign(parentStyle->m_inheritedStyles.m_textAlign);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // Initial: a nameless value that acts as 'left' if 'direction' is 'ltr', 'right' if 'direction' is 'rtl'
                    style->setTextAlign(ComputedStyle::initialTextAlign());
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::TextAlignValueKind);
                    style->setTextAlign(cssValues[k].textAlignValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::TextDecoration:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setTextDecoration(parentStyle->textDecoration());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setTextDecoration(TextDecorationValue::NoneTextDecorationValue);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::None){
                    style->setTextDecoration(cssValues[k].textDecoration());
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::TextDecorationKind);
                    style->setTextDecoration(cssValues[k].textDecoration());
                }
                break;
            case CSSStyleValuePair::KeyKind::LetterSpacing:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setLetterSpacing(parentStyle->letterSpacing());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Normal) {
                    style->setLetterSpacing(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length);
                    style->setLetterSpacing(cssValues[k].lengthValue().toLength());
                }
                break;
            case CSSStyleValuePair::KeyKind::Direction:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_direction = parentStyle->m_inheritedStyles.m_direction;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_inheritedStyles.m_direction = DirectionValue::LtrDirectionValue;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::DirectionValueKind);
                    style->m_inheritedStyles.m_direction = cssValues[k].directionValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBgColor(parentStyle->bgColor());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->setBgColor(Color(0, 0, 0, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBgColor(parseColor(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundImage:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::None) {
                    style->setBgImage(ComputedStyle::initialBgImage());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBgImage(parentStyle->bgImage());
                } else {
                	STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBgImage(parseUrl(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundSize:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBackgroundSizeType(parentStyle->bgSizeType());
                    style->setBackgroundSizeValue(parentStyle->bgSizeValue());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // initial value : auto
                    LengthSize* result = new LengthSize();
                    style->setBackgroundSizeValue(result);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Cover) {
                    style->setBackgroundSizeType(BackgroundSizeType::Cover);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Contain) {
                    style->setBackgroundSizeType(BackgroundSizeType::Contain);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueListKind) {
                    ValueList* list = cssValues[k].multiValue();
                    LengthSize* result = new LengthSize();
                    if (list->size() >= 1) {
                        result->m_width = convertValueToLength(list->getValueKindAtIndex(0), list->getValueAtIndex(0));
                    }
                    if (list->size() >= 2) {
                        result->m_height = convertValueToLength(list->getValueKindAtIndex(1), list->getValueAtIndex(1));
                    }
                    style->setBackgroundSizeValue(result);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundRepeatX:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBackgroundRepeatX(parentStyle->backgroundRepeatX());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setBackgroundRepeatX(BackgroundRepeatValue::RepeatRepeatValue);
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind == cssValues[k].valueKind());
                    style->setBackgroundRepeatX(cssValues[k].backgroundRepeatXValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundRepeatY:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBackgroundRepeatY(parentStyle->backgroundRepeatY());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setBackgroundRepeatY(BackgroundRepeatValue::RepeatRepeatValue);
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind == cssValues[k].valueKind());
                    style->setBackgroundRepeatY(cssValues[k].backgroundRepeatYValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::Top:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setTop(parentStyle->top());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->setTop(Length());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setTop(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setTop(Length(Length::Percent, cssValues[k].percentageValue()));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Right:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setRight(parentStyle->right());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->setRight(Length());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setRight(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setRight(Length(Length::Percent, cssValues[k].percentageValue()));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Bottom:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBottom(parentStyle->bottom());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->setBottom(Length());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setBottom(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setBottom(Length(Length::Percent, cssValues[k].percentageValue()));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Left:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setLeft(parentStyle->left());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->setLeft(Length());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setLeft(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setLeft(Length(Length::Percent, cssValues[k].percentageValue()));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderTopColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderTopColor(parentStyle->borderTopColor());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // This is done after resolve style (arrangeStyleValues function in ComputedStyle Class)
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->setBorderTopColor(Color(0, 0, 0, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBorderTopColor(parseColor(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderRightColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderRightColor(parentStyle->borderRightColor());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // This is done after resolve style (arrangeStyleValues function in ComputedStyle Class)
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->setBorderRightColor(Color(0, 0, 0, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBorderRightColor(parseColor(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderBottomColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderBottomColor(parentStyle->borderBottomColor());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // This is done after resolve style (arrangeStyleValues function in ComputedStyle Class)
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->setBorderBottomColor(Color(0, 0, 0, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBorderBottomColor(parseColor(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderLeftColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderLeftColor(parentStyle->borderLeftColor());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // This is done after resolve style (arrangeStyleValues function in ComputedStyle Class)
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->setBorderLeftColor(Color(0, 0, 0, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->setBorderLeftColor(parseColor(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageRepeat:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setBorderImageRepeatX(StretchValue);
                    style->setBorderImageRepeatY(StretchValue);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    // TODO: Prevent parentStyle->surround() from creating object for this
                    style->setBorderImageRepeatFromOther(parentStyle);
                } else {
                    style->setBorderImageRepeatX(cssValues[k].multiValue()->getValueAtIndex(0).m_borderImageRepeat);
                    style->setBorderImageRepeatY(cssValues[k].multiValue()->getValueAtIndex(1).m_borderImageRepeat);
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageSlice:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setBorderImageSlices(ComputedStyle::initialBorderImageSlices());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    // TODO: Prevent parentStyle->surround() from creating object for this
                    style->setBorderImageSliceFromOther(parentStyle);
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::ValueListKind);
                    Length top, right, bottom, left;
                    ValueList* l = cssValues[k].multiValue();
                    unsigned int size = l->size();
                    if (l->getValueKindAtIndex(size - 1) == CSSStyleValuePair::ValueKind::StringValueKind) {
                        style->setBorderImageSliceFill(true);
                        size--;
                    }
                    top = convertPercentOrNumberToLength(l->getValueKindAtIndex(0), l->getValueAtIndex(0));
                    if (size > 1) {
                        right = convertPercentOrNumberToLength(l->getValueKindAtIndex(1), l->getValueAtIndex(1));
                    } else {
                        right = top;
                    }
                    if (size > 2) {
                        bottom = convertPercentOrNumberToLength(l->getValueKindAtIndex(2), l->getValueAtIndex(2));
                    } else {
                        bottom = top;
                    }
                    if (size > 3) {
                        left = convertPercentOrNumberToLength(l->getValueKindAtIndex(3), l->getValueAtIndex(3));
                    } else {
                        left = right;
                    }
                    style->setBorderImageSlices(LengthBox(top, right, bottom, left));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageSource:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderImageSource(parentStyle->borderImageSource());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setBorderImageSource(String::emptyString);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::None) {
                    style->setBorderImageSource(String::emptyString);
                } else {
                	STARFISH_ASSERT(CSSStyleValuePair::ValueKind::StringValueKind == cssValues[k].valueKind());
                    style->setBorderImageSource(parseUrl(cssValues[k].stringValue()));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageWidth:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderImageWidths(parentStyle->borderImageWidths());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    BorderImageLengthBox box;
                    style->setBorderImageWidths(box);
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::ValueListKind);
                    BorderImageLength top, right, bottom, left;
                    ValueList* l = cssValues[k].multiValue();
                    unsigned int size = l->size();
                    if (l->getValueKindAtIndex(0) == CSSStyleValuePair::ValueKind::Number)
                        top.setValue(l->getValueAtIndex(0).m_floatValue);
                    else
                        top.setValue(convertValueToLength(l->getValueKindAtIndex(0), l->getValueAtIndex(0)));
                    if (size > 1) {
                        if (l->getValueKindAtIndex(1) == CSSStyleValuePair::ValueKind::Number)
                            right.setValue(l->getValueAtIndex(1).m_floatValue);
                        else
                            right.setValue(convertValueToLength(l->getValueKindAtIndex(1), l->getValueAtIndex(1)));
                    } else {
                        right = top;
                    }
                    if (size > 2) {
                        if (l->getValueKindAtIndex(2) == CSSStyleValuePair::ValueKind::Number)
                            bottom.setValue(l->getValueAtIndex(2).m_floatValue);
                        else
                            bottom.setValue(convertValueToLength(l->getValueKindAtIndex(2), l->getValueAtIndex(2)));
                    } else {
                        bottom = top;
                    }
                    if (size > 3) {
                        if (l->getValueKindAtIndex(3) == CSSStyleValuePair::ValueKind::Number)
                            left.setValue(l->getValueAtIndex(3).m_floatValue);
                        else
                            left.setValue(convertValueToLength(l->getValueKindAtIndex(3), l->getValueAtIndex(3)));
                    } else {
                        left = right;
                    }
                    style->setBorderImageWidths(BorderImageLengthBox(top, right, bottom, left));
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderTopStyle:
                // border-style(<none> | solid) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderTopStyle(parentStyle->borderTopStyle());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderNone) {
                    style->setBorderTopStyle(BorderStyleValue::BNone);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderSolid) {
                    style->setBorderTopStyle(BorderStyleValue::BSolid);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderRightStyle:
                // border-style(<none> | solid) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderRightStyle(parentStyle->borderRightStyle());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderNone) {
                    style->setBorderRightStyle(BorderStyleValue::BNone);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderSolid) {
                    style->setBorderRightStyle(BorderStyleValue::BSolid);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderBottomStyle:
                // border-style(<none> | solid) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderBottomStyle(parentStyle->borderBottomStyle());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderNone) {
                    style->setBorderBottomStyle(BorderStyleValue::BNone);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderSolid) {
                    style->setBorderBottomStyle(BorderStyleValue::BSolid);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderLeftStyle:
                // border-style(<none> | solid) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderLeftStyle(parentStyle->borderLeftStyle());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderNone) {
                    style->setBorderLeftStyle(BorderStyleValue::BNone);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderSolid) {
                    style->setBorderLeftStyle(BorderStyleValue::BSolid);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderTopWidth:
                // border-width(thin | <medium> | thick | length) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderTopWidth(parentStyle->borderTopWidth());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setBorderTopWidth(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThin) {
                    style->setBorderTopWidth(Length(Length::Fixed, 1));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderMedium) {
                    style->setBorderTopWidth(Length(Length::Fixed, 3));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThick) {
                    style->setBorderTopWidth(Length(Length::Fixed, 5));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderRightWidth:
                // border-width(thin | <medium> | thick | length) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderRightWidth(parentStyle->borderRightWidth());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setBorderRightWidth(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThin) {
                    style->setBorderRightWidth(Length(Length::Fixed, 1));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderMedium) {
                    style->setBorderRightWidth(Length(Length::Fixed, 3));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThick) {
                    style->setBorderRightWidth(Length(Length::Fixed, 5));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderBottomWidth:
                // border-width(thin | <medium> | thick | length) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderBottomWidth(parentStyle->borderBottomWidth());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setBorderBottomWidth(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThin) {
                    style->setBorderBottomWidth(Length(Length::Fixed, 1));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderMedium) {
                    style->setBorderBottomWidth(Length(Length::Fixed, 3));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThick) {
                    style->setBorderBottomWidth(Length(Length::Fixed, 5));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderLeftWidth:
                // border-width(thin | <medium> | thick | length) | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBorderLeftWidth(parentStyle->borderLeftWidth());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->setBorderLeftWidth(cssValues[k].lengthValue().toLength());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThin) {
                    style->setBorderLeftWidth(Length(Length::Fixed, 1));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderMedium) {
                    style->setBorderLeftWidth(Length(Length::Fixed, 3));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderThick) {
                    style->setBorderLeftWidth(Length(Length::Fixed, 5));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::LineHeight:
                // <normal> | number | length | percentage | inherit
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setLineHeight(parentStyle->lineHeight());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Normal) {
                    // The compute value should be 'normal'.
                    // https://developer.mozilla.org/ko/docs/Web/CSS/line-height.
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Number) {
                    // The computed value should be same as the specified value.
                    style->setLineHeight(Length(Length::EmToBeFixed, cssValues[k].numberValue()));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->setLineHeight(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::MarginTop:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setMarginTop(parentStyle->marginTop());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setMarginTop(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setMarginTop(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::MarginBottom:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->setMarginBottom(Length(Length::Auto));
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setMarginBottom(parentStyle->marginBottom());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setMarginBottom(ComputedStyle::initialMargin());
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setMarginBottom(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::MarginLeft:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setMarginLeft(parentStyle->marginLeft());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setMarginLeft(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setMarginLeft(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::MarginRight:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setMarginRight(parentStyle->marginRight());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setMarginRight(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setMarginRight(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::PaddingTop:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setPaddingTop(parentStyle->paddingTop());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setPaddingTop(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setPaddingTop(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::PaddingRight:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setPaddingRight(parentStyle->paddingRight());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setPaddingRight(ComputedStyle::initialPadding());
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setPaddingRight(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::PaddingBottom:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setPaddingBottom(parentStyle->paddingBottom());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setPaddingBottom(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setPaddingBottom(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::PaddingLeft:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setPaddingLeft(parentStyle->paddingLeft());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setPaddingLeft(Length(Length::Fixed, 0));
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length ||
                                    cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage);
                    style->setPaddingLeft(convertValueToLength(cssValues[k].valueKind(), cssValues[k].value()));
                }
                break;
            case CSSStyleValuePair::KeyKind::Opacity:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_opacity = parentStyle->m_opacity;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_opacity = 1.0;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Number) {
                    float beforeClip = cssValues[k].numberValue();
                    style->m_opacity = beforeClip < 0 ? 0 : (beforeClip > 1.0 ? 1.0: beforeClip);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::OverflowX:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_overflowX = parentStyle->overflowX();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_overflowX = OverflowValue::VisibleOverflow;
                } else {
                    style->m_overflowX = cssValues[k].overflowValueX();
                }
                break;
/*            case CSSStyleValuePair::KeyKind::OverflowY:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_overflowY = parentStyle->m_overflowY;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // TODO : should assign initial value
                } else {
                    style->m_overflowY = cssValues[k].overflowValueY();
                }
                break;
*/
            case CSSStyleValuePair::KeyKind::Visibility:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_visibility = parentStyle->visibility();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->m_inheritedStyles.m_visibility = VisibilityValue::VisibleVisibilityValue;
                } else {
                    style->m_inheritedStyles.m_visibility = cssValues[k].visibility();
                }
                break;
            case CSSStyleValuePair::KeyKind::ZIndex:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_zIndex = parentStyle->m_zIndex;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial ||
                           cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_zIndex = 0;
                } else {
                    style->m_zIndex = cssValues[k].numberValue();
                }
                break;
            }
        }
    };

    auto chkPseudoClass = [](CSSStyleRule* rule, Element* e) -> bool {
        if (rule->m_pseudoClass == CSSStyleRule::None) {
            return true;
        }

        bool chk = true;
        if ((rule->m_pseudoClass & CSSStyleRule::Active) && ((e->state() & Node::NodeState::NodeStateActive) == 0)) {
            chk = false;
        }

        return chk;
    };

    // * selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j]->m_kind == CSSStyleRule::UniversalSelector &&
                    sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                apply(cssValues, ret, parent);
            }
        }
    }

    // *:active selector
    if ((element->state() & Node::NodeState::NodeStateActive)) {
        for (unsigned i = 0; i < m_sheets.size(); i ++) {
            CSSStyleSheet* sheet = m_sheets[i];
            for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
                if (sheet->m_rules[j]->m_kind == CSSStyleRule::UniversalSelector &&
                        sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    // type selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j]->m_kind == CSSStyleRule::TypeSelector &&
                    sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                if (sheet->m_rules[j]->m_ruleText->equals(element->localName())) {
                    auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    // type:active selector
    if ((element->state() & Node::NodeState::NodeStateActive)) {
        for (unsigned i = 0; i < m_sheets.size(); i ++) {
            CSSStyleSheet* sheet = m_sheets[i];
            for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
                if (sheet->m_rules[j]->m_kind == CSSStyleRule::TypeSelector &&
                        sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    if (sheet->m_rules[j]->m_ruleText->equals(element->localName())) {
                        auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                        apply(cssValues, ret, parent);
                    }
                }
            }
        }
    }

    // class selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j]->m_kind == CSSStyleRule::ClassSelector &&
                    sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                auto className = element->classNames();
                for (unsigned f = 0; f < className.size(); f ++) {
                    if (className[f]->equals(sheet->m_rules[j]->m_ruleText)) {
                        auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                        apply(cssValues, ret, parent);
                    }
                }
            }
        }
    }

    // class:active selector
    if ((element->state() & Node::NodeState::NodeStateActive)) {
        for (unsigned i = 0; i < m_sheets.size(); i ++) {
            CSSStyleSheet* sheet = m_sheets[i];
            for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
                if (sheet->m_rules[j]->m_kind == CSSStyleRule::ClassSelector &&
                        sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    auto className = element->classNames();
                    for (unsigned f = 0; f < className.size(); f ++) {
                        if (className[f]->equals(sheet->m_rules[j]->m_ruleText)) {
                            auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                            apply(cssValues, ret, parent);
                        }
                    }
                }
            }
        }
    }

    // id selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j]->m_kind == CSSStyleRule::IdSelector &&
                    sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                if (element->id()->equals(sheet->m_rules[j]->m_ruleText)) {
                    auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    // id:active selector
    if ((element->state() & Node::NodeState::NodeStateActive)) {
        for (unsigned i = 0; i < m_sheets.size(); i ++) {
            CSSStyleSheet* sheet = m_sheets[i];
            for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
                if (sheet->m_rules[j]->m_kind == CSSStyleRule::IdSelector &&
                        sheet->m_rules[j]->m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    if (element->id()->equals(sheet->m_rules[j]->m_ruleText)) {
                        auto cssValues = sheet->m_rules[j]->styleDeclaration()->m_cssValues;
                        apply(cssValues, ret, parent);
                    }
                }
            }
        }
    }

    //inline style

    auto inline_cssValues = element->inlineStyle()->m_cssValues;
    if(inline_cssValues.size()>0)
        apply(inline_cssValues, ret, parent);

    ret->loadResources(element->document()->window()->starFish());
    ret->arrangeStyleValues(parent);
    return ret;
}

void resolveDOMStyleInner(StyleResolver* resolver, Element* element, ComputedStyle* parentStyle, bool force = false)
{
    if (element->needsStyleRecalc() || force) {
        ComputedStyle* style = resolver->resolveStyle(element, parentStyle);
        if (!element->style() || compareStyle(element->style(), style) == ComputedStyleDamage::ComputedStyleDamageInherited) {
            force = force | true;
        }
        element->setStyle(style);
        element->clearNeedsStyleRecalc();
    }

    ComputedStyle* childStyle = nullptr;
    Node* child = element->firstChild();
    while (child) {
        if (child->isElement()) {
            resolveDOMStyleInner(resolver, child->asElement(), element->style(), force);
        } else {
            if (force) {
                if (childStyle == nullptr) {
                    childStyle = new ComputedStyle(element->style());
                    childStyle->loadResources(element->document()->window()->starFish());
                    childStyle->arrangeStyleValues(element->style());
                }
                child->setStyle(childStyle);
            }
        }
        child = child->nextSibling();
    }
}

void StyleResolver::resolveDOMStyle(Document* document)
{
    Node* child = document->firstChild();
    while (child) {
        if (child->isElement()) {
            resolveDOMStyleInner(this, child->asElement(), document->style());
        }
        child = child->nextSibling();
    }
}

void dump(Node* node, unsigned depth)
{
    for (unsigned i = 0; i < depth; i ++) {
        printf("  ");
    }

    node->dumpStyle();
    printf("\n");

    Node* child = node->firstChild();
    while (child) {
        if (child->isElement()) {
            dump(child, depth + 1);
        }
        child = child->nextSibling();
    }
}

void StyleResolver::dumpDOMStyle(Document* document)
{
    dump(document->asNode(), 0);
    printf("\n");
}

}
