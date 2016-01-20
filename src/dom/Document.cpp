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

HTMLCollection* Document::getElementsByTagName(String* qualifiedName)
{
  auto filter = [=](Node* node) {
      if ( node->isElement() && node->localName()->equals(qualifiedName))
          return true;
      return false;
  };
  return new HTMLCollection(scriptBindingInstance(), this, filter);
}

HTMLCollection* Document::getElementsByClassName(String* classNames)
{
  auto filter = [=](Node* node) {
      if (node->isElement()&&node->asElement()->classNames().size()>0){

        bool is_single_query = true;

        std::string query = std::string(classNames->utf8Data());
        std::string::size_type start = 0;
        std::string::size_type pos = query.find(" ");

        while (pos != std::string::npos||is_single_query)
        {
            bool is_match = false;
            is_single_query = false;
            String* tok = String::fromUTF8(query.substr(start, pos - start).data());
            auto c_classNames =  node->asElement()->classNames();
            for (unsigned i = 0; i < c_classNames.size(); i ++) {
              if(tok->equals(c_classNames[i]))
                is_match=true;
            }
            if(is_match==false)
              return false;

            start = pos + 1;
            pos = query.find(" ", start);
        }
        return true;
      }
      return false;
  };
  return new HTMLCollection(scriptBindingInstance(), this, filter);
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
