#ifndef __StarFishStyle__
#define __StarFishStyle__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"
#include "platform/canvas/font/Font.h"
#include "style/DefaultStyle.h"

namespace StarFish {

class ComputedStyle;
class Element;
class Document;

// FIXME
// for support javascript CSSStyleSheet Object
// we should store style rules that way(like CSSStyleSheet object)


// https://www.w3.org/TR/CSS21/syndata.html#value-def-length
class CSSLength {
public:
    enum Kind {
        PX,
        //em
        //ex
        //in
        //cm
        //mm
        //pt
        //pc
    };

    CSSLength(float f)
    {
        m_kind = PX;
        m_value = f;
    }

    Kind kind()
    {
        return m_kind;
    }

    float value()
    {
        return m_value;
    }

    Length toLength(/* parentValue */)
    {
        if (m_kind == PX)
            return Length(Length::Fixed, m_value);
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
protected:
    Kind m_kind;
    float m_value;
};

// inline | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
enum DisplayValue {
    InlineDisplayValue, //initial value
    BlockDisplayValue,
    InlineBlockDisplayValue,
    NoneDisplayValue,
};

enum PositionValue {
    StaticPositionValue,
    RelativePositionValue,
    AbsolutePositionValue,
    // FixedPositionValue, // TODO
};

enum TextAlignValue {
    LeftTextAlignValue,
    RightTextAlignValue,
    CenterTextAlignValue,
    // JustifyTextAlignValue,
};

enum DirectionValue {
    LtrDirectionValue,
    RtlDirectionValue,
};

enum BackgroundSizeType {
    Cover,
    Contain,
    SizeValue,
    SizeNone,
};

enum BackgroundRepeatValue {
    RepeatRepeatValue,
    NoRepeatRepeatValue,
};

// Widget Engine will support only visible and hidden values.
enum OverflowValue {
    Visible,
    Hidden,
};

enum BorderImageRepeatValue {
    StretchValue,
    RepeatValue,
    RoundValue,
    SpaceValue,
};

enum BorderStyleValue {
    BNone,
    BSolid,
};

class ValueList;

class CSSStyleValuePair : public gc {
    friend class ValueList;
public:
    enum KeyKind {
        // <name> <- initial value
        // http://www.w3.org/TR/CSS21/visuren.html#propdef-display
        Display, // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
        // http://www.w3.org/TR/CSS21/visuren.html#choose-position
        Position, // <static> | relative | absolute | fixed | inherit
        // https://www.w3.org/TR/CSS21/visudet.html#the-width-property
        Width, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/visudet.html#the-height-property
        Height, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/colors.html
        Color, // color | inherit // initial value -> depends on user agent
        // https://www.w3.org/TR/CSS21/fonts.html#font-size-props
        FontSize, // absolute-size | relative-size | length | percentage | inherit // initial value -> medium
        // https://www.w3.org/TR/CSS21/text.html#propdef-text-align
        TextAlign, // left | right | center | justify | <inherit>
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-direction
        Direction, // <ltr> | rtl | inherit
        // https://www.w3.org/TR/2011/REC-CSS2-20110607/colors.html#background-properties
        BackgroundColor, // color | <transparent> | inherit
        // https://www.w3.org/TR/CSS21/colors.html#propdef-background-image
        BackgroundImage, // uri | <none> | inherit
        // https://www.w3.org/TR/css3-background/#the-background-size
        BackgroundSize, // [length | percentage | auto]{1,2} | cover | contain // initial value -> auto
        // https://www.w3.org/TR/CSS21/colors.html#propdef-background-repeat
        //BackgroundRepeat, // repeat | repeat-x | repeat-y | no-repeat | initial | inherit
        BackgroundRepeatX, // repeat | no-repeat | initial | inherit
        BackgroundRepeatY, // repeat | no-repeat | initial | inherit
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-bottom
        Bottom, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-left
        Left, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/box.html#border-color-properties
        // BorderColor // color | transparent | inherit
        BorderTopColor, // color | transparent | inherit // initial value -> the value of 'color' property
        BorderRightColor, // color | transparent | inherit // initial value -> the value of 'color' property
        BorderBottomColor, // color | transparent | inherit // initial value -> the value of 'color' property
        BorderLeftColor, // color | transparent | inherit // initial value -> the value of 'color' property
        // https://www.w3.org/TR/css3-background/#the-border-image-repeat
        BorderImageRepeat, // <stretch> | repeat | round | space {1,2}
        // https://www.w3.org/TR/css3-background/#border-image-slice
        BorderImageSlice, // number | percentage {1,4} && fill? // initial value -> 100%
        // https://www.w3.org/TR/css3-background/#the-border-image-source
        BorderImageSource, // none | <image>
        // https://www.w3.org/TR/CSS21/box.html#border-style-properties
        // BorderStyle, // border-style(<none> | hidden | dotted | dashed | solid | double | groove | ridge | inset | outset) | inherit
        BorderTopStyle, // border-style(<none> | solid) | inherit -> We doesn't support all type because of implementation issues of Evas
        BorderRightStyle, // border-style(<none> | solid) | inherit -> We doesn't support all type because of implementation issues of Evas
        BorderBottomStyle, // border-style(<none> | solid) | inherit -> We doesn't support all type because of implementation issues of Evas
        BorderLeftStyle, // border-style(<none> | solid) | inherit -> We doesn't support all type because of implementation issues of Evas
        // https://www.w3.org/TR/CSS21/box.html#border-width-properties
        // BorderWidth, // border-width(thin | <medium> | thick | length) | inherit
        BorderTopWidth, // border-width(thin | <medium> | thick | length) | inherit
        BorderRightWidth, // border-width(thin | <medium> | thick | length) | inherit
        BorderBottomWidth, // border-width(thin | <medium> | thick | length) | inherit
        BorderLeftWidth, // border-width(thin | <medium> | thick | length) | inherit
        // https://www.w3.org/TR/CSS21/box.html#propdef-margin-bottom
        MarginBottom, // length | percentage | auto | inherit // <0>
        // https://www.w3.org/TR/CSS21/box.html#propdef-margin-left
        MarginLeft, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS2/box.html#propdef-margin-right
        MarginRight, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/css3-color/#transparency
        Opacity, // alphavalue | inherit // <1>
        // https://www.w3.org/TR/2011/REC-CSS2-20110607/visufx.html#propdef-overflow
        //Overflow, // visible | hidden | scroll | auto | inherit // Initial value -> visible
        OverflowX, // visible | hidden | scroll | auto | inherit // Initial value -> visible
        OverflowY, // visible | hidden | scroll | auto | inherit // Initial value -> visible
    };

