#ifndef __StarFishComputedStyle__
#define __StarFishComputedStyle__

#include "style/Style.h"
#include "style/StyleBackgroundData.h"
#include "style/StyleSurroundData.h"
#include "style/DefaultStyle.h"

namespace StarFish {

enum ComputedStyleDamage {
    ComputedStyleDamageNone,
    ComputedStyleDamageInherited,
};

class ComputedStyle : public gc {
    friend class StyleResolver;
    friend void resolveDOMStyleInner(StyleResolver* resolver, Element* element, ComputedStyle* parentStyle, bool force);
    friend ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle);
public:
    ComputedStyle()
    {
        m_font = nullptr;

        m_inheritedStyles.m_color = Color(0, 0, 0, 255);
        m_inheritedStyles.m_fontSize = Length(Length::Fixed, DEFAULT_FONT_SIZE);

        m_inheritedStyles.m_visibility = VisibilityValue::VisibleVisibilityValue;
        m_inheritedStyles.m_letterSpacing = Length(Length::Fixed, 0);
        m_inheritedStyles.m_lineHeight = Length(Length::Percent, -100);

        initNonInheritedStyles();
    }

    ComputedStyle(ComputedStyle* from)
    {
        m_font = nullptr;

        m_inheritedStyles = from->m_inheritedStyles;
        initNonInheritedStyles();
    }

    DisplayValue display()
    {
        return m_display;
    }

    void setDisplay(DisplayValue v)
    {
        m_display = v;
    }

    PositionValue position()
    {
        return m_position;
    }

    Length width()
    {
        return m_width;
    }

    void setWidth(const Length& l)
    {
        m_width = l;
    }

    Length height()
    {
        return m_height;
    }

    void setHeight(const Length& l)
    {
        m_height = l;
    }

    void setColor(Color r)
    {
        m_inheritedStyles.m_color = r;
    }

    Color color()
    {
        return m_inheritedStyles.m_color;
    }

    VerticalAlignValue verticalAlign()
    {
        return m_verticalAlign;
    }

    void setVerticalAlign(VerticalAlignValue v)
    {
        m_verticalAlign = v;
    }

    Length verticalAlignLength()
    {
        return m_verticalAlignLength;
    }

    void setVerticalAlignLength(Length l)
    {
        setVerticalAlign(VerticalAlignValue::NumericVAlignValue);
        m_verticalAlignLength = l;
    }

    bool isNumericVerticalAlign()
    {
        return (verticalAlign() == VerticalAlignValue::NumericVAlignValue);
    }

    TextAlignValue textAlign()
    {
        if (m_inheritedStyles.m_textAlign == TextAlignValue::NamelessTextAlignValue) {
            if (m_inheritedStyles.m_direction == DirectionValue::RtlDirectionValue)
                return TextAlignValue::RightTextAlignValue;
            return TextAlignValue::LeftTextAlignValue;
        }
        return m_inheritedStyles.m_textAlign;
    }

    void setTextAlign(TextAlignValue t)
    {
        m_inheritedStyles.m_textAlign = t;
    }

    TextDecorationValue textDecoration()
    {
        return m_textDecoration;
    }

    void setTextDecoration(TextDecorationValue decoration)
    {
        m_textDecoration = decoration;
    }

    DirectionValue direction()
    {
        return m_inheritedStyles.m_direction;
    }

    void setLineHeight(Length length)
    {
        m_inheritedStyles.m_lineHeight = length;
    }

    void setBackgroundIfNeeded()
    {
        if (m_background == NULL) {
            m_background = new StyleBackgroundData();
        }
    }

    void setBgColor(Color color)
    {
        setBackgroundIfNeeded();
        m_background->setBgColor(color);
    }

    void setBgImage(String* img)
    {
        setBackgroundIfNeeded();
        m_background->setBgImage(img);
    }

    void setBgImageData(ImageData* imgdata)
    {
        setBackgroundIfNeeded();
        m_background->setBgImageData(imgdata);
    }

    void setBackgroundRepeatX(BackgroundRepeatValue repeat)
    {
        setBackgroundIfNeeded();
        m_background->setRepeatX(repeat);
    }

    void setBackgroundRepeatY(BackgroundRepeatValue repeat)
    {
        setBackgroundIfNeeded();
        m_background->setRepeatY(repeat);
    }

    void setBackgroundSizeType(BackgroundSizeType type)
    {
        setBackgroundIfNeeded();
        m_background->setSizeType(type);
    }

    void setBackgroundSizeValue(LengthSize* size)
    {
        setBackgroundIfNeeded();
        m_background->setSizeValue(size);
    }

    Color bgColor()
    {
        if (m_background == NULL)
            return Color();
        return m_background->bgColor();
    }

    String* bgImage()
    {
        if (m_background == NULL)
            return String::emptyString;
        return m_background->bgImage();
    }

