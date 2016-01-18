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
    }

    virtual NodeType nodeType() {
        return DOCUMENT_TYPE_NODE;
    }

protected:
};


}

#endif
