#include "StarFishConfig.h"
#include "Document.h"
#include "Traverse.h"

#include "layout/FrameDocument.h"

namespace StarFish {

String* Document::localName()
{
    return window()->starFish()->staticStrings()->m_documentLocalName;
}

Element* Document::getElementById(String* id)
{
    return (Element*) Traverse::findDescendant(this, [&](Node* child){
        if (child->isElement() && child->asElement()->id()->equals(id)) {
            return true;
        } else
            return false;

    });
}


Element* Document::createElement(String* localName)
{
  return NULL;
}

Text*    Document::createTextNode(String* data)
{
  return new Text(this,data);
}


}
