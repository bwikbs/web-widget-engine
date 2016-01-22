#ifndef __StarFishBorderData__
#define __StarFishBorderData__

#include "style/BorderValue.h"
#include "style/BorderImage.h"

namespace StarFish {

class BorderData {
public:
    BorderData()
    {
    }

    BorderValue& top()
    {
        return m_top;
    }

    BorderValue& right()
    {
        return m_right;
    }

    BorderValue& bottom()
    {
        return m_bottom;
    }

    BorderValue& left()
    {
        return m_left;
    }

    BorderImage& image() { return m_image; }

private:
    BorderValue m_top;
    BorderValue m_right;
    BorderValue m_bottom;
    BorderValue m_left;

    BorderImage m_image;
};

} /* namespace StarFish */

#endif /* __StarFishBorderData__ */

