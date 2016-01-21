#ifndef __StarFishBorderData__
#define __StarFishBorderData__

#include "style/BorderValue.h"

namespace StarFish {

class BorderData {
public:
    BorderData()
    {
    }

    Length topWidth()
    {
        return m_top.width();
    }

    Length rightWidth()
    {
        return m_right.width();
    }

    Length bottomWidth()
    {
        return m_bottom.width();
    }

    Length leftWidth()
    {
        return m_left.width();
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

    Length imageOffsetTop()
    {
        return m_imageOffsetTop;
    }

    Length imageOffsetRight()
    {
        return m_imageOffsetRight;
    }

    Length imageOffsetBottom()
    {
        return m_imageOffsetBottom;
    }

    Length imageOffsetLeft()
    {
        return m_imageOffsetLeft;
    }

    void setImageOffsetTop(Length top)
    {
        m_imageOffsetTop = top;
    }

    void setImageOffsetRight(Length right)
    {
        m_imageOffsetRight = right;
    }

    void setImageOffsetBottom(Length bottom)
    {
        m_imageOffsetBottom = bottom;
    }

    void setImageOffsetLeft(Length left)
    {
        m_imageOffsetLeft = left;
    }

    bool imageFill()
    {
        return m_imageFill;
    }

    void setImageFill(bool fill)
    {
        m_imageFill = fill;
    }

    void borderImageSliceInherit(BorderData& parent)
    {
        m_imageOffsetTop = parent.imageOffsetTop();
        m_imageOffsetRight = parent.imageOffsetRight();
        m_imageOffsetBottom = parent.imageOffsetBottom();
        m_imageOffsetLeft = parent.imageOffsetLeft();
        m_imageFill = parent.imageFill();
    }

    void borderImageSliceInitialize() {
        m_imageOffsetTop = Length(Length::Percent, 1);
        m_imageOffsetRight = Length(Length::Percent, 1);
        m_imageOffsetBottom = Length(Length::Percent, 1);
        m_imageOffsetLeft = Length(Length::Percent, 1);
        m_imageFill = false;
    }

private:
    BorderValue m_top;
    BorderValue m_right;
    BorderValue m_bottom;
    BorderValue m_left;

    // border-image-slice
    Length m_imageOffsetTop;
    Length m_imageOffsetRight;
    Length m_imageOffsetBottom;
    Length m_imageOffsetLeft;

    bool m_imageFill;
};

} /* namespace StarFish */

#endif /* __StarFishBorderData__ */

