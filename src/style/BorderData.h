#ifndef __StarFishBorderData__
#define __StarFishBorderData__

namespace StarFish {

class BorderData {
public:
    BorderData()
    {

    }

    BorderData(Length top, Length right, Length bottom, Length left)
    {
        m_top = top;
        m_right = right;
        m_bottom = bottom;
        m_left = left;
    }

    Length top()
    {
        return m_top;
    }

    Length right()
    {
        return m_right;
    }

    Length bottom()
    {
        return m_bottom;
    }

    Length left()
    {
        return m_left;
    }

    void setTop(Length top)
    {
        m_top = top;
    }

    void setRight(Length right)
    {
        m_right = right;
    }

    void setBottom(Length bottom)
    {
        m_bottom = bottom;
    }

    void setLeft(Length left)
    {
        m_left = left;
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
    Length m_top;
    Length m_right;
    Length m_bottom;
    Length m_left;

    // border-image-slice
    Length m_imageOffsetTop;
    Length m_imageOffsetRight;
    Length m_imageOffsetBottom;
    Length m_imageOffsetLeft;

    bool m_imageFill;

    // TODO: Add background image and more styles (color, etc.)
};

} /* namespace StarFish */

#endif /* __StarFishBorderData__ */
