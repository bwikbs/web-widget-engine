#include "StarFishConfig.h"
#include "Frame.h"
#include "FrameBox.h"

namespace StarFish {

float LayoutContext::parentContentWidth(Frame* frm)
{
    return frm->parent()->asFrameBox()->contentWidth();
}


}