    enum ValueKind {
        Initial,
        Inherit,
        Length,
        Percentage,
        Auto,
        None,
        Number, // real number values - https://www.w3.org/TR/CSS21/syndata.html#value-def-number
        StringValueKind,

        DisplayValueKind,
        PositionValueKind,
        TextAlignValueKind,
        DirectionValueKind,

        Transparent,

        ValueListKind,

        //BackgroundSize
        Cover,
        Contain,

        BackgroundRepeatValueKind,
        BorderImageRepeatValueKind,

        LargerFontSize,
        SmallerFontSize,

        // border-style
        BorderNone,
        BorderSolid,

        // border-width
        BorderThin,
        BorderMedium,
        BorderThick,

        OverflowValueKind,
    };

    CSSStyleValuePair()
        : m_value{0}
    {

    }

    KeyKind keyKind()
    {
        return m_keyKind;
    }

    ValueKind valueKind()
    {
        return m_valueKind;
    }

    bool isAuto()
    {
        return valueKind() == Auto;
    }

    bool isInherit()
    {
        return valueKind() == Inherit;
    }

    DisplayValue displayValue()
    {
        STARFISH_ASSERT(m_valueKind == DisplayValueKind);
        return m_value.m_display;
    }

    PositionValue positionValue()
    {
        STARFISH_ASSERT(m_valueKind == PositionValueKind);
        return m_value.m_position;
    }

    TextAlignValue textAlignValue()
    {
        STARFISH_ASSERT(m_valueKind == TextAlignValueKind);
        return m_value.m_textAlign;
    }

    DirectionValue directionValue()
    {
        STARFISH_ASSERT(m_valueKind == DirectionValueKind);
        return m_value.m_direction;
    }

    CSSLength lengthValue()
    {
        STARFISH_ASSERT(m_valueKind == Length);
        return m_value.m_length;
    }

    float numberValue() {
        STARFISH_ASSERT(m_valueKind == Number);
        return m_value.m_floatValue;
    }

    // 0~1
    float percentageValue()
    {
        STARFISH_ASSERT(m_valueKind == Percentage);
        return m_value.m_floatValue;
    }

    String* stringValue()
    {
        STARFISH_ASSERT(m_valueKind == StringValueKind);
        return m_value.m_stringValue;
    }

    BackgroundRepeatValue backgroundRepeatXValue()
    {
    	STARFISH_ASSERT(m_valueKind == BackgroundRepeatValueKind);
    	return m_value.m_backgroundRepeatX;
    }

    BackgroundRepeatValue backgroundRepeatYValue()
	{
		STARFISH_ASSERT(m_valueKind == BackgroundRepeatValueKind);
		return m_value.m_backgroundRepeatY;
	}

    BorderImageRepeatValue borderImageRepeatValue() {
        STARFISH_ASSERT(m_valueKind == BorderImageRepeatValueKind);
        return m_value.m_borderImageRepeat;
    }

