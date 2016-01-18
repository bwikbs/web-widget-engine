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

    virtual bool isFrameBox()
    {
        return true;
    }

    virtual void dump()
    {
        Frame::dump();
        printf(" frameRect(%g,%g,%g,%g) ", x(), y(), width(), height());
    }


    float x() const { return m_frameRect.x(); }
    float y() const { return m_frameRect.y(); }
    float width() const { return m_frameRect.width(); }
    float height() const { return m_frameRect.height(); }

    void setX(float x) { m_frameRect.setX(x); }
    void setY(float y) { m_frameRect.setY(y); }
    void setWidth(float width) { m_frameRect.setWidth(width); }
    void setHeight(float height) { m_frameRect.setHeight(height); }

    float paddingWidth()
    {
        return m_padding.left() + m_padding.right();
    }

    float paddingHeight()
    {
        return m_padding.top() + m_padding.bottom();
    }

    float borderWidth()
    {
        return m_border.left() + m_border.right();
    }

    float borderHeight()
    {
        return m_border.top() + m_border.bottom();
    }

    float marginWidth()
    {
        return m_margin.left() + m_margin.right();
    }

    float marginHeight()
    {
        return m_margin.top() + m_margin.bottom();
    }

    float contentWidth()
    {
        return m_frameRect.width() - paddingWidth() - borderWidth();
    }

    float contentHeight()
    {
        return m_frameRect.height() - paddingHeight() - borderHeight();
    }

    void paintBackgroundAndBorders(Canvas* canvas)
    {
        // TODO draw background color
        // TODO draw background image
        // TODO draw border
    }

    virtual Frame* hitTest(float x, float y,HitTestStage stage)
    {
        if (x >= 0 && x < m_frameRect.width() && y >= 0 && y < m_frameRect.height()) {
            return this;
        }
        return nullptr;
    }
protected:
    // content + padding + border
    Rect m_frameRect;
    BoxSurroundData m_padding, m_border, m_margin;
};

}

#endif
