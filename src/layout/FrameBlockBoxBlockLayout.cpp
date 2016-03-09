#include "StarFishConfig.h"
#include "FrameBlockBox.h"
#include "FrameText.h"
#include "FrameInline.h"

namespace StarFish {

LayoutUnit FrameBlockBox::layoutBlock(LayoutContext& ctx)
{
    LayoutUnit top = paddingTop() + borderTop();
    LayoutUnit bottom = paddingBottom() + borderBottom();
    LayoutUnit normalFlowHeight = 0, maxNormalFlowBottom = top;
    MarginInfo marginInfo(top, bottom, isEstablishesBlockFormattingContext(), style()->height());
    LayoutUnit maxPositiveMarginTop, maxNegativeMarginTop;
    if (marginInfo.canCollapseTopWithChildren()) {
        if (marginTop() >= 0) {
            maxPositiveMarginTop = marginTop();
            ctx.setMaxPositiveMarginTop(std::max(ctx.maxPositiveMarginTop(), marginTop()));
            marginInfo.setPositiveMargin(ctx.maxPositiveMarginTop());
        } else {
            maxNegativeMarginTop = -marginTop();
            ctx.setMaxNegativeMarginTop(std::max(ctx.maxNegativeMarginTop(), maxNegativeMarginTop));
            marginInfo.setNegativeMargin(ctx.maxNegativeMarginTop());
        }
    } else {
        ctx.setMaxMarginTop(0, 0);
    }
    Frame* child = firstChild();
    if (!child)
        marginInfo.setMargin(0, 0);
    LayoutUnit marginForAbsolute;
    while (child) {
        // Place the child.
        child->asFrameBox()->setX(paddingLeft() + borderLeft());
        child->asFrameBox()->setY(normalFlowHeight + top);

        // Lay out the child
        if (child->isNormalFlow()) {
            child->layout(ctx);
            Length marginLeft = child->style()->marginLeft();
            Length marginRight = child->style()->marginRight();
            LayoutUnit mX = 0;
            if (!marginLeft.isAuto() && !marginRight.isAuto()) {
                // FIXME what "direction value" should we watch? self? child?
                if (style()->direction() == LtrDirectionValue) {
                    mX = child->asFrameBox()->marginLeft();
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
            } else if (marginLeft.isAuto() && !marginRight.isAuto()) {
                mX = contentWidth() - child->asFrameBox()->width();
                mX -= child->asFrameBox()->marginRight();
            } else if (!marginLeft.isAuto() && marginRight.isAuto()) {
                mX = child->asFrameBox()->marginLeft();
            } else {
                // auto-auto
                mX = child->asFrameBox()->marginLeft();
            }
            child->asFrameBox()->moveX(mX);

            LayoutUnit posTop = ctx.maxPositiveMarginTop(), negTop = ctx.maxNegativeMarginTop();
            if (child->asFrameBox()->marginTop() >= 0)
                posTop = std::max(child->asFrameBox()->marginTop(), posTop);
            else
                negTop = std::max(-child->asFrameBox()->marginTop(), negTop);

            if (marginInfo.canCollapseWithMarginTop() && marginInfo.atTopSideOfBlock()) {
                // NOTE: max[Positive/Negative]MarginTop has the collapsed margin between ancestors and first child
                maxPositiveMarginTop = std::max(maxPositiveMarginTop, posTop);
                maxNegativeMarginTop = std::max(maxNegativeMarginTop, negTop);
            }

            if (child->asFrameBox()->isSelfCollapsingBlock(ctx)) {
                if (child->asFrameBox()->marginBottom() >= 0)
                    posTop = std::max(posTop, child->asFrameBox()->marginBottom());
                else
                    negTop = std::max(negTop, -child->asFrameBox()->marginBottom());
            }
            posTop = std::max(marginInfo.positiveMargin(), posTop);
            negTop = std::max(marginInfo.negativeMargin(), negTop);
            if (child->asFrameBox()->isSelfCollapsingBlock(ctx)) {
                marginInfo.setMargin(posTop, negTop);
            } else {
                if (!marginInfo.atTopSideOfBlock() || !marginInfo.canCollapseWithMarginTop()) {
                    LayoutUnit logicalTop = std::max(marginInfo.positiveMargin(), posTop)
                        - std::max(marginInfo.negativeMargin(), negTop);
                    child->asFrameBox()->moveY(logicalTop);
                }
                marginInfo.setMargin(child->asFrameBox()->marginBottom());
                marginForAbsolute = marginInfo.positiveMargin() - marginInfo.negativeMargin();
                marginInfo.setPositiveMargin(std::max(marginInfo.positiveMargin(), ctx.maxPositiveMarginBottom()));
                marginInfo.setNegativeMargin(std::max(marginInfo.negativeMargin(), ctx.maxNegativeMarginBottom()));
            }
            ctx.setMaxMarginTop(marginInfo.positiveMargin(), marginInfo.negativeMargin());

            if (marginInfo.atTopSideOfBlock() && !child->asFrameBox()->isSelfCollapsingBlock(ctx)) {
                marginInfo.setAtTopSideOfBlock(false);
            }
        }

        if (child->isNormalFlow()) {
            if (maxNormalFlowBottom < child->asFrameBox()->height() + child->asFrameBox()->y())
                maxNormalFlowBottom = child->asFrameBox()->height() + child->asFrameBox()->y();
            normalFlowHeight = child->asFrameBox()->height() + child->asFrameBox()->y() - top;
        } else {
            child->asFrameBox()->moveY(marginForAbsolute);
            ctx.registerAbsolutePositionedFrames(child);
        }

        child = child->next();
    }

    // NOTE: At this point, ctx.max[P/N]MarginTop has the collapsed margin
    // between collapsible ancestors and first descendants.
    ctx.setMaxMarginTop(maxPositiveMarginTop, maxNegativeMarginTop);
    // NOTE: At this point, ctx.max[P/N]MarginBottom has the collapsed margin
    // between this block and last descendants.
    ctx.setMaxMarginBottom(std::max(ctx.maxPositiveMarginBottom(), marginInfo.positiveMargin()),
        std::max(ctx.maxNegativeMarginBottom(), marginInfo.negativeMargin()));
    normalFlowHeight = maxNormalFlowBottom - top;
    if (!marginInfo.canCollapseWithMarginBottom()) {
        normalFlowHeight += ctx.maxPositiveMarginBottom() - ctx.maxNegativeMarginBottom();
        ctx.setMaxMarginBottom(0, 0);
    }
    return normalFlowHeight;
}

}
