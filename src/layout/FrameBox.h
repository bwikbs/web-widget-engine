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

    void setContentWidth(float width) { m_frameRect.setWidth(width + paddingWidth() + borderWidth()); }
    void setContentHeight(float height) { m_frameRect.setHeight(height + paddingHeight() + borderHeight()); }

    void setPaddingTop(float t) { m_padding.setTop(t); }
    void setPaddingRight(float t) { m_padding.setRight(t); }
    void setPaddingBottom(float t) { m_padding.setBottom(t); }
    void setPaddingLeft(float t) { m_padding.setLeft(t); }

    float paddingTop() { return m_padding.top(); }
    float paddingRight() { return m_padding.right(); }
    float paddingBottom() { return m_padding.bottom(); }
    float paddingLeft() { return m_padding.left(); }

    void setBorderTop(float t) { m_border.setTop(t); }
    void setBorderRight(float t) { m_border.setRight(t); }
    void setBorderBottom(float t) { m_border.setBottom(t); }
    void setBorderLeft(float t) { m_border.setLeft(t); }

    float borderTop() { return m_border.top(); }
    float borderRight() { return m_border.right(); }
    float borderBottom() { return m_border.bottom(); }
    float borderLeft() { return m_border.left(); }

    void setMarginTop(float t) { m_margin.setTop(t); }
    void setMarginRight(float t) { m_margin.setRight(t); }
    void setMarginBottom(float t) { m_margin.setBottom(t); }
    void setMarginLeft(float t) { m_margin.setLeft(t); }

    float marginTop() { return m_margin.top(); }
    float marginRight() { return m_margin.right(); }
    float marginBottom() { return m_margin.bottom(); }
    float marginLeft() { return m_margin.left(); }

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
        Rect bgRect(borderLeft(), borderTop(), m_frameRect.width() - borderWidth(), m_frameRect.height() - borderHeight());
        if (!style()->bgColor().isTransparent()) {
            canvas->save();
            canvas->setColor(style()->bgColor());
            canvas->drawRect(bgRect);
            canvas->restore();
        }
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