    ImageData* bgImageData()
    {
        if (m_background == NULL)
            return NULL;
        return m_background->bgImageData();
    }

    BackgroundRepeatValue backgroundRepeatX()
    {
        if (m_background == NULL)
            return BackgroundRepeatValue::RepeatRepeatValue;
        return m_background->repeatX();
    }

    BackgroundRepeatValue backgroundRepeatY()
    {
        if (m_background == NULL)
            return BackgroundRepeatValue::RepeatRepeatValue;
        return m_background->repeatY();
    }

    BackgroundSizeType bgSizeType()
    {
        if (m_background == NULL)
            return BackgroundSizeType::SizeValue;
        return m_background->sizeType();
    }

    LengthSize* bgSizeValue()
    {
        if (m_background == NULL)
            return new LengthSize();

        STARFISH_ASSERT(m_background && m_background->sizeType() == BackgroundSizeType::SizeValue);
        return m_background->sizeValue();
    }

    Font* font()
    {
        STARFISH_ASSERT(m_font);
        return m_font;
    }

    Length lineHeight()
    {
        // -100% is used to represent "normal" line height.
        return m_inheritedStyles.m_lineHeight;
    }

    float opacity()
    {
        return m_opacity;
    }

    int32_t zIndex()
    {
        return m_zIndex;
    }

    StyleBackgroundData* background()
    {
        return m_background;
    }

    bool hasBorderColor()
    {
        if (m_surround == nullptr) {
            return false;
        } else {
            return m_surround->border.hasBorderColor();
        }
    }

    bool hasBorderStyle()
    {
        if (m_surround == nullptr) {
            return false;
        } else {
            return m_surround->border.hasBorderStyle();
        }
    }

    Color borderTopColor()
    {
        if (m_surround == nullptr) {
            return m_inheritedStyles.m_color;
        } else {
            return m_surround->border.top().color();
        }
    }

    Color borderRightColor()
    {
        if (m_surround == nullptr) {
            return m_inheritedStyles.m_color;
        } else {
            return m_surround->border.right().color();
        }
    }

    Color borderBottomColor()
    {
        if (m_surround == nullptr) {
            return m_inheritedStyles.m_color;
        } else {
            return m_surround->border.bottom().color();
        }
    }

    Color borderLeftColor()
    {
        if (m_surround == nullptr) {
            return m_inheritedStyles.m_color;
        } else {
            return m_surround->border.left().color();
        }
    }

    BorderStyleValue borderTopStyle()
    {
        if (m_surround == nullptr) {
            return BorderStyleValue::BNone;
        } else {
            return m_surround->border.top().style();
        }
    }

    BorderStyleValue borderRightStyle()
    {
        if (m_surround == nullptr) {
            return BorderStyleValue::BNone;
        } else {
            return m_surround->border.right().style();
        }
    }

    BorderStyleValue borderBottomStyle()
    {
        if (m_surround == nullptr) {
            return BorderStyleValue::BNone;
        } else {
            return m_surround->border.bottom().style();
        }
    }

    BorderStyleValue borderLeftStyle()
    {
        if (m_surround == nullptr) {
            return BorderStyleValue::BNone;
        } else {
            return m_surround->border.left().style();
        }
    }

