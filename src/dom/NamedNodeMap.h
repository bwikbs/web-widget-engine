#ifndef __StarFishNamedNodeMap__
#define __StarFishNamedNodeMap__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Element;
class Attr;
class NamedNodeMap : public ScriptWrappable {
public:
    NamedNodeMap(ScriptBindingInstance* instance, Element* element)
    : m_element(element), m_instance(instance) {
        initScriptWrappable(this, instance);
    }
    unsigned long length();
    Attr* item(unsigned long index);
    Attr* getNamedItem(String* name);
    void setNamedItem(Attr* attr);
    void removeNamedItem(String* name);
    ScriptBindingInstance* striptBindingInstance() { return m_instance; }
private:
    Element* m_element;
    ScriptBindingInstance* m_instance;
};

}

#endif
