#ifndef __StarFishFrameBox__
#define __StarFishFrameBox__

#include "layout/Frame.h"

namespace StarFish {

class FrameBox : public Frame {
public:
    FrameBox(Node* node, ComputedStyle* style)
        : Frame(node, style)
        , m_frameRect(0, 0, 0, 0)
    {

    }

    float x() const { return m_frameRect.x(); }
    float y() const { return m_frameRect.y(); }
    float width() const { return m_frameRect.width(); }
    float height() const { return m_frameRect.height(); }

    void setX(float x) { m_frameRect.setX(x); }
    void setY(float y) { m_frameRect.setY(y); }
    void setWidth(float width) { m_frameRect.setWidth(width); }
    void setHeight(float height) { m_frameRect.setHeight(height); }

protected:
    // content + padding + border
    Rect m_frameRect;
    BoxSurroundData m_margin;

};

}

#endif
