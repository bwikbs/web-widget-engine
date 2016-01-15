#include "StarFishConfig.h"
#include "Document.h"

#include "layout/FrameDocument.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}

void Document::setDocumentStyle(ComputedStyle* s)
{
    setStyle(s);
    setFrame(new FrameDocument(this, style()));
}


}
