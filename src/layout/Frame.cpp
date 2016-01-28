#include "StarFishConfig.h"
#include "Frame.h"
#include "FrameBox.h"

namespace StarFish {

Frame* LayoutContext::blockContainer(Frame* currentFrame)
{
    Frame* f = currentFrame->parent();

    if (!f)
        return currentFrame;

    while (true) {
        if (f->isFrameBlockBox() && f->node() != nullptr) {
            return f;
        }
        f = f->parent();
    }
}

Frame* LayoutContext::containingBlock(Frame* currentFrame)
{
    // https://www.w3.org/TR/2011/REC-CSS2-20110607/visudet.html#containing-block-details
    Frame* block = blockContainer(currentFrame);
    if (currentFrame->style()->position() == AbsolutePositionValue) {
        while (!block->isFrameDocument() && !block->isPositionedElement()) {
            block = blockContainer(block);
        }
        return block;
    } else {
        return block;
    }
}

float LayoutContext::parentContentWidth(Frame* currentFrame)
{
    return blockContainer(currentFrame)->asFrameBox()->contentWidth();
}

bool LayoutContext::parentHasFixedHeight(Frame* currentFrame)
{
    return blockContainer(currentFrame)->style()->height().isFixed();
}

float LayoutContext::parentFixedHeight(Frame* currentFrame)
{
    STARFISH_ASSERT(parentHasFixedHeight(currentFrame));
    return blockContainer(currentFrame)->style()->height().fixed();
}

}
