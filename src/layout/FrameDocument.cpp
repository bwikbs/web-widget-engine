#include "StarFishConfig.h"
#include "FrameDocument.h"

namespace StarFish {

void FrameDocument::layout(LayoutContext& ctx)
{
    int w = node()->asDocument()->window()->width();
    int h = node()->asDocument()->window()->height();

    m_frameRect.setWidth(w);
    m_frameRect.setHeight(h);

    m_firstChild->layout(ctx);
}

}
