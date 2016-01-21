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
//    Attr* getNamedItem(String* name);
//    Attr* setNamedItem(Attr attr);
//    Attr* removeNamedItem(String* name);
private:
    Element* m_element;
    ScriptBindingInstance* m_instance;
};

}

#endif
