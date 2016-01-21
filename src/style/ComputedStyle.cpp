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

ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle)
{
    if (memcmp(&oldStyle->m_inheritedStyles, &newStyle->m_inheritedStyles, sizeof (ComputedStyle::InheritedStyles)) != 0) {
        return ComputedStyleDamage::ComputedStyleDamageInherited;
    }
    return ComputedStyleDamage::ComputedStyleDamageNone;
}


}
