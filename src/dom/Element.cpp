#include "StarFishConfig.h"
#include "Element.h"

#include "Document.h"

namespace StarFish {

size_t Element::hasAtttibute(String* name)
{
    for (size_t i = 0; i < m_attributes.size(); i ++) {
        if (m_attributes[i].name()->equals(name)) {
            return i;
        }
    }
    return SIZE_MAX;
}

void Element::setAttribute(String* name, String* value)
{
    size_t idx = hasAtttibute(name);
    if (idx == SIZE_MAX) {
        m_attributes.push_back(Attribute(name, value));
        didAttributeChanged(name, String::emptyString, value);
    } else {
        String* v = m_attributes[idx].value();
        m_attributes[idx].setValue(value);
        didAttributeChanged(name, v, value);
    }
}

void Element::removeAttribute(String* name)
{
    size_t idx = hasAtttibute(name);
    if (idx == SIZE_MAX) {
    } else {
        String* v = m_attributes[idx].value();
        m_attributes.erase(m_attributes.begin() + idx);
        didAttributeChanged(name, v, String::emptyString);
    }
}

void Element::didAttributeChanged(String* name, String* old, String* value)
{
    if (name->equals("id")) {
        m_id = value;
        setNeedsStyleRecalc();
    } else if (name->equals("class")) {
        m_classNames.clear();

        // TODO split class Names
        STARFISH_RELEASE_ASSERT(value->indexOf(' ') == SIZE_MAX);
        m_classNames.push_back(value);

        setNeedsStyleRecalc();
    }
}

}
