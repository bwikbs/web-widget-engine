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

private:
    Length m_top;
    Length m_right;
    Length m_bottom;
    Length m_left;

    // TODO: Add background image and more styles (color, etc.)
};

} /* namespace StarFish */

#endif /* __StarFishBorderData__ */
