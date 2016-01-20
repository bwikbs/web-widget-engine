#ifndef __StarFishStyle__
#define __StarFishStyle__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"
#include "platform/canvas/font/Font.h"

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
    NoneDisplayValue,
};

enum TextAlignValue {
    LeftTextAlignValue,
    RightTextAlignValue,
    CenterTextAlignValue,
    // JustifyTextAlignValue,
};

enum BackgroundRepeatValue {
	RepeatRepeatValue,
    NoRepeatRepeatValue,
    IntialRepeatValue,
};

enum BorderImageRepeatValue {
    StretchValue,
    RepeatValue,
    RoundValue,
    SpaceValue,
};

class SizeValueComponent {
public:
    enum ValueKind {
        Length,
        Percentage,
        Auto
    };
    SizeValueComponent()
        : m_valueKind(ValueKind::Auto),
          m_value{0}
    {
    }

    ValueKind m_valueKind;
    union {
        float m_floatValue;
        CSSLength m_length;
    } m_value;
};

class SizeValue : public gc {
    SizeValue()
    {
    }
    SizeValue(SizeValueComponent width)
        : m_width(width) {
    }
    SizeValue(SizeValueComponent width, SizeValueComponent height)
        : m_width(width),
          m_height(height) {
    }
public:
    static SizeValue* fromString(const char* value);
    SizeValueComponent m_width;
    SizeValueComponent m_height;
};

class ValueList;
class CSSStyleValuePair : public gc {
    friend class ValueList;
public:
    enum KeyKind {
        // <name> <- initial value
        // http://www.w3.org/TR/CSS21/visuren.html#propdef-display
        Display, // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
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
        // https://www.w3.org/TR/2011/REC-CSS2-20110607/colors.html#background-properties
        BackgroundColor, // color | <transparent> | inherit
        // https://www.w3.org/TR/css3-background/#the-background-size
        BackgroundSize, // [length | percentage | auto]{1,2} | cover | contain // initial value -> auto
        // https://www.w3.org/TR/CSS21/colors.html#propdef-background-repeat
        //BackgroundRepeat, // repeat | repeat-x | repeat-y | no-repeat | initial | inherit
        BackgroundRepeatX, // repeat | no-repeat | initial | inherit
        BackgroundRepeatY, // repeat | no-repeat | initial | inherit
        Bottom, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-bottom
        BorderImageRepeat, // <stretch> | repeat | round | space
        // https://www.w3.org/TR/css3-background/#the-border-image-repeat
        BorderImageSource,
        MarginBottom, // length | percentage | auto | inherit // <0>
        // https://www.w3.org/TR/CSS21/box.html#propdef-margin-bottom
        Opacity, // alphavalue | inherit // <1>
        // https://www.w3.org/TR/css3-color/#transparency
    };

    enum ValueKind {
        Inherit,
        Length,
        Percentage,
        Auto,
        None,
        Number, // real number values - https://www.w3.org/TR/CSS21/syndata.html#value-def-number
        StringValueKind,

        DisplayValueKind,
        TextAlignValueKind,

        Transparent,

        ValueListKind,

        //BackgroundSize
        Cover,
        Contain,
        SizeValueKind,  //(width: [length|percentage|auto], height: [length|percentage|auto]) pair

        BackgroundRepeatValueKind,

        BorderImageRepeatValueKind,
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

    TextAlignValue textAlignValue()
    {
        STARFISH_ASSERT(m_valueKind == TextAlignValueKind);
        return m_value.m_textAlign;
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

    SizeValue* sizeValue()
    {
        STARFISH_ASSERT(m_valueKind == SizeValueKind);
        return m_value.m_sizeValue;
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

    friend void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value);
    static CSSStyleValuePair fromString(const char* key, const char* value);
protected:
    KeyKind m_keyKind;
    ValueKind m_valueKind;

    union ValueData {
        float m_floatValue;
        DisplayValue m_display;
        TextAlignValue m_textAlign;
        CSSLength m_length;
        String* m_stringValue;
        SizeValue* m_sizeValue;
        BackgroundRepeatValue m_backgroundRepeatX;
        BackgroundRepeatValue m_backgroundRepeatY;
        BorderImageRepeatValue m_borderImageRepeat;
        ValueList* m_multiValue;
    } m_value;
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

    CSSStyleRule(Kind kind, String* ruleText)
    {
        m_kind = kind;
        m_ruleText = ruleText;
        m_styleDeclaration = new CSSStyleDeclaration();
    }

    CSSStyleDeclaration* styleDeclaration()
    {
        return m_styleDeclaration;
    }

protected:
    Kind m_kind;
    String* m_ruleText;
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
    ComputedStyle* resolveDocumentStyle();
    ComputedStyle* resolveStyle(Element* node, ComputedStyle* parent);
protected:
    std::vector<CSSStyleSheet*, gc_allocator<CSSStyleSheet*>> m_sheets;
};

}

#endif
