#include "StarFishConfig.h"
#include "Document.h"

#include "layout/FrameDocument.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}

Node* childMatchedById(Node* parent, String* id) {
    Node* child = parent->firstChild();
    while (child) {
        if (child->isElement()) {
            if (((Element*) child)->id()->equals(id))
                return child;
            Node* matchedDescendant = childMatchedById(child, id);
            if (matchedDescendant)
                return matchedDescendant;
        }

        child = child->nextSibling();
    }
    return nullptr;
}

Element* Document::getElementById(String* id) {
    Element* body = (Element*) ((Node*) this)->firstChild()->nextSibling()->firstChild()->nextSibling()->nextSibling(); // this->doctype->html->head->text->body
    return (Element*) childMatchedById(body, id);
}

}
