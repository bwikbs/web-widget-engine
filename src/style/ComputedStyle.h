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

    Color color()
    {
        return m_inheritedStyles.m_color;
    }

    TextAlignValue textAlign()
    {
        return m_inheritedStyles.m_textAlign;
    }

    Font* font()
    {
        if (m_font == nullptr) {
            m_font = FontSelector::loadFont(String::emptyString, m_inheritedStyles.m_fontSize);
        }
        return m_font;
    }
protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
    }

    struct {
        Color m_color;
        float m_fontSize;
        TextAlignValue m_textAlign;
    } m_inheritedStyles;

    DisplayValue m_display;
    Length m_width;
    Length m_height;

    Font* m_font;
    Color m_bgColor;
    SizeValue* m_bgSize;
};

}

#endif
