#ifndef __StarFishFrameReplacedImage__
#define __StarFishFrameReplacedImage__

#include "layout/FrameReplaced.h"
#include "platform/canvas/image/ImageData.h"

namespace StarFish {

class FrameReplacedImage : public FrameReplaced {
public:
    FrameReplacedImage(Node* node, ComputedStyle* style, String* src)
        : FrameReplaced(node, style)
    {
        m_imageData = node->document()->window()->starFish()->fetchImage(node->document()->window()->starFish()->makeResourcePath(src));
    }

    virtual bool isFrameReplacedImage()
    {
        return true;
    }

    virtual const char* name()
    {
        return "FrameReplacedImage";
    }

    virtual void layout(LayoutContext& ctx)
    {
        m_frameRect.setWidth(m_imageData->width());
        m_frameRect.setHeight(m_imageData->height());
    }

    virtual void paintReplaced(Canvas* canvas)
    {
        canvas->drawImage(m_imageData, Rect(0, 0, width(), height()));
    }

protected:
    ImageData* m_imageData;
};

}

#endif
