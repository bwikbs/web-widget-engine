#ifndef __StarFishDOMSettableTokenList__
#define __StarFishDOMSettableTokenList__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Element;
class DOMSettableTokenList : public DOMTokenList {
public:
    DOMSettableTokenList(ScriptBindingInstance* instance, Element* element, String* localName)
    : DOMTokenList(instance,element,localName)
    {
        initScriptWrappable(this, instance);
    }

    String* value()
    {
        return m_value;
    }

    void setValue(String* value)
    {
        m_value = value;
    }
private:
    String* m_value;
};

}

#endif
