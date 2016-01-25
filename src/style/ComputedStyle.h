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
        return surround()->margin.setTop(top);
    }

    void setMarginBottom(Length r)
    {
        m_marginBottom = r;
    }

    void setMarginLeft(Length r)
    {
        m_marginLeft = r;
    }

    void setMarginRight(Length r)
    {
        m_marginRight = r;
    }

    void setPaddingTop(Length r)
    {
        m_paddingTop = r;
    }

    void setPaddingBottom(Length r)
    {
        m_paddingBottom = r;
    }

    Length marginTop()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->margin.top();
        }
    }

    Length marginBottom()
    {
        return m_marginBottom;
    }

    Length marginLeft() {
        return m_marginLeft;
    }

    Length marginRight()
    {
        return m_marginRight;
    }

    Length paddingTop()
    {
        return m_paddingTop;
    }

    Length paddingBottom()
    {
        return m_paddingBottom;
    }

    float fontSize()
    {
        return m_inheritedStyles.m_fontSize;
    }

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_marginBottom = Length(Length::Fixed, 0);
        m_marginLeft = Length(Length::Fixed, 0);
        m_marginRight = Length(Length::Fixed, 0);
        m_paddingBottom = Length(Length::Fixed, 0);
        m_opacity = 1;
        m_background = NULL;
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
        m_marginBottom.changeToFixedIfNeeded(fontSize(), font());
        m_marginLeft.changeToFixedIfNeeded(fontSize(), font());
        m_marginRight.changeToFixedIfNeeded(fontSize(), font());
        m_paddingBottom.changeToFixedIfNeeded(fontSize(), font());

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

protected:
    Font* m_font;
    StyleBackgroundData* m_background;

    StyleSurroundData* m_surround;

    //TODO: margin/padding data should be moved to StyleSurroundData
    Length m_marginBottom;
    Length m_marginLeft;
    Length m_marginRight;
    Length m_paddingBottom;
    Length m_paddingTop;
};

ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle);

}

#endif
