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

#define VALUE_IS_STRING(str) \
    (strcmp(value,str)) == 0

#define VALUE_IS_INHERIT() \
    VALUE_IS_STRING("inherit")

#define VALUE_IS_INITIAL() \
    VALUE_IS_STRING("initial")

#define VALUE_IS_NONE() \
    VALUE_IS_STRING("none")

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

void parseUrl(CSSStyleValuePair& ret, const char* value)
{
    int pathlen = strlen(value);
    if (pathlen >= 7) {
        ret.m_value.m_stringValue = String::fromUTF8(value + 5, pathlen - 7);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
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

void CSSStyleValuePair::parseFontSizeForKeyword(CSSStyleValuePair* ret, int col) {
    int mediumSize = DEFAULT_FONT_SIZE;
    int row = -1;
    if (mediumSize >= fontSizeTableMin && mediumSize <= fontSizeTableMax)
        row =  mediumSize - fontSizeTableMin;

    ret->m_valueKind = CSSStyleValuePair::ValueKind::Length;

    if (row >= 0)
        ret->m_value.m_length = CSSLength(strictFontSizeTable[row][col]);
    else
        ret->m_value.m_length = CSSLength(fontSizeFactors[col] * mediumSize);
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

CSSStyleValuePair CSSStyleValuePair::fromString(const char* key, const char* value)
{
    CSSStyleValuePair ret;
    if (strcmp(key, "display") == 0) {
        // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Display;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::DisplayValueKind;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if(VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("block")) {
            ret.m_value.m_display = DisplayValue::BlockDisplayValue;
        } else if (VALUE_IS_STRING("inline")) {
            ret.m_value.m_display = DisplayValue::InlineDisplayValue;
        } else if (VALUE_IS_STRING("inline-block")) {
            ret.m_value.m_display = DisplayValue::InlineBlockDisplayValue;
        } else if (VALUE_IS_STRING("none")) {
            ret.m_value.m_display = DisplayValue::NoneDisplayValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "position") == 0) {
        // <static> | relative | absolute | fixed | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Position;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::PositionValueKind;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("static")) {
            ret.m_value.m_position = PositionValue::StaticPositionValue;
        } else if (VALUE_IS_STRING("relative")) {
            ret.m_value.m_position = PositionValue::RelativePositionValue;
        } else if (VALUE_IS_STRING("absolute")) {
            ret.m_value.m_position = PositionValue::AbsolutePositionValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

    } else if (strcmp(key, "width") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Width;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "height") == 0) {
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Height;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "font-size") == 0) {
        // TODO add initial
        // absolute-size | relative-size | length | percentage | inherit // initial value -> medium
        //        O      |       O       |   O    |    O       |    O
        ret.m_keyKind = CSSStyleValuePair::KeyKind::FontSize;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_STRING("xx-small")) {
            parseFontSizeForKeyword(&ret, 0);
        } else if (VALUE_IS_STRING("x-small")) {
            parseFontSizeForKeyword(&ret, 1);
        } else if (VALUE_IS_STRING("small")) {
            parseFontSizeForKeyword(&ret, 2);
        } else if (VALUE_IS_STRING("medium")) {
            parseFontSizeForKeyword(&ret, 3);
        } else if (VALUE_IS_STRING("large")) {
            parseFontSizeForKeyword(&ret, 4);
        } else if (VALUE_IS_STRING("x-large")) {
            parseFontSizeForKeyword(&ret, 5);
        } else if (VALUE_IS_STRING("xx-large")) {
            parseFontSizeForKeyword(&ret, 6);
        } else if (VALUE_IS_STRING("larger")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::LargerFontSize;
        } else if (VALUE_IS_STRING("smaller")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::SmallerFontSize;
        } else if (VALUE_IS_INITIAL()) {
            parseFontSizeForKeyword(&ret, 3);
        }
        else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "color") == 0) {
        // color | inherit // initial value -> depends on user agent
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Color;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            // TODO check string has right color string
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "background-color") == 0) {
        // color | <transparent> | inherit
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundColor;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
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
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundImage;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;

        if (VALUE_IS_NONE()) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parseUrl(ret, value);
        }
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
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "text-decoration") == 0) {
        // none | [ underline || overline || line-through || blink ] | inherit // Initial value -> none
        ret.m_keyKind = CSSStyleValuePair::KeyKind::TextDecoration;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::TextDecorationKind;

        if(VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_NONE()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::None;
            ret.m_value.m_textDecoration = TextDecorationValue::None;
        } else if (VALUE_IS_STRING("underline")) {
            ret.m_value.m_textDecoration = TextDecorationValue::UnderLine;
        } else if (VALUE_IS_STRING("overline")) {
            ret.m_value.m_textDecoration = TextDecorationValue::OverLine;
        } else if (VALUE_IS_STRING("line-through")) {
            ret.m_value.m_textDecoration = TextDecorationValue::LineThrough;
        } else if (VALUE_IS_STRING("blink")) {
            ret.m_value.m_textDecoration = TextDecorationValue::Blink;
        }
    } else if (strcmp(key, "letter-spacing") == 0) {
        // normal | length | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::LetterSpacing;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("normal")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Normal;
        } else {
            parseLength(ret, value);
        }
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
        // <normal> | italic | oblique | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::FontStyle;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("normal")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
            ret.m_value.m_fontStyle = FontStyleValue::NormalFontStyleValue;
        } else if (VALUE_IS_STRING("italic")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
            ret.m_value.m_fontStyle = FontStyleValue::ItalicFontStyleValue;
        } else if (VALUE_IS_STRING("oblique")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::FontStyleValueKind;
            ret.m_value.m_fontStyle = FontStyleValue::ObliqueFontStyleValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "direction") == 0) {
        // <ltr> | rtl | inherit
        // TODO add initial
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Direction;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::DirectionValueKind;
        if (VALUE_IS_STRING("ltr")) {
            ret.m_value.m_direction = DirectionValue::LtrDirectionValue;
        } else if (VALUE_IS_STRING("rtl")) {
            ret.m_value.m_direction = DirectionValue::RtlDirectionValue;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
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
        // TODO add initial
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
        // TODO add initial
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
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Top;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "right") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Right;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "bottom") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Bottom;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "left") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Left;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "border-top-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopColor;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("transparent")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "border-right-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightColor;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("transparent")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "border-bottom-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomColor;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("transparent")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "border-left-color") == 0) {
        // color | transparent | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftColor;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("transparent")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }

    } else if (strcmp(key, "border-image-repeat") == 0) {
        // <stretch> | repeat | round | space {1,2}
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderImageRepeat;
        if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
            ret.m_value.m_multiValue = new ValueList();

            // TODO: find better way to parse axis data
            // 1) parse X-axis data
            if (startsWith(value, "stretch")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = StretchValue});
            } else if (startsWith(value, "repeat")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RepeatValue});
            } else if (startsWith(value, "round")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RoundValue});
            } else if (startsWith(value, "space")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = SpaceValue});
            }
            // 2) parse Y-axis data
            if (endsWith(value, "stretch")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = StretchValue});
            } else if (endsWith(value, "repeat")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RepeatValue});
            } else if (endsWith(value, "round")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = RoundValue});
            } else if (endsWith(value, "space")) {
                ret.m_value.m_multiValue->append(BorderImageRepeatValueKind, {.m_borderImageRepeat = SpaceValue});
            }
        }
    } else if (strcmp(key, "border-image-slice") == 0) {
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderImageSlice;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::ValueListKind;
            ret.m_value.m_multiValue = new ValueList();
            std::vector<String*, gc_allocator<String*>> tokens;
            DOMTokenList::tokenize(&tokens, String::fromUTF8(value));
            for (unsigned int i = 0; i < tokens.size(); i++) {
                const char* currentToken = tokens[i]->utf8Data();
                if (startsWith(currentToken, "fill")) {
                    ret.m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::StringValueKind, {0});
                } else if (endsWith(currentToken, "%")) {
                    float f;
                    sscanf(currentToken, "%f%%", &f);
                    ret.m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::Percentage, {.m_floatValue = (f / 100.f)});
                } else {
                    char* pEnd;
                    double d = strtod (currentToken, &pEnd);
                    STARFISH_ASSERT(pEnd == currentToken + tokens[i]->length());
                    ret.m_value.m_multiValue->append(CSSStyleValuePair::ValueKind::Number, {.m_floatValue = (float)d});
                }
            }
        }
    } else if (strcmp(key, "border-image-source") == 0) {
        // none | <image>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderImageSource;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::None;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if(VALUE_IS_STRING("none")) {
            // Do nothing!!
        } else {
            // TODO: parse "url()"
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "border-top-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopStyle;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("solid")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
        }
    } else if (strcmp(key, "border-right-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightStyle;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("solid")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
        }
    } else if (strcmp(key, "border-bottom-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomStyle;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("solid")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
        }
    } else if (strcmp(key, "border-left-style") == 0) {
        // border-style(<none> | solid) | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftStyle;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderNone;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("solid")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderSolid;
        }
    } else if (strcmp(key, "border-top-width") == 0) {
        // border-width | inherit // initial value -> medium ('thin' <='medium' <= 'thick')
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderTopWidth;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("thin")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
        } else if (VALUE_IS_STRING("medium")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
        } else if (VALUE_IS_STRING("thick")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
        } else {
            parseLength(ret, value);
        }
    } else if (strcmp(key, "border-right-width") == 0) {
        // border-width | inherit // initial value -> medium ('thin' <='medium' <= 'thick')
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderRightWidth;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("thin")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
        } else if (VALUE_IS_STRING("medium")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
        } else if (VALUE_IS_STRING("thick")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
        } else {
            parseLength(ret, value);
        }
    } else if (strcmp(key, "border-bottom-width") == 0) {
        // border-width | inherit // initial value -> medium ('thin' <='medium' <= 'thick')
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderBottomWidth;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("thin")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
        } else if (VALUE_IS_STRING("medium")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
        } else if (VALUE_IS_STRING("thick")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
        } else {
            parseLength(ret, value);
        }
    } else if (strcmp(key, "border-left-width") == 0) {
        // border-width | inherit // initial value -> medium ('thin' <='medium' <= 'thick')
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderLeftWidth;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else if (VALUE_IS_STRING("thin")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThin;
        } else if (VALUE_IS_STRING("medium")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderMedium;
        } else if (VALUE_IS_STRING("thick")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderThick;
        } else {
            parseLength(ret, value);
        }
    } else if (strcmp(key, "margin-top") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginTop;

        if (VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "margin-bottom") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginBottom;

        if (VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "margin-left") == 0) {
        // length | percentage | auto | inherit <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginLeft;

        if (VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "margin-right") == 0) {
        // length | percentage | auto | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::MarginRight;

        if (VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "padding-top") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingTop;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "padding-right") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingRight;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "padding-bottom") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingBottom;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "padding-left") == 0) {
        // length | percentage | inherit  <0>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::PaddingLeft;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_INITIAL()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Initial;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "opacity") == 0) {
        // alphavalue | inherit <1>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Opacity;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Number;

        if (VALUE_IS_INITIAL()) {
            ret.m_value.m_floatValue = 1.0f;
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
/*    } else if (strcmp(key, "overflow-y") == 0) {
        // visible | hidden | scroll(X) | auto(X) | inherit // initial value -> visible
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
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    return ret;
}

String* CSSStyleDeclaration::color()
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::Color
                && m_cssValues.at(i).valueKind() == CSSStyleValuePair::ValueKind::StringValueKind)
            return m_cssValues.at(i).stringValue();
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setColor(String* value)
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::Color
                && m_cssValues.at(i).valueKind() == CSSStyleValuePair::ValueKind::StringValueKind) {
            m_cssValues.at(i).setStringValue(value);
        }
    }
    CSSStyleValuePair ret;
    ret.setKeyKind(CSSStyleValuePair::KeyKind::Color);
    ret.setValueKind(CSSStyleValuePair::ValueKind::StringValueKind);
    ret.setStringValue(value);
    m_cssValues.push_back(ret);
}

String* CSSStyleDeclaration::direction()
{
    for(auto itr = m_cssValues.begin(); itr != m_cssValues.end(); ++itr) {
        CSSStyleValuePair v = *itr;
        if(v.keyKind() == CSSStyleValuePair::KeyKind::Direction) {
            if(v.directionValue() == LtrDirectionValue) {
                return String::createASCIIString("ltr");
            } else if(v.directionValue() == RtlDirectionValue) {
                return String::createASCIIString("rtl");
            }
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setLengthValue(CSSStyleValuePair* pair, const char* value)
{
    if (VALUE_IS_STRING("auto")) {
        pair->setValueKind(CSSStyleValuePair::ValueKind::Auto);
    } else if (VALUE_IS_INHERIT()) {
        pair->setValueKind(CSSStyleValuePair::ValueKind::Inherit);
    } else if (VALUE_IS_INITIAL()) {
        pair->setValueKind(CSSStyleValuePair::ValueKind::Initial);
    } else {
        parsePercentageOrLength(*pair, value);
    }
}

String* CSSStyleDeclaration::marginTop()
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginTop) {
            return m_cssValues.at(i).lengthOrPercentageToString();
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setMarginTop(const char* value)
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginTop) {
            setLengthValue(&m_cssValues.at(i), value);
            return;
        }
    }
    CSSStyleValuePair ret;
    ret.setKeyKind(CSSStyleValuePair::KeyKind::MarginTop);
    setLengthValue(&ret, value);
    m_cssValues.push_back(ret);
}

String* CSSStyleDeclaration::marginBottom()
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginBottom) {
            return m_cssValues.at(i).lengthOrPercentageToString();
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setMarginBottom(const char* value)
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginBottom) {
            setLengthValue(&m_cssValues.at(i), value);
            return;
        }
    }
    CSSStyleValuePair ret;
    ret.setKeyKind(CSSStyleValuePair::KeyKind::MarginBottom);
    setLengthValue(&ret, value);
    m_cssValues.push_back(ret);
}

String* CSSStyleDeclaration::marginRight()
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginRight) {
            return m_cssValues.at(i).lengthOrPercentageToString();
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setMarginRight(const char* value)
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginRight) {
            setLengthValue(&m_cssValues.at(i), value);
            return;
        }
    }
    CSSStyleValuePair ret;
    ret.setKeyKind(CSSStyleValuePair::KeyKind::MarginRight);
    setLengthValue(&ret, value);
    m_cssValues.push_back(ret);
}

String* CSSStyleDeclaration::marginLeft()
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginLeft) {
            return m_cssValues.at(i).lengthOrPercentageToString();
        }
    }
    return String::emptyString;
}

void CSSStyleDeclaration::setMarginLeft(const char* value)
{
    for (unsigned i = 0; i < m_cssValues.size(); i++) {
        if (m_cssValues.at(i).keyKind() == CSSStyleValuePair::KeyKind::MarginLeft) {
            setLengthValue(&m_cssValues.at(i), value);
            return;
        }
    }
    CSSStyleValuePair ret;
    ret.setKeyKind(CSSStyleValuePair::KeyKind::MarginLeft);
    setLengthValue(&ret, value);
    m_cssValues.push_back(ret);
}

String* CSSStyleDeclaration::margin()
{
    String* sum;
    String* space = String::fromUTF8(" ");
    String* top = marginTop();
    if (!top->equals(String::emptyString)) {
        String* right = marginRight();
        if (!right->equals(String::emptyString)) {
            String* bottom = marginBottom();
            if (!bottom->equals(String::emptyString)) {
                String* left = marginLeft();
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
    unsigned len = tokens.size();
    if (len > 0)
        setMarginTop(tokens[0]->utf8Data());
    if (len > 1)
            setMarginRight(tokens[1]->utf8Data());
    if (len > 2)
            setMarginBottom(tokens[2]->utf8Data());
    if (len > 3)
            setMarginLeft(tokens[3]->utf8Data());
}

String* CSSStyleDeclaration::position()
{
    for(auto itr = m_cssValues.begin(); itr != m_cssValues.end(); ++itr) {
        CSSStyleValuePair v = *itr;
        if(v.keyKind() == CSSStyleValuePair::KeyKind::Position) {
            switch(v.positionValue()) {
                case PositionValue::StaticPositionValue:
                    return String::fromUTF8("static");
                case PositionValue::RelativePositionValue:
                    return String::fromUTF8("relative");
                case PositionValue::AbsolutePositionValue:
                    return String::fromUTF8("absolute");
                case PositionValue::FixedPositionValue:
                    return String::fromUTF8("fixed");
                default: break;
            }
        }
    }
    return String::emptyString;
}

String* CSSStyleDeclaration::width()
{
    for(auto itr = m_cssValues.begin(); itr != m_cssValues.end(); ++itr) {
        CSSStyleValuePair v = *itr;
        if(v.keyKind() == CSSStyleValuePair::KeyKind::Width) {
            switch(v.valueKind()) {
                case CSSStyleValuePair::ValueKind::Length:
                case CSSStyleValuePair::ValueKind::Percentage:
                    return v.lengthOrPercentageToString();
                default: break;
            }
        }
    }
    return String::emptyString;
}

String* CSSStyleDeclaration::visibility()
{
    for(auto itr = m_cssValues.begin(); itr != m_cssValues.end(); ++itr) {
        CSSStyleValuePair v = *itr;
        if(v.keyKind() == CSSStyleValuePair::KeyKind::Visibility && v.valueKind() == CSSStyleValuePair::ValueKind::VisibilityKind) {
            switch(v.visibility()) {
                case VisibilityValue::VisibleVisibilityValue:
                    return String::fromUTF8("visible");
                case VisibilityValue::HiddenVisibilityValue:
                    return String::fromUTF8("hidden");
                default: break;
            }
        }
    }
    return String::emptyString;
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
                    // TODO : should assign initial value
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::LargerFontSize) {
                    style->m_inheritedStyles.m_fontSize = (parentStyle->m_inheritedStyles.m_fontSize) * 1.2;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::SmallerFontSize) {
                    style->m_inheritedStyles.m_fontSize = (parentStyle->m_inheritedStyles.m_fontSize) / 1.2;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length);
                    style->m_inheritedStyles.m_fontSize = cssValues[k].lengthValue().toLength().fixed();
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
            case CSSStyleValuePair::KeyKind::TextAlign:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_textAlign = parentStyle->m_inheritedStyles.m_textAlign;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // TODO : should assign initial value
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::TextAlignValueKind);
                    style->m_inheritedStyles.m_textAlign = cssValues[k].textAlignValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::TextDecoration:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setTextDecoration(parentStyle->textDecoration());
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
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // TODO: Use initialized value
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setBgImage(parentStyle->bgImage());
                } else {
                    style->setBgImage(cssValues[k].stringValue());
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
                    // Use initialized value
                    style->setBorderImageRepeatX(StretchValue);
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    // TODO: Prevent parentStyle->surround() from creating object for this
                    style->borderImageRepeatInherit(parentStyle);
                } else {
                    style->setBorderImageRepeatX(cssValues[k].multiValue()->getValueAtIndex(0).m_borderImageRepeat);
                    style->setBorderImageRepeatY(cssValues[k].multiValue()->getValueAtIndex(1).m_borderImageRepeat);
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageSlice:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // Use initialized value
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    // TODO: Prevent parentStyle->surround() from creating object for this
                    style->borderImageSliceInherit(parentStyle);
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
                    // TODO : should assign initial value(none)
                } else {
                    //STARFISH_ASSERT(CSSStyleValuePair::ValueKind::StringValueKind == cssValues[k].valueKind());
                    style->setBorderImageSource(cssValues[k].stringValue());
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
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->setMarginBottom(parentStyle->marginBottom());
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    style->setMarginBottom(Length(Length::Fixed, 0));
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
                    style->setPaddingRight(Length(Length::Fixed, 0));
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
                    style->m_overflowX = parentStyle->m_overflowX;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Initial) {
                    // TODO : should assign initial value
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
            if (sheet->m_rules[j].m_kind == CSSStyleRule::UniversalSelector &&
                    sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                apply(cssValues, ret, parent);
            }
        }
    }

    // *:active selector
    if ((element->state() & Node::NodeState::NodeStateActive)) {
        for (unsigned i = 0; i < m_sheets.size(); i ++) {
            CSSStyleSheet* sheet = m_sheets[i];
            for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
                if (sheet->m_rules[j].m_kind == CSSStyleRule::UniversalSelector &&
                        sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    // type selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j].m_kind == CSSStyleRule::TypeSelector &&
                    sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                if (sheet->m_rules[j].m_ruleText->equals(element->localName())) {
                    auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
                if (sheet->m_rules[j].m_kind == CSSStyleRule::TypeSelector &&
                        sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    if (sheet->m_rules[j].m_ruleText->equals(element->localName())) {
                        auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
            if (sheet->m_rules[j].m_kind == CSSStyleRule::ClassSelector &&
                    sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                auto className = element->classNames();
                for (unsigned f = 0; f < className.size(); f ++) {
                    if (className[f]->equals(sheet->m_rules[j].m_ruleText)) {
                        auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
                if (sheet->m_rules[j].m_kind == CSSStyleRule::ClassSelector &&
                        sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    auto className = element->classNames();
                    for (unsigned f = 0; f < className.size(); f ++) {
                        if (className[f]->equals(sheet->m_rules[j].m_ruleText)) {
                            auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
            if (sheet->m_rules[j].m_kind == CSSStyleRule::IdSelector &&
                    sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::None) {
                if (element->id()->equals(sheet->m_rules[j].m_ruleText)) {
                    auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
                if (sheet->m_rules[j].m_kind == CSSStyleRule::IdSelector &&
                        sheet->m_rules[j].m_pseudoClass == CSSStyleRule::PseudoClass::Active) {
                    if (element->id()->equals(sheet->m_rules[j].m_ruleText)) {
                        auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
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
    ret->arrangeStyleValues();
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
                    childStyle->arrangeStyleValues();
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
