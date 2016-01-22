#ifndef __StarFishAttr__
#define __StarFishAttr__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Element;
class Attr : public ScriptWrappable {
public:
    Attr(ScriptBindingInstance* instance, Element* element, String* name)
    : m_element(element), m_name(name), m_standAloneValue(nullptr)
    {
        initScriptWrappable(this, instance);
    }

    Attr(ScriptBindingInstance* instance, String* name)
    : m_element(nullptr), m_name(name), m_standAloneValue(nullptr)
    {
        initScriptWrappable(this, instance);
    }

    String* name()
    {
        return m_name;
    }

    String* value()
    {
        return m_element->getAttribute(m_name);
    }

    void setValue(String* value) {
        if (m_element)
            m_element->setAttribute(m_name, value);
        else
            m_standAloneValue = value;
    }

    String* nodeValue() {
        return value();
    }

    void setNodeValue(String* value)
    {
        setValue(value);
    }

    String* textContent() {
        return value();
    }

    void setTextContext(String* value)
    {
        setValue(value);
    }

    Element* ownerElement() { return m_element; }

private:
    Element* m_element;
    String* m_name;
    String* m_standAloneValue;
};

}

#endif
