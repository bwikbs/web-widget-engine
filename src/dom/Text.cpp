#include "StarFishConfig.h"
#include "Text.h"

#include "Document.h"

namespace StarFish {

String* Text::nodeName()
{
    return document()->window()->starFish()->staticStrings()->m_textLocalName;
}

String* Text::localName()
{
    return document()->window()->starFish()->staticStrings()->m_textLocalName;
}


}
