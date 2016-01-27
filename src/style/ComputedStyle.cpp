#include "StarFishConfig.h"
#include "ComputedStyle.h"

#include "StarFish.h"

namespace StarFish {

void ComputedStyle::loadResources(StarFish* sf)
{
	if (m_inheritedStyles.m_fontSize.isPercent())
		m_font = sf->fetchFont(String::emptyString, m_inheritedStyles.m_fontSize.percent(), 0);
	else
		m_font = sf->fetchFont(String::emptyString, m_inheritedStyles.m_fontSize.fixed(), 0);
    if (!bgImage()->equals(String::emptyString)) {
        setBgImageData(sf->fetchImage(sf->makeResourcePath(bgImage())));
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
