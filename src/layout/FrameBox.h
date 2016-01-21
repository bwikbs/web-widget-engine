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
        if (!style()->bgColor().isTransparent()) {
            canvas->save();
            canvas->setColor(style()->bgColor());
            Rect bgRect(borderLeft(), borderTop(), m_frameRect.width() - borderWidth(), m_frameRect.height() - borderHeight());
            canvas->drawRect(bgRect);
            canvas->restore();
        }
        if (style()->bgImageData()) {
            ImageData* id = style()->bgImageData();
            if (!id->width() || !id->height())
                return;

            // TODO background-position
            canvas->save();
            canvas->translate(borderLeft(), borderTop());
            float bw = m_frameRect.width() - borderWidth();
            float bh = m_frameRect.height() - borderHeight();
            canvas->clip(Rect(0, 0, bw, bh));

            if (style()->bgSizeType() == BackgroundSizeType::Cover) {
                canvas->drawImage(id, Rect(0, 0, bw, bh));
            } else if (style()->bgSizeType() == BackgroundSizeType::Contain) {
                float boxR = bw/bh;
                float imgR = id->width()/(float)id->height();
                if (boxR > imgR) {
                    float start = bh * (float)id->width() / (float)id->height();
                    canvas->drawImage(id, Rect(0, 0, start, bh));

                    if (style()->backgroundRepeatX() == BackgroundRepeatValue::RepeatRepeatValue) {
                        for (float s = start; s < bw; s += start) {
                            canvas->drawImage(id, Rect(s, 0, start, bh));
                        }
                    }
                } else {
                    float start = bw * (float)id->height() / (float)id->width();
                    canvas->drawImage(id, Rect(0, 0, bw, start));

                    if (style()->backgroundRepeatY() == BackgroundRepeatValue::RepeatRepeatValue) {
                        for (float s = start; s < bw; s += start) {
                            canvas->drawImage(id, Rect(0, s, bw, start));
                        }
                    }
                }

            } else if (style()->bgSizeType() == BackgroundSizeType::SizeValue) {
                float w, h;
                if (style()->bgSizeValue()->width().isAuto()) {
                    w = id->width();
                } else {
                    w = style()->bgSizeValue()->width().specifiedValue(bw);
                }

                if (style()->bgSizeValue()->height().isAuto()) {
                    h = id->height();
                } else {
                    h = style()->bgSizeValue()->height().specifiedValue(bh);
                }

                canvas->drawImage(id, Rect(0, 0, w, h));
                if (style()->backgroundRepeatX() == BackgroundRepeatValue::RepeatRepeatValue) {
                    if (style()->backgroundRepeatY() == BackgroundRepeatValue::RepeatRepeatValue) {
                        for (float x = 0; x < bw; x += w) {
                            for (float y = 0; y < bh; y += h) {
                                canvas->drawImage(id, Rect(x, y, w, h));
                            }
                        }
                    } else {
                        for (float x = 0; x < bw; x += w) {
                            canvas->drawImage(id, Rect(x, 0, w, h));
                        }
                    }
                } else {
                    if (style()->backgroundRepeatY() == BackgroundRepeatValue::RepeatRepeatValue) {
                        for (float y = 0; y < bh; y += h) {
                            canvas->drawImage(id, Rect(0, y, w, h));
                        }
                    }
                }
            } else {
                STARFISH_ASSERT(style()->bgSizeType() == BackgroundSizeType::SizeNone);
                STARFISH_ASSERT_NOT_REACHED();
            }
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
