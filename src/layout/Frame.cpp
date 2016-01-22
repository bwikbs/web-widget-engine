#include "StarFishConfig.h"
#include "Frame.h"
#include "FrameBox.h"

namespace StarFish {

Frame* LayoutContext::blockContainer(Frame* currentFrame)
{
    Frame* f = currentFrame->parent();

    while (true) {
        if (f->isFrameBlockBox() && f->node() != nullptr) {
            return f;
        }
        f = f->parent();
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
