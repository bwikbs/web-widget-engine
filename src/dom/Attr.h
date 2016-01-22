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

    Attr(ScriptBindingInstance* instance, String* name, String* value)
    : m_element(nullptr), m_name(name), m_standAloneValue(value) {
        initScriptWrappable(this, instance);
    }

    String* name() {
        return m_name;
    }

    String* value() {
        if (m_element)
            return m_element->getAttribute(m_name);
        return m_standAloneValue;
    }

    void setValue(String* value) {
        if (m_element)
            m_element->setAttribute(m_name, value);
        else
            m_standAloneValue = value;
    }

    Element* ownerElement() { return m_element; }

private:
    Element* m_element;
    String* m_name;
    String* m_standAloneValue;
};

}

#endif
