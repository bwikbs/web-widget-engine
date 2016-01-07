#include "StarFishConfig.h"
#include "ImageElement.h"
#include "DocumentElement.h"
#include "StarFish.h"

namespace StarFish {

void ImageElement::setSrc(String* src)
{
    m_src = src;
    m_imageData = m_documentElement->window()->starFish()->fetchImage(m_documentElement->window()->starFish()->makeResourcePath(src));
    setNeedsRendering();
}

}
