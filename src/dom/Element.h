#ifndef __StarFishElement__
#define __StarFishElement__

#include "dom/Node.h"
#include "dom/Attribute.h"

namespace StarFish {

class HTMLElement;

class Element : public Node {
public:
    Element(Document* document, ScriptBindingInstance* instance)
        : Node(document, instance)
    {
        initScriptWrappable(this, instance);
        m_id = String::emptyString;
    }

    Element(Document* document)
        : Node(document)
    {
        initScriptWrappable(this);
        m_id = String::emptyString;
    }

    /* 4.4 Interface Node */
    virtual NodeType nodeType() {
        return ELEMENT_NODE;
    }

    virtual String* nodeName() = 0;

    virtual Element* parentElement()
    {
        Node* parent = parentNode();
        if(parent->nodeType() == ELEMENT_NODE) {
            return parent->asElement();
        } else {
            return nullptr;
        }
    }

    virtual String* nodeValue()
    {
        return nullptr;
    }

    virtual String* textContent()
    {
        if((nodeType() == TEXT_NODE) || (nodeType() == DOCUMENT_FRAGMENT_NODE)) {
            return textContent();
        }
        String* str = String::createASCIIString("");
        for(Node* child = firstChild(); child != nullptr; child = child->nextSibling()) {
            str = str->concat(child->textContent());
        }
        return str;
    }

    /* Other methods (not in Node interface) */

    virtual bool isElement()
    {
        return true;
    }

    virtual bool isHTMLElement()
    {
        return false;
    }

    HTMLElement* asHTMLElement()
    {
        STARFISH_ASSERT(isHTMLElement());
        return (HTMLElement*)this;
    }

    String* id()
    {
        return m_id;
    }

    // DO NOT MODIFY THIS VECTOR
    const std::vector<String*, gc_allocator<String*>>& classNames()
    {
        return m_classNames;
    }

    size_t hasAtttibute(String* name);
    String* getAttribute(String* name)
    {
        size_t siz = hasAtttibute(name);
        if (siz == SIZE_MAX)
            return String::emptyString;
        return getAttribute(siz);
    }
    String* getAttribute(size_t pos);
    void setAttribute(String* name, String* value);
    void removeAttribute(String* name);

    virtual void didAttributeChanged(String* name, String* old, String* value);

    virtual void dump()
    {
        Node::dump();

        printf("id:%s, ", m_id->utf8Data());
        std::string className;
        for (unsigned i = 0; i < m_classNames.size(); i ++) {
            className += m_classNames[i]->utf8Data();
            className += " ";
        }

        printf("className:%s", className.data());
    }

protected:
    std::vector<Attribute, gc_allocator<Attribute>> m_attributes;

    String* m_id;
    std::vector<String*, gc_allocator<String*>> m_classNames;

};


}

#endif
