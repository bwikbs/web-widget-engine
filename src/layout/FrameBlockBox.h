#ifndef __StarFishBlockBox__
#define __StarFishBlockBox__

#include "layout/FrameBox.h"
#include "layout/FrameReplaced.h"

namespace StarFish {

class FrameBlockBox;
class InlineTextBox;
class InlineBlockBox;

class InlineBox : public FrameBox {
public:
    InlineBox(Node* node, ComputedStyle* style, Frame* parent)
        : FrameBox(node, style)
    {
        setParent(parent);
    }


    Frame* hitTest(float x, float y, HitTestStage stage)
    {
        if (stage == HitTestStage::HitTestNormalFlowInline) {
            return FrameBox::hitTest(x, y, stage);
        }
        return nullptr;
    }

    virtual bool isInlineTextBox() const { return false; }
    virtual bool isInlineReplacedBox() const { return false; }
    virtual bool isInlineBlockBox() const { return false; }

    InlineTextBox* asInlineTextBox()
    {
        STARFISH_ASSERT(isInlineTextBox());
        return (InlineTextBox*)this;
    }

    InlineBlockBox* asInlineBlockBox()
    {
        STARFISH_ASSERT(isInlineBlockBox());
        return (InlineBlockBox*)this;
    }
};

class InlineTextBox : public InlineBox {
public:
    InlineTextBox(Node* node, ComputedStyle* style, Frame* parent, String* str)
        : InlineBox(node, style, parent)
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

    virtual const char* name()
    {
        return "InlineTextBox";
    }

protected:
    String* m_text;
};

class InlineReplacedBox : public InlineBox {
public:
    InlineReplacedBox(Node* node, ComputedStyle* style, Frame* parent, FrameReplaced* f)
        : InlineBox(node, style, parent)
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

    virtual const char* name()
    {
        return "InlineReplacedBox";
    }

protected:
    FrameReplaced* m_frameReplaced;
};

class InlineBlockBox : public InlineBox {
    friend FrameBlockBox;
public:
    InlineBlockBox(Node* node, ComputedStyle* style, Frame* parent, FrameBlockBox* f, float ascender)
        : InlineBox(node, style, parent)
    {
        m_frameBlockBox = f;
        m_ascender = ascender;
    }

    virtual bool isInlineBlockBox() const { return true; }

    virtual void paint(Canvas* canvas, PaintingStage stage);
    virtual Frame* hitTest(float x, float y, HitTestStage stage);
    virtual const char* name()
    {
        return "InlineBlockBox";
    }

protected:
    float m_ascender;
    FrameBlockBox* m_frameBlockBox;
};

class LineBox : public FrameBox {
    friend class FrameBlockBox;
public:
    LineBox(Frame* parent)
        : FrameBox(nullptr, nullptr)
    {
        setParent(parent);
        m_decender = m_ascender = 0;
    }

    float ascender()
    {
        return m_ascender;
    }

    float decender()
    {
        return m_decender;
    }

protected:
    // FIXME
    // we use these value only for vertical-align of inline-block
    // in layout, we use only 'ascender'
    // should we delete m_decender?
    float m_ascender;
    float m_decender;
    std::vector<InlineBox*, gc_allocator<InlineBox*>> m_boxes;
};

class FrameBlockBox : public FrameBox {
    friend class LineFormattingContext;
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

    virtual void computePreferredWidth(ComputePreferredWidthContext& ctx);

    virtual void dump(int depth);
    virtual void paint(Canvas* canvas, PaintingStage stage);
    virtual Frame* hitTest(float x, float y,HitTestStage stage);

    bool hasBlockFlow()
    {
        return !m_firstChild || m_firstChild->isFrameBlockBox();
    }
protected:
    void layoutBlock(LayoutContext& ctx);
    void layoutInline(LayoutContext& ctx);

    std::vector<LineBox, gc_allocator<LineBox>> m_lineBoxes;

    static void paintChildrenWith(FrameBlockBox* block, Canvas* canvas, PaintingStage stage);
};

}

#endif
