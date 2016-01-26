#ifndef __StarFishStyle__
#define __StarFishStyle__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"
#include "platform/canvas/font/Font.h"
#include "style/DefaultStyle.h"
#include "style/UnitHelper.h"
#include "dom/EventTarget.h"
#include <sstream>


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
        EM,
        EX,
        IN,
        CM,
        MM,
        PT,
        PC
    };

    CSSLength(float f)
    {
        m_kind = PX;
        m_value = f;
    }

    CSSLength(Kind kind, float f)
    {
        m_kind = kind;
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
        // absolute length
        if (m_kind == PX)
            return Length(Length::Fixed, m_value);
        else if (m_kind == CM)
            return Length(Length::Fixed, convertFromCmToPx(m_value));
        else if (m_kind == MM)
            return Length(Length::Fixed, convertFromMmToPx(m_value));
        else if (m_kind == IN)
            return Length(Length::Fixed, convertFromInToPx(m_value));
        else if (m_kind == PC)
            return Length(Length::Fixed, convertFromPcToPx(m_value));
        else if (m_kind == PT)
            return Length(Length::Fixed, convertFromPtToPx(m_value));

        // font-relative length
        else if (m_kind == EM)
            return Length(Length::EmToBeFixed, m_value);
        else if (m_kind == EX)
            return Length(Length::ExToBeFixed, m_value);

        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    String* toString()
    {
        std::stringstream ss (std::stringstream::in | std::stringstream::out);
        ss << m_value;
        std::string stdStr = ss.str();
        if (m_kind == PX)
            return String::fromUTF8(stdStr.append("px").c_str());
        else if (m_kind == CM)
            return String::fromUTF8(stdStr.append("cm").c_str());
        else if (m_kind == MM)
            return String::fromUTF8(stdStr.append("mm").c_str());
        else if (m_kind == IN)
            return String::fromUTF8(stdStr.append("in").c_str());
        else if (m_kind == PC)
            return String::fromUTF8(stdStr.append("pc").c_str());
        else if (m_kind == PT)
            return String::fromUTF8(stdStr.append("pt").c_str());
        else if (m_kind == EM)
            return String::fromUTF8(stdStr.append("em").c_str());
        else if (m_kind == EX)
            return String::fromUTF8(stdStr.append("ex").c_str());

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
    FixedPositionValue, // TODO
};

enum TextAlignValue {
    NamelessTextAlignValue, // Depends on direction
    LeftTextAlignValue,
    RightTextAlignValue,
    CenterTextAlignValue,
    JustifyTextAlignValue,
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
    VisibleOverflow,
    HiddenOverflow,
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

enum TextDecorationValue {
    NoneTextDecorationValue,
    UnderLineTextDecorationValue,
    OverLineTextDecorationValue,
    LineThroughTextDecorationValue,
    BlinkTextDecorationValue,
};

enum FontStyleValue {
    NormalFontStyleValue,
    ItalicFontStyleValue,
    ObliqueFontStyleValue,
};

// Widget Engine will support only clip and ellipsis values.
enum TextOverflowValue {
    ClipTextOverflowValue,
    EllipsisTextOverflowValue,
};

enum VisibilityValue {
    VisibleVisibilityValue,
    HiddenVisibilityValue,
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
        // https://www.w3.org/TR/CSS2/fonts.html#propdef-font-style
        FontStyle, // <normal> | italic | oblique | inherit
        // https://www.w3.org/TR/CSS21/text.html#propdef-text-align
        TextAlign, // left | right | center | justify | <inherit>
        // https://www.w3.org/TR/CSS2/text.html#propdef-text-decoration
        TextDecoration,  // none | [ underline || overline || line-through || blink ] | inherit // Initial value -> none
        // https://www.w3.org/TR/2010/WD-css3-text-20101005/#text-overflow0
        TextOverflow, // <clip> | ellipsis | string
        // https://www.w3.org/TR/CSS2/text.html#propdef-letter-spacing
        LetterSpacing, //  normal | length | inherit // Initial -> normal
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
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-top
        Top, // length | percentage | <auto> | inherit
        // https://www.w3.org/TR/CSS21/visuren.html#propdef-right
        Right, // length | percentage | <auto> | inherit
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
        // https://www.w3.org/TR/2011/REC-CSS2-20110607/box.html#propdef-margin-top
        MarginTop, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-margin-bottom
        MarginBottom, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-margin-left
        MarginLeft, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS2/box.html#propdef-margin-right
        MarginRight, // length | percentage | auto | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-padding-top
        PaddingTop, // length | percentage | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-padding-right
        PaddingRight, // length | percentage | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-padding-bottom
        PaddingBottom, // length | percentage | inherit // Initial value -> 0
        // https://www.w3.org/TR/CSS21/box.html#propdef-padding-left
        PaddingLeft, // length | percentage | inherit // Initial value -> 0
        // https://www.w3.org/TR/css3-color/#transparency
        Opacity, // alphavalue | inherit // <1>
        // https://www.w3.org/TR/2011/REC-CSS2-20110607/visufx.html#propdef-overflow
        //Overflow, // visible | hidden | scroll | auto | inherit // Initial value -> visible
        OverflowX, // visible | hidden | scroll | auto | inherit // Initial value -> visible
        //OverflowY, // visible | hidden | scroll | auto | inherit // Initial value -> visible
        // https://www.w3.org/TR/CSS2/visufx.html#visibility
        Visibility,  // visible | hidden | collapse | inherit // Initial value -> visible
        // http://www.w3.org/TR/CSS2/visuren.html#z-index
        ZIndex, //  <auto> | integer | inherit
    };

    enum ValueKind {
        Initial,
        Inherit,
        Length,
        Percentage,
        Auto,
        None,
        Number, // real number values - https://www.w3.org/TR/CSS21/syndata.html#value-def-number
        Normal,
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

        XXSmallFontSizeValueKind,
        XSmallFontSizeValueKind,
        SmallFontSizeValueKind,
        MediumFontSizeValueKind,
        LargeFontSizeValueKind,
        XLargeFontSizeValueKind,
        XXLargeFontSizeValueKind,
        LargerFontSizeValueKind,
        SmallerFontSizeValueKind,

        FontStyleValueKind,
        TextOverflowValueKind,

        // border-style
        BorderNone,
        BorderSolid,

        // border-width
        BorderThin,
        BorderMedium,
        BorderThick,

        OverflowValueKind,
        TextDecorationKind,
        VisibilityKind,
    };

    CSSStyleValuePair()
        : m_value{0}
    {

    }

    KeyKind keyKind()
    {
        return m_keyKind;
    }

    void setKeyKind(KeyKind kind)
    {
        m_keyKind = kind;
    }

    ValueKind valueKind()
    {
        return m_valueKind;
    }

    void setValueKind(ValueKind kind)
    {
        m_valueKind = kind;
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

    TextOverflowValue textOverflowValue()
    {
        STARFISH_ASSERT(m_valueKind == TextOverflowValueKind);
        return m_value.m_textOverflow;
    }

    FontStyleValue fontStyleValue()
    {
        STARFISH_ASSERT(m_valueKind == FontStyleValueKind);
        return m_value.m_fontStyle;
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

    void setStringValue(String* value)
    {
        STARFISH_ASSERT(m_valueKind == StringValueKind);
        m_value.m_stringValue = value;
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

/*    OverflowValue overflowValueY() {
        STARFISH_ASSERT(m_valueKind == OverflowValueKind);
        return m_value.m_overflowY;
    }
*/
    VisibilityValue visibility()
    {
        STARFISH_ASSERT(m_valueKind == VisibilityKind);
        return m_value.m_visibility;
    }

    TextDecorationValue textDecoration() {
        return m_value.m_textDecoration;
    }

    friend void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value);
    friend void parseLength(CSSStyleValuePair& ret, const char* value);
    friend void parseUrl(CSSStyleValuePair& ret, const char* value);
    static CSSStyleValuePair fromString(const char* key, const char* value);

    union ValueData {
        float m_floatValue;
        DisplayValue m_display;
        PositionValue m_position;
        FontStyleValue m_fontStyle;
        TextOverflowValue m_textOverflow;
        TextAlignValue m_textAlign;
        DirectionValue m_direction;
        CSSLength m_length;
        String* m_stringValue;
        BackgroundRepeatValue m_backgroundRepeatX;
        BackgroundRepeatValue m_backgroundRepeatY;
        BorderImageRepeatValue m_borderImageRepeat;
        ValueList* m_multiValue;
        OverflowValue m_overflowX;
//        OverflowValue m_overflowY;
        VisibilityValue m_visibility;
        TextDecorationValue m_textDecoration;
    };

    ValueData& value()
    {
        return m_value;
    }

    String* percentageToString(float f)
    {
        float v = f * 100.f;
        if (v == std::floor(v))
            return String::fromUTF8(std::to_string((int) std::floor(v)).append("%").c_str());

        return String::fromUTF8(std::to_string(v).append("%").c_str());
    }

    String* lengthOrPercentageToString()
    {
        if (valueKind() == CSSStyleValuePair::ValueKind::Auto)
            return String::fromUTF8("auto");
        else if (valueKind() == CSSStyleValuePair::ValueKind::Inherit)
            return String::fromUTF8("Inherit");
        else if (valueKind() == CSSStyleValuePair::ValueKind::Initial)
            return String::fromUTF8("Initial");
        else if (valueKind() == CSSStyleValuePair::ValueKind::Length)
            return lengthValue().toString();
        else if (valueKind() == CSSStyleValuePair::ValueKind::Percentage)
            return percentageToString(percentageValue());
        else
            return nullptr;
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

class CSSStyleDeclaration : public EventTarget<ScriptWrappable> {
    friend class StyleResolver;
public:
    CSSStyleDeclaration(Document* document)
        : m_document(document)
    {
        initScriptWrappable(this);
    }

    void addValuePair(CSSStyleValuePair p)
    {
        for(auto itr = m_cssValues.begin(); itr != m_cssValues.end(); ++itr) {
            CSSStyleValuePair v = *itr;
            if(v.keyKind() == p.keyKind()) {
                m_cssValues.erase(itr); // FIXME: m_cssValues should be a list
                break;
            }
        }
        m_cssValues.push_back(p);
    }

    Document* document()
    {
        return m_document;
    }

    void setLengthValue(CSSStyleValuePair* pair, const char* value);

    String* color();
    void setColor(String* color);

    String* direction();
    String* height();
    String* marginTop();
    void setMarginTop(const char* value);
    String* marginBottom();
    void setMarginBottom(const char* value);
    String* marginLeft();
    void setMarginLeft(const char* value);
    String* marginRight();
    void setMarginRight(const char* value);
    String* margin();
    void setMargin(const char* value);
    String* overflow();
    String* position();
    String* textDecoration();
    String* width();
    String* visibility();
    String* opacity();
    String* textAlign();

protected:
    std::vector<CSSStyleValuePair, gc_allocator<CSSStyleValuePair>> m_cssValues;
    Document* m_document;
};

class CSSStyleRule : public EventTarget<ScriptWrappable> {
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

    CSSStyleRule(Kind kind, String* ruleText, PseudoClass pc, Document* document)
        : m_document(document)
    {
        m_kind = kind;
        m_ruleText = ruleText;
        m_pseudoClass = pc;
        initScriptWrappable(this);
        m_styleDeclaration = new CSSStyleDeclaration(document);
    }

    CSSStyleDeclaration* styleDeclaration()
    {
        return m_styleDeclaration;
    }

    Document* document()
    {
        return m_document;
    }

protected:
    Kind m_kind;
    String* m_ruleText;
    PseudoClass m_pseudoClass;
    CSSStyleDeclaration* m_styleDeclaration;
    Document* m_document;
};

class CSSStyleSheet : public gc {
    friend class StyleResolver;
public:
    void addRule(CSSStyleRule rule)
    {
        m_rules.push_back(rule);
    }
protected:
    std::vector<CSSStyleRule> m_rules;
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
