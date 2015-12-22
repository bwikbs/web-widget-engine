#ifndef __StarFishElement__
#define __StarFishElement__

#include "dom/Node.h"

namespace StarFish {

class Element : public Node {
public:
    Element(DocumentElement* documentElement)
        : Node(documentElement)
    {
        m_firstChild = nullptr;
    }

    virtual bool isElement()
    {
        return true;
    }

    Node* firstChild()
    {
        return m_firstChild;
    }

    void setFirstChild(Node* s)
    {
        m_firstChild = s;
    }

    void appendChild(Node* child)
    {
        ASSERT(child->parentElement() == nullptr);
        if (m_firstChild) {
            Node* node = m_firstChild;
            while (node->nextSibling() != nullptr) {
                node = node->nextSibling();
            }
            ASSERT(node->nextSibling() == nullptr);
            node->setNextSibling(child);
        } else {
            m_firstChild = child;
        }
        child->setParentElement(this);
        setNeedsRendering();
    }

    void removeChild(Node* child)
    {
        ASSERT(child);
        ASSERT(child->parentElement() == this);
        Node* prevNode = nullptr;
        Node* node = m_firstChild;
        while (node != child) {
            prevNode = node;
            node = node->nextSibling();
        }

        ASSERT(node == child);
        node->setParentElement(nullptr);
        prevNode->setNextSibling(node->nextSibling());
        node->setNextSibling(nullptr);
        setNeedsRendering();
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
            auto w = node->computedWidth();
            auto h = node->computedHeight();
            auto x = node->computedX();
            auto y = node->computedY();

            node->setComputedRect(Rect(x.fixed(), y.fixed(), w.fixed(), h.fixed()));
            node->computeLayout();

            node = node->nextSibling();
        }
    }

    virtual void paint(Canvas* canvas)
    {
        Node* node = m_firstChild;
        while (node) {
            canvas->save();

            canvas->translate(node->computedRect().x(), node->computedRect().y());
            node->paint(canvas);
            canvas->restore();
            node = node->nextSibling();
        }

    }

protected:
    Node* m_firstChild;
};


}

#endif
