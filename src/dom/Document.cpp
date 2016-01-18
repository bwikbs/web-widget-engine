#include "StarFishConfig.h"
#include "Document.h"

#include "layout/FrameDocument.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}

Element* childMatchedById(Node* parent, String* id)
{
    Node* child = parent->firstChild();
    while (child) {
        if (child->isElement()) {
            if (child->asElement()->id()->equals(id))
                return child->asElement();
            Element* matchedDescendant = childMatchedById(child, id);
            if (matchedDescendant)
                return matchedDescendant;
        }

        child = child->nextSibling();
    }
    return nullptr;
}

Element* Document::getElementById(String* id)
{
    return childMatchedById(this, id);
}

}
