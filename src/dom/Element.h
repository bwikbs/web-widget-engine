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

    virtual NodeType nodeType() {
        return ELEMENT_NODE;
    }

    virtual String* nodeName() = 0;

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