    ValueList* multiValue() {
        STARFISH_ASSERT(m_valueKind == ValueListKind);
        return m_value.m_multiValue;
    }

    OverflowValue overflowValueX() {
        STARFISH_ASSERT(m_valueKind == OverflowValueKind);
        return m_value.m_overflowX;
    }

    OverflowValue overflowValueY() {
        STARFISH_ASSERT(m_valueKind == OverflowValueKind);
        return m_value.m_overflowY;
    }

    friend void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value);
    friend void parseLength(CSSStyleValuePair& ret, const char* value);
    friend void parseUrl(CSSStyleValuePair& ret, const char* value);
    static CSSStyleValuePair fromString(const char* key, const char* value);
    static void parseFontSizeForKeyword(CSSStyleValuePair* ret, int col);

    union ValueData {
        float m_floatValue;
        DisplayValue m_display;
        PositionValue m_position;
        TextAlignValue m_textAlign;
        DirectionValue m_direction;
        CSSLength m_length;
        String* m_stringValue;
        BackgroundRepeatValue m_backgroundRepeatX;
        BackgroundRepeatValue m_backgroundRepeatY;
        BorderImageRepeatValue m_borderImageRepeat;
        ValueList* m_multiValue;
        OverflowValue m_overflowX;
        OverflowValue m_overflowY;
    };

    ValueData& value()
    {
        return m_value;
    }

protected:
    KeyKind m_keyKind;
    ValueKind m_valueKind;
    ValueData m_value;
};

class ValueList : public gc {
public:
    enum Separator {
        SpaceSeparator,
        CommaSeparator,
        SlashSeparator
    };

    ValueList()
        : m_separator(SpaceSeparator)
    {
    }

    ValueList(Separator sep)
        : m_separator(sep)
    {
    }

    void append(CSSStyleValuePair::ValueKind kind, CSSStyleValuePair::ValueData value)
    {
        m_valueKinds.push_back(kind);
        m_values.push_back(value);
    }

    CSSStyleValuePair::ValueKind getValueKindAtIndex(int idx)
    {
        return m_valueKinds[idx];
    }

    CSSStyleValuePair::ValueData& getValueAtIndex(int idx)
    {
        return m_values[idx];
    }

    unsigned int size()
    {
        return m_valueKinds.size();
    }
protected:
    Separator m_separator;
    std::vector<CSSStyleValuePair::ValueKind, gc_allocator<CSSStyleValuePair::ValueKind>> m_valueKinds;
    std::vector<CSSStyleValuePair::ValueData, gc_allocator<CSSStyleValuePair::ValueData>> m_values;
};

class CSSStyleDeclaration : public gc {
    friend class StyleResolver;
public:
    void addValuePair(CSSStyleValuePair p)
    {
        m_cssValues.push_back(p);
    }
protected:
    std::vector<CSSStyleValuePair, gc_allocator<CSSStyleValuePair>> m_cssValues;
};

class CSSStyleRule : public gc {
    friend class StyleResolver;
public:
    enum Kind {
        UniversalSelector,
        TypeSelector,
        ClassSelector,
        IdSelector,
    };

    enum PseudoClass {
        None,
        Active
    };

    CSSStyleRule(Kind kind, String* ruleText, PseudoClass pc)
    {
        m_kind = kind;
        m_ruleText = ruleText;
        m_styleDeclaration = new CSSStyleDeclaration();
        m_pseudoClass = pc;
    }

    CSSStyleDeclaration* styleDeclaration()
    {
        return m_styleDeclaration;
    }

protected:
    Kind m_kind;
    String* m_ruleText;
    PseudoClass m_pseudoClass;
    CSSStyleDeclaration* m_styleDeclaration;
};

class CSSStyleSheet : public gc {
    friend class StyleResolver;
public:
    void addRule(CSSStyleRule rule)
    {
        m_rules.push_back(rule);
    }
protected:
    std::vector<CSSStyleRule, gc_allocator<CSSStyleRule>> m_rules;
};

class StyleResolver {
public:
    void addSheet(CSSStyleSheet* rule)
    {
        STARFISH_ASSERT(rule);
        m_sheets.push_back(rule);
    }

    void resolveDOMStyle(Document* document);
    void dumpDOMStyle(Document* document);
    ComputedStyle* resolveDocumentStyle(StarFish* sf);
    friend Length convertValueToLength(CSSStyleValuePair::ValueKind kind, CSSStyleValuePair::ValueData data);
    ComputedStyle* resolveStyle(Element* node, ComputedStyle* parent);
protected:
    std::vector<CSSStyleSheet*, gc_allocator<CSSStyleSheet*>> m_sheets;
};

}

#endif