    Length borderTopWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.top().width();
        }
    }

    Length borderRightWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.right().width();
        }
    }

    Length borderBottomWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.bottom().width();
        }
    }

    Length borderLeftWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.left().width();
        }
    }

    void setBorderTopColor(Color color)
    {
        surround()->border.top().setColor(color);
    }

    void setBorderRightColor(Color color)
    {
        surround()->border.right().setColor(color);
    }

    void setBorderBottomColor(Color color)
    {
        surround()->border.bottom().setColor(color);
    }

    void setBorderLeftColor(Color color)
    {
        surround()->border.left().setColor(color);
    }

    void setBorderTopStyle(BorderStyleValue style)
    {
        surround()->border.top().setStyle(style);
    }

    void setBorderRightStyle(BorderStyleValue style)
    {
        surround()->border.right().setStyle(style);
    }

    void setBorderBottomStyle(BorderStyleValue style)
    {
        surround()->border.bottom().setStyle(style);
    }

    void setBorderLeftStyle(BorderStyleValue style)
    {
        surround()->border.left().setStyle(style);
    }

    void setBorderTopWidth(Length width)
    {
        surround()->border.top().setWidth(width);
    }

    void setBorderRightWidth(Length width)
    {
        surround()->border.right().setWidth(width);
    }

    void setBorderBottomWidth(Length width)
    {
        surround()->border.bottom().setWidth(width);
    }

    void setBorderLeftWidth(Length width)
    {
        surround()->border.left().setWidth(width);
    }

    String* borderImageSource() { return surround()->border.image().url(); }
    LengthBox borderImageSlices() { return surround()->border.image().slices(); }
    bool borderImageSliceFill() { return surround()->border.image().sliceFill(); }
    BorderImageRepeatValue borderImageRepeatX() { return surround()->border.image().repeatX(); }
    BorderImageRepeatValue borderImageRepeatY() { return surround()->border.image().repeatY(); }
    BorderImageLengthBox borderImageWidths() { return surround()->border.image().widths(); }

    void setBorderImageSource(String* url) { surround()->border.image().setUrl(url); }
    void setBorderImageSlices(LengthBox slices) { surround()->border.image().setSlices(slices); }
    void setBorderImageSliceFill(bool fill) { surround()->border.image().setSliceFill(fill); }
    void setBorderImageRepeatX(BorderImageRepeatValue value) { surround()->border.image().setRepeatX(value); }
    void setBorderImageRepeatY(BorderImageRepeatValue value) { surround()->border.image().setRepeatY(value); }
    void setBorderImageWidths(BorderImageLengthBox value) { surround()->border.image().setWidths(value); }

    void setBorderImageSliceFromOther(ComputedStyle* other)
    {
        setBorderImageSlices(other->borderImageSlices());
        setBorderImageSliceFill(other->borderImageSliceFill());
    }
    void setBorderImageRepeatFromOther(ComputedStyle* other)
    {
        setBorderImageRepeatX(other->borderImageRepeatX());
        setBorderImageRepeatY(other->borderImageRepeatY());
    }

    StyleSurroundData* surround() {
        if (m_surround == nullptr) {
            m_surround = new StyleSurroundData();
            // FIXME: need to cleanup
            BorderValue();
        }
        return m_surround;
    }

    OverflowValue overflow() {
        return overflowX();
    }

    OverflowValue overflowX() {
        return m_overflowX;
    }

/*    OverflowValue overflowY() {
        return m_overflowY;
    }
*/
    void setTop(Length top)
    {
        surround()->offset.setTop(top);
    }

    void setRight(Length right)
    {
        surround()->offset.setRight(right);
    }

    void setBottom(Length bottom)
    {
        surround()->offset.setBottom(bottom);
    }

    void setLeft(Length left)
    {
        surround()->offset.setLeft(left);
    }

    void setMarginTop(Length top)
    {
        surround()->margin.setTop(top);
    }

    void setMarginBottom(Length bottom)
    {
        surround()->margin.setBottom(bottom);
    }

    void setMarginRight(Length right)
    {
        surround()->margin.setRight(right);
    }

    void setMarginLeft(Length left)
    {
        surround()->margin.setLeft(left);
    }

    void setPaddingTop(Length top)
    {
        surround()->padding.setTop(top);
    }

    void setPaddingRight(Length right)
    {
        surround()->padding.setRight(right);
    }

    void setPaddingBottom(Length bottom)
    {
        surround()->padding.setBottom(bottom);
    }

    void setPaddingLeft(Length left)
    {
        surround()->padding.setLeft(left);
    }

    Length top()
    {
        if (m_surround == nullptr) {
            return Length();
        } else {
            return m_surround->offset.top();
        }
    }

    Length right()
    {
        if (m_surround == nullptr) {
            return Length();
        } else {
            return m_surround->offset.right();
        }
    }

    Length bottom()
    {
        if (m_surround == nullptr) {
            return Length();
        } else {
            return m_surround->offset.bottom();
        }
    }

    Length left()
    {
        if (m_surround == nullptr) {
            return Length();
        } else {
            return m_surround->offset.left();
        }
    }

    Length marginTop()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->margin.top();
        }
    }

    Length marginRight()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->margin.right();
        }
    }

    Length marginBottom()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->margin.bottom();
        }
    }

    Length marginLeft()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->margin.left();
        }
    }

    Length paddingTop()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->padding.top();
        }
    }

    Length paddingRight()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->padding.right();
        }
    }

    Length paddingBottom()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->padding.bottom();
        }
    }

    Length paddingLeft()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->padding.left();
        }
    }

    Length fontSize()
    {
        return m_inheritedStyles.m_fontSize;
    }

    void setTextOverflow(TextOverflowValue val)
    {
        m_textOverflow = val;
    }

    void setLetterSpacing(Length len)
    {
        m_inheritedStyles.m_letterSpacing = len;
    }

    TextOverflowValue textOverflow()
    {
        return m_textOverflow;
    }

    VisibilityValue visibility()
    {
        return m_inheritedStyles.m_visibility;
    }

    FontStyleValue fontStyle()
    {
        return m_inheritedStyles.m_fontStyle;
    }

    Length letterSpacing()
    {
        return m_inheritedStyles.m_letterSpacing;
    }

    static VerticalAlignValue initialVerticalAlign() { return VerticalAlignValue::BaselineVAlignValue; }
    static TextAlignValue initialTextAlign() { return TextAlignValue::NamelessTextAlignValue; }
    static Length initialPadding() { return Length(Length::Fixed, 0); }
    static Length initialMargin() { return Length(Length::Fixed, 0); }
    static String* initialBgImage() { return String::emptyString; }
    static String* initialBorderImageSource() { return String::emptyString; }
    static LengthBox initialBorderImageSlices() { return LengthBox(Length(Length::Percent, 1.f), Length(Length::Percent, 1.f), Length(Length::Percent, 1.f), Length(Length::Percent, 1.f)); }
    static bool initialBorderImageSliceFill() { return false; }
    static BorderImageRepeatValue initialBorderImageRepeat() { return BorderImageRepeatValue::StretchValue; }

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_opacity = 1;
        m_zIndex = 0;
        m_background = NULL;
        m_overflowX = OverflowValue::VisibleOverflow;
