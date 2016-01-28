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
        Size s = intrinsicSize();
        float parentContentWidth = ctx.blockContainer(this)->asFrameBox()->contentWidth();
        computeBorderMarginPadding(parentContentWidth);

        if (isNormalFlow()) {
            if ((s.width() == 0 || s.height() == 0) && (style()->width().isAuto() || style()->height().isAuto())) {
                setContentWidth(0);
                setContentHeight(0);
            } else if (style()->width().isAuto() && style()->height().isAuto()) {
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
                STARFISH_ASSERT(style()->width().isSpecified() && style()->height().isSpecified());
                setContentWidth(style()->width().specifiedValue(ctx.parentContentWidth(this)));
                if (style()->height().isFixed()) {
                    setContentHeight(style()->height().fixed());
                } else {
                    if (ctx.parentHasFixedHeight(this))
                        setContentHeight(style()->height().percent() * ctx.parentFixedHeight(this));
                    else
                        setContentHeight(s.height());
                }
            }

            if (style()->display() == BlockDisplayValue && style()->marginLeft().isAuto() && style()->marginRight().isAuto()) {
                float remain = parentContentWidth;
                remain -= contentWidth();
                remain -= borderWidth();
                remain -= paddingWidth();
                if (remain > 0) {
                    setMarginLeft(remain / 2);
                    setMarginRight(remain / 2);
                }
            }
        }
    }

    virtual Size intrinsicSize() = 0;

    virtual void paintReplaced(Canvas* canvas)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    virtual void paint(Canvas* canvas, PaintingStage stage)
    {
        if (stage == PaintingNormalFlowInline) {
            paintBackgroundAndBorders(canvas);
            paintReplaced(canvas);
        }
    }

protected:
};

}

#endif
