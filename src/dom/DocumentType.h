#ifndef __StarFishDocumentType__
#define __StarFishDocumentType__

#include "dom/Node.h"

namespace StarFish {

class DocumentType : public Node {
public:
    DocumentType(Document* document)
        : Node(document)
    {
        initScriptWrappable(this);
        // FIXME: should be given by the parser
        m_name = String::createASCIIString("html");
        m_publicId = String::emptyString;
        m_systemId = String::emptyString;
    }

    virtual NodeType nodeType() {
        return DOCUMENT_TYPE_NODE;
    }

    virtual String* nodeName()
    {
        return m_name;
    }

    virtual Element* parentElement()
    {
        return nullptr;
    }

    virtual String* nodeValue()
    {
        return nullptr;
    }

    virtual String* textContent()
    {
        return nullptr;
    }

protected:
    String* m_name;
    String* m_publicId;
    String* m_systemId;
};

}

#endif
