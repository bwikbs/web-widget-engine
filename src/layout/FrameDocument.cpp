#include "StarFishConfig.h"
#include "FrameDocument.h"

namespace StarFish {

void FrameDocument::layout(LayoutContext& ctx)
{
    int w = node()->asDocument()->window()->width();
    int h = node()->asDocument()->window()->height();

    style()->setWidth(Length(Length::Fixed, w));
    style()->setHeight(Length(Length::Fixed, h));

    FrameBlockBox::layout(ctx);
}

}
