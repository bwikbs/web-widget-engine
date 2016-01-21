#ifndef __StarFishComputedStyle__
#define __StarFishComputedStyle__

#include "style/Style.h"
#include "style/StyleBackgroundData.h"

namespace StarFish {

class ComputedStyle : public gc {
    friend class StyleResolver;
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

    void setbgColor(Color color)
    {
        m_background->setbgColor(color);
    }

    Color bgColor()
    {
        return m_background->bgColor();
    }

    String* bgImage() {
        return m_bgImage;
    }

    Font* font()
    {
        STARFISH_ASSERT(m_font);
        return m_font;
    }

    void ensureFont(StarFish* sf)
    {
        if (m_font == nullptr) {
            m_font = fontSlowCase(sf);
        }
    }

    Font* font(StarFish* sf)
    {
        if (m_font == nullptr) {
            return fontSlowCase(sf);
        }
        return m_font;
    }

    BackgroundRepeatValue backgroundRepeatX() {
    	return m_backgroundRepeatX;
    }

    BackgroundRepeatValue backgroundRepeatY() {
        return m_backgroundRepeatY;
    }

    Length bottom() {
        return m_bottom;
    }

    Length left() {
        return m_left;
    }

    AxisValue<BorderImageRepeatValue>* borderImageRepeat() {
        return m_borderImageRepeat;
    }

    String* borderImageSource() {
        return m_borderImageSource;
    }

    Length marginBottom() {
        return m_marginBottom;
    }

    float opacity() {
        return m_opacity;
    }

    StyleBackgroundData* background() {
        return m_background;
    }

    StyleSurroundData* surround() {
        if (m_surround == nullptr) {
            m_surround = new StyleSurroundData();
        }
        return m_surround;
    }

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_borderImageRepeat = new AxisValue<BorderImageRepeatValue>(BorderImageRepeatValue::StretchValue, BorderImageRepeatValue::StretchValue);
        m_marginBottom = Length(Length::Fixed, 0);
        m_opacity = 1;
        m_bgImage = String::emptyString;
        m_background = new StyleBackgroundData();;
        m_surround = new StyleSurroundData();
    }

    Font* fontSlowCase(StarFish* sf);

    struct {
        Color m_color;
        float m_fontSize;
        TextAlignValue m_textAlign;
    } m_inheritedStyles;

    DisplayValue m_display;
    Length m_width;
    Length m_height;
    Length m_bottom;
    Length m_left;

    Font* m_font;
    String* m_bgImage;
    StyleBackgroundData* m_background;

    BackgroundRepeatValue m_backgroundRepeatX;
    BackgroundRepeatValue m_backgroundRepeatY;

    AxisValue<BorderImageRepeatValue>* m_borderImageRepeat;
    String* m_borderImageSource;

    Length m_marginBottom;
    float m_opacity;

    StyleSurroundData* m_surround;
};

}

#endif