//        m_overflowY = OverflowValue::VisibleOverflow;
        m_textDecoration = TextDecorationValue::NoneTextDecorationValue;
        m_verticalAlign = initialVerticalAlign();
    }

    void arrangeStyleValues(ComputedStyle* parentStyle)
    {
        // 9.7 Relationships between 'display', 'position', and 'float'
        if (position() == AbsolutePositionValue || position() == FixedPositionValue) {
            m_display = DisplayValue::BlockDisplayValue;
        }

        if (lineHeight().isPercent()) {
            if (lineHeight().percent() == -100) {
                // FIXME: The computed value of initial and normal should be normal.
                setLineHeight(Length(Length::EmToBeFixed, 1.2));
            } else {
                // The computed value of the property is this percentage multiplied by the element's computed font size. Negative values are illegal.
                setLineHeight(Length(Length::Fixed, lineHeight().percent() * fontSize().fixed()));
            }
        }

        // Convert all non-computed Lengths to computed Length
        STARFISH_ASSERT(m_inheritedStyles.m_fontSize.isFixed());
        Length baseFontSize = fontSize();
        m_inheritedStyles.m_letterSpacing.changeToFixedIfNeeded(baseFontSize, font());
        m_inheritedStyles.m_lineHeight.changeToFixedIfNeeded(baseFontSize, font());
        m_width.changeToFixedIfNeeded(baseFontSize, font());
        m_height.changeToFixedIfNeeded(baseFontSize, font());
        m_verticalAlignLength.changeToFixedIfNeeded(baseFontSize, font());

        if (m_surround) {
            m_surround->margin.top().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->margin.right().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->margin.bottom().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->margin.left().changeToFixedIfNeeded(baseFontSize, font());

            m_surround->padding.top().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->padding.right().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->padding.bottom().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->padding.left().changeToFixedIfNeeded(baseFontSize, font());

            m_surround->offset.top().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->offset.right().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->offset.bottom().changeToFixedIfNeeded(baseFontSize, font());
            m_surround->offset.left().changeToFixedIfNeeded(baseFontSize, font());

            m_surround->border.checkComputed(baseFontSize, font());

            if (hasBorderStyle() && !hasBorderColor()) {
                // If an element's border color is not specified with a border property,
                // user agents must use the value of the element's 'color' property as the computed value for the border color.
                setBorderTopColor(m_inheritedStyles.m_color);
                setBorderRightColor(m_inheritedStyles.m_color);
                setBorderBottomColor(m_inheritedStyles.m_color);
                setBorderLeftColor(m_inheritedStyles.m_color);
            }
        }

        if(m_background)
            m_background->checkComputed(baseFontSize, font());

        // [vertical-align] : Update percentage value referring to the [line-height]
        if (isNumericVerticalAlign() && verticalAlignLength().isPercent()) {
            m_verticalAlignLength.percentToFixed(lineHeight().fixed());
        }
    }

    void loadResources(StarFish* sf);

    struct InheritedStyles {
        Color m_color;
        Length m_fontSize;
        FontStyleValue m_fontStyle;
        Length m_letterSpacing;
        Length m_lineHeight;
        TextAlignValue m_textAlign;
        DirectionValue m_direction;
        VisibilityValue m_visibility;
    } m_inheritedStyles;

    DisplayValue m_display;
    PositionValue m_position;
    TextOverflowValue m_textOverflow;
    Length m_width;
    Length m_height;
    VerticalAlignValue m_verticalAlign;
    Length m_verticalAlignLength;

    OverflowValue m_overflowX;
//    OverflowValue m_overflowY;

    float m_opacity;
    int32_t m_zIndex;

protected:
    Font* m_font;
    StyleBackgroundData* m_background;
    StyleSurroundData* m_surround;
    TextDecorationValue m_textDecoration;
};

ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle);

}

#endif
