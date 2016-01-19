#ifndef __StarFishFrameReplaced__
#define __StarFishFrameReplaced__

#include "layout/FrameBox.h"

namespace StarFish {

class FrameReplacedImage;

class FrameReplaced : public FrameBox {
public:
    FrameReplaced(Node* node, ComputedStyle* style)
        : FrameBox(node, style)
    {
    }

    virtual bool isFrameReplaced()
    {
        return true;
    }

    virtual bool isFrameReplacedImage()
    {
        return false;
    }

    FrameReplacedImage* asFrameReplacedImage()
    {
        STARFISH_ASSERT(isFrameReplacedImage());
        return (FrameReplacedImage*)this;
    }

    virtual const char* name()
    {
        return "FrameReplaced";
    }

    virtual void layout(LayoutContext& ctx)
    {
        // TODO A computed value of 'auto' for 'margin-left' or 'margin-right' becomes a used value of '0'.
        Size s = intrinsicSize();
        if (s.width() == 0 || s.height() == 0) {
            setContentWidth(0);
            setContentHeight(0);
            return;
        }
        if (style()->width().isAuto() && style()->height().isAuto()) {
            setContentWidth(s.width());
            setContentHeight(s.height());
        } else if (style()->width().isSpecified() && style()->height().isAuto()) {
            float w = style()->width().specifiedValue(ctx.parentContentWidth(this));
            float h = w * (s.height() / s.width());
            setContentWidth(w);
            setContentHeight(h);
        } else if (style()->width().isAuto() && style()->height().isSpecified()) {
            if (style()->height().isFixed()) {
                float h = style()->height().fixed();
                float w = h * (s.width() / s.height());
                setContentWidth(w);
                setContentHeight(h);
            } else {
                // FIXME
                setContentWidth(s.width());
                setContentHeight(s.height());
            }
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }

    virtual Size intrinsicSize() = 0;

    virtual void paintReplaced(Canvas* canvas)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    virtual void paint(Canvas* canvas, PaintingStage stage)
    {
        // FIXME check spec. is this stage right??
        if (stage == PaintingNormalFlowBlock) {
            paintReplaced(canvas);
        }
    }

protected:
};

}

#endif
