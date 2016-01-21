#ifndef __StarFishBlockBox__
#define __StarFishBlockBox__

#include "layout/FrameBox.h"
#include "layout/FrameReplaced.h"

namespace StarFish {

class InlineBox : public FrameBox {
public:
    InlineBox(Node* node, ComputedStyle* style)
        : FrameBox(node, style)
    {

    }

    virtual bool isInlineTextBox() const { return false; }
    virtual bool isInlineReplacedBox() const { return false; }
    virtual bool isInlineBlockBox() const { return false; }
};

class InlineTextBox : public InlineBox {
public:
    InlineTextBox(Node* node, ComputedStyle* style, String* str)
        : InlineBox(node, style)
    {
        m_text = str;
    }

    virtual bool isInlineTextBox() const { return true; }

    virtual void paint(Canvas* canvas, PaintingStage stage)
    {
        if (stage == PaintingNormalFlowInline) {
            canvas->setFont(style()->font());
            canvas->setColor(style()->color());
            canvas->drawText(0, 0, m_text);
        }
    }

protected:
    String* m_text;
};

class InlineReplacedBox : public InlineBox {
public:
    InlineReplacedBox(Node* node, ComputedStyle* style, FrameReplaced* f)
        : InlineBox(node, style)
    {
        m_frameReplaced = f;
    }

    virtual bool isInlineReplacedBox() const { return true; }

    virtual void paint(Canvas* canvas, PaintingStage stage)
    {
        if (stage == PaintingNormalFlowInline) {
            m_frameReplaced->paintReplaced(canvas);\
        }
    }

protected:
    FrameReplaced* m_frameReplaced;
};

class LineBox : public gc {
    friend class FrameBlockBox;
public:
    LineBox()
        : m_frameRect(0, 0, 0, 0)
    {

    }
protected:
    Rect m_frameRect;
    std::vector<InlineBox*, gc_allocator<InlineBox*>> m_boxes;
};

class FrameBlockBox : public FrameBox {
public:
    FrameBlockBox(Node* node, ComputedStyle* style)
        : FrameBox(node, style)
    {

    }

    virtual bool isFrameBlockBox()
    {
        return true;
    }

    virtual const char* name()
    {
        return "FrameBlockBox";
    }

    virtual void layout(LayoutContext& ctx)
    {
        if (hasBlockFlow()) {
            layoutBlock(ctx);
        } else {
            layoutInline(ctx);
        }
    }

    virtual void paint(Canvas* canvas, PaintingStage stage);
    virtual Frame* hitTest(float x, float y,HitTestStage stage);

protected:
    void layoutBlock(LayoutContext& ctx);
    void layoutInline(LayoutContext& ctx);

    bool hasBlockFlow()
    {
        return !m_firstChild || m_firstChild->isFrameBlockBox();
    }

    std::vector<LineBox, gc_allocator<LineBox>> m_lineBoxes;
};

}

#endif
