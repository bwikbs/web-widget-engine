#include "StarFishConfig.h"
#include "NamedNodeMap.h"
#include "dom/Document.h"
#include "dom/Element.h"
#include "dom/Attribute.h"
#include "dom/Attr.h"

namespace StarFish {

unsigned long NamedNodeMap::length()
{
    return m_element->getAttributes()->size() - 1; // The localName have to be excepted
}

Attr* NamedNodeMap::item(unsigned long index) {
    // The localName is considered
    if ((index+1) < m_element->getAttributes()->size()) {
        Attribute attr = m_element->getAttributes()->at(index + 1);
        return new Attr(m_instance, m_element, attr.name());
    } else
        return nullptr;
}
//Attr* NamedNodeMap::getNamedItem(String* name);
//Attr* NamedNodeMap::setNamedItem(Attr attr);
//Attr* NamedNodeMap::removeNamedItem(String* name);

}
