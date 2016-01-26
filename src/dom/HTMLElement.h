#ifndef __StarFishHTMLElement__
#define __StarFishHTMLElement__

#include "dom/Element.h"

namespace StarFish {

#define FOR_EACH_HTML_ELEMENT_OP(F) \
        F(Html) \
        F(Head) \
        F(Script) \
        F(Style) \
        F(Body) \
        F(Div) \
        F(Image) \
        F(BR)

#define FORWRAD_DECLARE_NAME(kind) class HTML##kind##Element;

#define IS_KIND_ELEMENT(kind) \
    virtual bool isHTML##kind##Element() const { return false; } \
    virtual HTML##kind##Element* asHTML##kind##Element() \
    { \
        STARFISH_ASSERT(isHTML##kind##Element()); \
        return (HTML##kind##Element*)this; \
    }

FOR_EACH_HTML_ELEMENT_OP(FORWRAD_DECLARE_NAME)

class HTMLElement : public Element {
public:
    HTMLElement(Document* document)
        : Element(document)
    {
        initScriptWrappable(this);
        m_id = String::emptyString;
        m_className = String::emptyString;
    }

    /* 4.4 Interface Node */

    virtual String* nodeName()
    {
        return localName()->toUpper();
    }

    /* Other methods (not in DOM API) */

    virtual bool isHTMLElement() const
    {
        return true;
    }

    FOR_EACH_HTML_ELEMENT_OP(IS_KIND_ELEMENT);

protected:
    String* m_id;
    String* m_className;
};

}

#endif
