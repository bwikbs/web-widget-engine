#ifndef __StarFishStyle__
#define __StarFishStyle__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"

namespace StarFish {

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
    JustifyTextAlignValue,
};

class CSSStyleValuePair : public gc {
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
        TextAlign, // left | right | center | justify | inherit
    };

    enum ValueKind {
        Inherit,
        Length,
        Percentage,
        Auto,
        StringValueKind,

        DisplayValueKind,
        TextAlignValueKind,
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

    friend void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value);
    static CSSStyleValuePair fromString(const char* key, const char* value);
protected:
    KeyKind m_keyKind;
    ValueKind m_valueKind;

    union {
        float m_floatValue;
        DisplayValue m_display;
        TextAlignValue m_textAlign;
        CSSLength m_length;
        String* m_stringValue;
    } m_value;
};

class CSSStyleDeclaration : public gc {
public:
    void addValuePair(CSSStyleValuePair p)
    {
        m_cssValues.push_back(p);
    }
protected:
    std::vector<CSSStyleValuePair, gc_allocator<CSSStyleValuePair>> m_cssValues;
};

class CSSStyleRule : public gc {
public:
    enum Kind {
        TagRule = 1,
        ClassRule = 10,
        IdRule = 100,
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
        m_sheets.push_back(rule);
    }
protected:
    std::vector<CSSStyleSheet*, gc_allocator<CSSStyleSheet*>> m_sheets;
};

}

#endif
