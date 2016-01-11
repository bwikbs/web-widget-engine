#ifndef __StarFishElement__
#define __StarFishElement__

#include "dom/Node.h"

namespace StarFish {

class Element : public Node {
public:
    Element(Document* document, ScriptBindingInstance* instance)
        : Node(document, instance)
    {
        initScriptWrappable(this, instance);
    }
    Element(Document* document)
        : Node(document)
    {
        initScriptWrappable(this);
    }

    virtual bool isElement()
    {
        return true;
    }

    virtual void computeStyle()
    {
        Node::computeStyle();

        Node* node = m_firstChild;
        while (node) {
            node->computeStyle();
            node = node->nextSibling();
        }
    }

    virtual void computeLayout()
    {
        Node::computeLayout();
        Node* node = m_firstChild;
        while (node) {
            node->computeLayout();
            node = node->nextSibling();
        }
    }

    virtual void paint(Canvas* canvas)
    {
        Node::paint(canvas);
        Node* node = m_firstChild;
        while (node) {
            canvas->save();
            // canvas->translate(node->computedRect().x(), node->computedRect().y());
            node->paint(canvas);
            canvas->restore();
            node = node->nextSibling();
        }
    }

    virtual Node* hitTest(float x, float y)
    {
        return NULL;
    }

protected:
};


}

#endif
