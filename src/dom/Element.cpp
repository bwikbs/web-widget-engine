#include "StarFishConfig.h"
#include "Element.h"

#include "Document.h"

#include "HTMLHtmlElement.h"
#include "HTMLHeadElement.h"
#include "HTMLBodyElement.h"
#include "HTMLDivElement.h"
#include "HTMLImageElement.h"
#include "HTMLScriptElement.h"
#include "HTMLStyleElement.h"


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

String* Element::getAttribute(size_t pos)
{
    return m_attributes[pos].value();
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

        const char* data = value->utf8Data();
        size_t length = value->length();

        bool isWhiteSpaceState = true;

        std::string str;
        for (size_t i = 0; i < length; i ++) {
            if (isWhiteSpaceState) {
                if (data[i] != ' ') {
                    isWhiteSpaceState = false;
                    str += data[i];
                }
            } else {
                if (data[i] == ' ') {
                    isWhiteSpaceState = true;
                    m_classNames.push_back(String::fromUTF8(str.data(), str.length()));
                    str.clear();
                } else {
                    str += data[i];
                }
            }
        }

        if (str.length()) {
            m_classNames.push_back(String::fromUTF8(str.data(), str.length()));
        }
        STARFISH_RELEASE_ASSERT(value->indexOf(' ') == SIZE_MAX || m_classNames.size() > 1);
        m_classNames.push_back(value);

        setNeedsStyleRecalc();
    }
}

Node* Element::clone()
{
    Node* newNode = nullptr;
    if(isHTMLElement()) {
        if(localName()->equals("html")) {
            newNode = new HTMLHtmlElement(document());
        } else if(localName()->equals("head")) {
            newNode = new HTMLHeadElement(document());
        } else if(localName()->equals("body")) {
            newNode = new HTMLBodyElement(document());
        } else if(localName()->equals("div")) {
            newNode = new HTMLDivElement(document());
        } else if(localName()->equals("img")) {
            newNode = new HTMLImageElement(document());
        } else if(localName()->equals("script")) {
            newNode = new HTMLScriptElement(document());
        } else if(localName()->equals("style")) {
            newNode = new HTMLStyleElement(document());
        } else {
            STARFISH_RELEASE_ASSERT(false);
        }
    } else {
        STARFISH_RELEASE_ASSERT(false);
    }

    // FIXME: impl 4.8.1 NamedNodeMap to iterate attr

    STARFISH_ASSERT(newNode);
    return newNode;
}

}
