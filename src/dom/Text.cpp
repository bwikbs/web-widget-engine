#include "StarFishConfig.h"
#include "Text.h"

#include "Document.h"

namespace StarFish {

String* Text::localName()
{
    return document()->window()->starFish()->staticStrings()->m_textLocalName;
}


}
