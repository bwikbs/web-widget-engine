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

    virtual bool isElement()
    {
        return true;
    }

    virtual void computeStyle()
    {
        Node::computeStyle();

        Node* node = m_firstChild;
        while (node) {
            node->computeStyle();
            node = node->nextSibling();
        }
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

    size_t hasAtttibute(String* name);
    void setAttribute(String* name, String* value);
    void removeAttribute(String* name);

    virtual void didAttributeChanged(String* name, String* old, String* value);

protected:
    std::vector<Attribute, gc_allocator<Attribute>> m_attributes;

    String* m_id;
    std::vector<String*, gc_allocator<String*>> m_classNames;

};


}

#endif
