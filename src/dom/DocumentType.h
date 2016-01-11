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

    virtual void computeStyle()
    {
        Node::computeStyle();
    }

    virtual void computeLayout()
    {
        Node::computeLayout();
    }

    virtual void paint(Canvas* canvas)
    {
    }

    virtual Node* hitTest(float x, float y)
    {
        return NULL;
    }
protected:
};


}

#endif
