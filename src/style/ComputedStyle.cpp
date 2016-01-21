#include "StarFishConfig.h"
#include "ComputedStyle.h"

#include "StarFish.h"

namespace StarFish {

Font* ComputedStyle::fontSlowCase(StarFish* sf)
{
    m_font = sf->fetchFont(String::emptyString, m_inheritedStyles.m_fontSize, 0);
    return m_font;
}


}
