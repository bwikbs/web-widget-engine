#ifndef __StarFishComputedStyle__
#define __StarFishComputedStyle__

#include "style/Style.h"
#include "style/StyleBackgroundData.h"
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
        return m_background->m_imageData;
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

    String* borderImageSource()
    {
        return m_borderImageSource;
    }

    Length marginBottom()
    {
        return m_marginBottom;
    }

    Length marginLeft() {
        return m_marginLeft;
    }

    float opacity()
    {
        return m_opacity;
    }

    StyleBackgroundData* background()
    {
        return m_background;
    }

    Length borderTopWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.topWidth();
        }
    }

    Length borderRightWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.rightWidth();
        }
    }

    Length borderBottomWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.bottomWidth();
        }
    }

    Length borderLeftWidth()
    {
        if (m_surround == nullptr) {
            return Length(Length::Fixed, 0);
        } else {
            return m_surround->border.leftWidth();
        }
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

    Length borderImageOffsetTop()
    {
        if (m_surround == nullptr) {
            return Length(Length::Percent, 1);
        } else {
            return m_surround->border.imageOffsetTop();
        }
    }

    Length borderImageOffsetRight()
    {
        if (m_surround == nullptr) {
            return Length(Length::Percent, 1);
        } else {
            return m_surround->border.imageOffsetRight();
        }
    }

    Length borderImageOffsetBottom()
    {
        if (m_surround == nullptr) {
            return Length(Length::Percent, 1);
        } else {
            return m_surround->border.imageOffsetBottom();
        }
    }

    Length borderImageOffsetLeft()
    {
        if (m_surround == nullptr) {
            return Length(Length::Percent, 1);
        } else {
            return m_surround->border.imageOffsetLeft();
        }
    }

    bool borderImageFill()
    {
        if (m_surround == nullptr) {
            return false;
        } else {
            return m_surround->border.imageFill();
        }
    }

    BorderImageRepeatValue borderImageRepeatX()
    {
        if (m_surround == nullptr) {
            return BorderImageRepeatValue::StretchValue;
        } else {
            return m_surround->border.imageRepeatX();
        }
    }

    BorderImageRepeatValue borderImageRepeatY()
    {
        if (m_surround == nullptr) {
            return BorderImageRepeatValue::StretchValue;
        } else {
            return m_surround->border.imageRepeatY();
        }
    }

    StyleSurroundData* surround() {
        if (m_surround == nullptr) {
            m_surround = new StyleSurroundData();
            m_surround->border.borderImageRepeatInitialize();
            m_surround->border.borderImageSliceInitialize();
        }
        return m_surround;
    }

    OverflowValue overflowX() {
        return m_overflowX;
    }

    OverflowValue overflowY() {
        return m_overflowY;
    }

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_marginBottom = Length(Length::Fixed, 0);
        m_marginLeft = Length(Length::Fixed, 0);
        m_opacity = 1;
        m_background = NULL;
    }

    void loadResources(StarFish* sf);

    struct InheritedStyles {
        Color m_color;
        float m_fontSize;
        TextAlignValue m_textAlign;
        DirectionValue m_direction;
    } m_inheritedStyles;

    DisplayValue m_display;
    Length m_width;
    Length m_height;
    Length m_bottom;
    Length m_left;

    Font* m_font;
    StyleBackgroundData* m_background;

    String* m_borderImageSource;
    OverflowValue m_overflowX;
    OverflowValue m_overflowY;

    Length m_marginBottom;
    Length m_marginLeft;
    float m_opacity;

    StyleSurroundData* m_surround;
};

ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle);

}

#endif
