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
  if(localName->equals(window()->starFish()->staticStrings()->m_htmlLocalName)){
    return new HTMLElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_headLocalName)){
    return new HTMLHeadElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_styleLocalName)){
    return new HTMLStyleElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_scriptLocalName)){
    return new HTMLScriptElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_bodyLocalName)){
    return new HTMLBodyElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_divLocalName)){
    return new HTMLDivElement(this);
  }else if(localName->equals(window()->starFish()->staticStrings()->m_imageLocalName)){
    return new HTMLImageElement(this);
  }
  return new HTMLUnknownElement(this,localName);
}

Text*    Document::createTextNode(String* data)
{
  return new Text(this,data);
}


}
