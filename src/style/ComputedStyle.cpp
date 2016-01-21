#include "StarFishConfig.h"
#include "ComputedStyle.h"

#include "StarFish.h"

namespace StarFish {

void ComputedStyle::loadResources(StarFish* sf)
{
    m_font = sf->fetchFont(String::emptyString, m_inheritedStyles.m_fontSize, 0);
    if (!m_background->m_image->equals(String::emptyString)) {
        m_background->m_imageData = sf->fetchImage(sf->makeResourcePath(m_background->m_image));
    }
}


}
