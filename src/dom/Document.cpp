#include "StarFishConfig.h"
#include "Document.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}


}
