#ifndef __StarFishBorderValue__
#define __StarFishBorderValue__

#include "style/Style.h"

namespace StarFish {

class BorderValue {
public:
    BorderValue()
        : m_style(BorderStyleValue::BNone)
        , m_width(Length(Length::Fixed, 3))
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

    void setStyle(BorderStyleValue style)
    {
        m_style = style;
    }

    void setWidth(Length length)
    {
        m_width = length;
    }

protected:
    BorderStyleValue m_style;
    Length m_width;
};

} /* namespace StarFish */

#endif /* __StarFishBorderValue__ */

