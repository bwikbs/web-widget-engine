#ifndef __StarFishComputedStyle__
#define __StarFishComputedStyle__

#include "style/Style.h"

namespace StarFish {

class ComputedStyle : public gc {
    friend class StyleResolver;
public:
    ComputedStyle()
    {
        m_font = nullptr;

        m_inheritedStyles.m_color = Color(0, 0, 0, 255);
        m_inheritedStyles.m_fontSize = 10;

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

    Color bgColor()
    {
        return m_bgColor;
    }

    Font* font()
    {
        if (m_font == nullptr) {
            m_font = FontSelector::loadFont(String::emptyString, m_inheritedStyles.m_fontSize);
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

protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
        m_borderImageRepeat = new AxisValue<BorderImageRepeatValue>(BorderImageRepeatValue::StretchValue, BorderImageRepeatValue::StretchValue);
        m_marginBottom = Length(Length::Fixed, 0);
        m_opacity = 1;
    }

    struct {
        Color m_color;
        float m_fontSize;
        TextAlignValue m_textAlign;
    } m_inheritedStyles;

    DisplayValue m_display;
    Length m_width;
    Length m_height;
    Length m_bottom;

    Font* m_font;
    Color m_bgColor;
    SizeValue* m_bgSize;

    BackgroundRepeatValue m_backgroundRepeatX;
    BackgroundRepeatValue m_backgroundRepeatY;

    AxisValue<BorderImageRepeatValue>* m_borderImageRepeat;
    String* m_borderImageSource;

    Length m_marginBottom;
    float m_opacity;
};

}

#endif
