#ifndef __StarFishBorderValue__
#define __StarFishBorderValue__

#include "style/Style.h"

namespace StarFish {

class BorderValue {
public:
    BorderValue()
        : m_style(BorderStyleValue::BNone)
        , m_width(Length(Length::Fixed, 3))
        , m_color(Color(0, 0, 0, 255))
        , m_hasBorderColor(false)
    {
    }

    BorderStyleValue style()
    {
        return m_style;
    }

    Length width()
    {
        if (style() == BorderStyleValue::BNone)
            return Length(Length::Fixed, 0);
        return m_width;
    }

    Color color()
    {
        return m_color;
    }

    void setStyle(BorderStyleValue style)
    {
        m_style = style;
    }

    void setWidth(Length length)
    {
        m_width = length;
    }

    void setColor(Color color)
    {
        m_color = color;
        m_hasBorderColor = true;
    }

    bool hasBorderColor()
    {
        return m_hasBorderColor;
    }

    bool hasBorderStyle()
    {
        return style() != BorderStyleValue::BNone;
    }

    void checkComputed(Length fontSize, Font* font)
    {
        m_width.changeToFixedIfNeeded(fontSize, font);
    }

protected:
    BorderStyleValue m_style;
    Length m_width;
    Color m_color;
    bool m_hasBorderColor;
};

} /* namespace StarFish */

#endif /* __StarFishBorderValue__ */
