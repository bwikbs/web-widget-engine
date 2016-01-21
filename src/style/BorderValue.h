#ifndef __StarFishBorderValue__
#define __StarFishBorderValue__

#include "style/Style.h"

namespace StarFish {

class BorderValue {
public:
    BorderValue()
        : m_width(Length(Length::Fixed, 3))
    {
    }

    Length width()
    {
        return m_width;
    }

    void setWidth(Length length)
    {
        m_width = length;
    }

protected:
    Length m_width;
};

} /* namespace StarFish */

#endif /* __StarFishBorderValue__ */

