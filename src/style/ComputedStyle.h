#ifndef __StarFishComputedStyle__
#define __StarFishComputedStyle__

#include "style/Style.h"

namespace StarFish {

class ComputedStyle : public gc {
    friend class StyleResolver;
public:
    ComputedStyle()
    {
        m_inheritedStyles.m_color = Color(0, 0, 0, 255);
        m_inheritedStyles.m_fontSize = 10;

        initNonInheritedStyles();
    }

    ComputedStyle(ComputedStyle* from)
    {
        m_inheritedStyles = from->m_inheritedStyles;
        initNonInheritedStyles();
    }
protected:
    void initNonInheritedStyles()
    {
        m_display = DisplayValue::InlineDisplayValue;
    }

    struct {
        Color m_color;
        float m_fontSize;
    } m_inheritedStyles;

    DisplayValue m_display;
    Length m_width;
    Length m_height;
};

}

#endif
