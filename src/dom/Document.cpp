#include "StarFishConfig.h"
#include "Document.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}

void Document::setDocumentStyle(ComputedStyle* s)
{
    m_documentStyle = s;
    setFrame(new Frame(this, s));
}


}
