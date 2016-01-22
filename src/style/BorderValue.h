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
    {
    }

    BorderStyleValue style()
    {
        return m_style;
    }

    Length width()
    {
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
    }

protected:
    BorderStyleValue m_style;
    Length m_width;
    Color m_color;
};

} /* namespace StarFish */

#endif /* __StarFishBorderValue__ */
