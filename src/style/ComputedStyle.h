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
        m_inheritedStyles.m_fontSize = DEFAULT_FONT_SIZE;

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

    PositionValue position()
    {
        return m_position;
    }

    Length width()
    {
        return m_width;
    }

    Length height()
    {
        return m_height;
    }

    void setColor(Color r)
    {
        m_inheritedStyles.m_color = r;
    }

    Color color()
    {
        return m_inheritedStyles.m_color;
    }

    TextAlignValue textAlign()
    {
        return m_inheritedStyles.m_textAlign;
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

    Length bottom()
    {
        return m_bottom;
    }

    Length left()
    {
        return m_left;
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

    void setBorderImageSource(String* url) { surround()->border.image().setUrl(url); }
    void setBorderImageSlices(LengthBox slices) { surround()->border.image().setSlices(slices); }
    void setBorderImageSliceFill(bool fill) { surround()->border.image().setSliceFill(fill); }
    void setBorderImageRepeatX(BorderImageRepeatValue value) { surround()->border.image().setRepeatX(value); }
    void setBorderImageRepeatY(BorderImageRepeatValue value) { surround()->border.image().setRepeatY(value); }

    void borderImageSliceInherit(ComputedStyle* parent)
    {
        setBorderImageSlices(parent->borderImageSlices());
        setBorderImageSliceFill(parent->borderImageSliceFill());
    }
    void borderImageRepeatInherit(ComputedStyle* parent)
    {
        setBorderImageRepeatX(parent->borderImageRepeatX());
        setBorderImageRepeatY(parent->borderImageRepeatY());
    }

    StyleSurroundData* surround() {
        if (m_surround == nullptr) {
            m_surround = new StyleSurroundData();
            // FIXME: need to cleanup
            BorderValue();
        }
        return m_surround;
    }

    OverflowValue overflowX() {
        return m_overflowX;
    }

    OverflowValue overflowY() {
        return m_overflowY;
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

    float fontSize()
    {
        return m_inheritedStyles.m_fontSize;
    }

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_opacity = 1;
        m_zIndex = 0;
        m_background = NULL;
        m_overflowX = OverflowValue::Visible;
        m_overflowY = OverflowValue::Visible;
        m_textDecoration = TextDecorationValue::None;
    }

    void arrangeStyleValues()
    {
        // if float: left, right
        // display-> block

        // https://developer.mozilla.org/en-US/docs/Web/CSS/float
        if (position() == AbsolutePositionValue || position() == FixedPositionValue) {
            m_display = DisplayValue::BlockDisplayValue;
        }

        m_width.changeToFixedIfNeeded(fontSize(), font());
        m_height.changeToFixedIfNeeded(fontSize(), font());
        m_bottom.changeToFixedIfNeeded(fontSize(), font());
        m_left.changeToFixedIfNeeded(fontSize(), font());

        if (m_surround) {
            m_surround->margin.top().changeToFixedIfNeeded(fontSize(), font());
            m_surround->margin.right().changeToFixedIfNeeded(fontSize(), font());
            m_surround->margin.bottom().changeToFixedIfNeeded(fontSize(), font());
            m_surround->margin.left().changeToFixedIfNeeded(fontSize(), font());

            m_surround->padding.top().changeToFixedIfNeeded(fontSize(), font());
            m_surround->padding.right().changeToFixedIfNeeded(fontSize(), font());
            m_surround->padding.bottom().changeToFixedIfNeeded(fontSize(), font());
            m_surround->padding.left().changeToFixedIfNeeded(fontSize(), font());

            if (hasBorderStyle() && !hasBorderColor()) {
                // If an element's border color is not specified with a border property,
                // user agents must use the value of the element's 'color' property as the computed value for the border color.
                setBorderTopColor(m_inheritedStyles.m_color);
                setBorderRightColor(m_inheritedStyles.m_color);
                setBorderBottomColor(m_inheritedStyles.m_color);
                setBorderLeftColor(m_inheritedStyles.m_color);

                setBorderTopWidth(Length(Length::Fixed, 3));
                setBorderRightWidth(Length(Length::Fixed, 3));
                setBorderBottomWidth(Length(Length::Fixed, 3));
                setBorderLeftWidth(Length(Length::Fixed, 3));
            }
        }

        if(m_background)
            m_background->checkComputed(fontSize(), font());

        //TODO: should check surround / ...
    }
    void loadResources(StarFish* sf);

    struct InheritedStyles {
        Color m_color;
        float m_fontSize;
        TextAlignValue m_textAlign;
        DirectionValue m_direction;
    } m_inheritedStyles;

    DisplayValue m_display;
    PositionValue m_position;
    Length m_width;
    Length m_height;
    Length m_bottom;
    Length m_left;

    OverflowValue m_overflowX;
    OverflowValue m_overflowY;

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
