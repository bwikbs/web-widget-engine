#ifndef __StarFishElement__
#define __StarFishElement__

#include "dom/Node.h"

namespace StarFish {

class Element : public Node {
public:
    Element(DocumentElement* documentElement, ScriptBindingInstance* instance)
        : Node(documentElement, instance)
    {
        initScriptWrappable(this, instance);
        m_firstChild = nullptr;
    }
    Element(DocumentElement* documentElement)
        : Node(documentElement)
    {
        initScriptWrappable(this);
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

    Element* getElementById(String* id)
    {
        if (!id)
            return nullptr;

        Node* node = m_firstChild;
        while (node) {
            if (node->isElement()) {
                if (node->asElement()->id() && node->asElement()->id()->equals(id)) {
                    return node->asElement();
                }
                Element* ret = node->asElement()->getElementById(id);
                if (ret)
                    return ret;
            }
            node = node->nextSibling();
        }

        return nullptr;
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
            auto w = node->width();
            auto h = node->height();
            auto x = node->x();
            auto y = node->y();

            node->setComputedRect(Rect(x.fixed(), y.fixed(), w.fixed(), h.fixed()));
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

            canvas->translate(node->computedRect().x(), node->computedRect().y());
            node->paint(canvas);
            canvas->restore();
            node = node->nextSibling();
        }
    }

    virtual Node* hitTest(float x, float y)
    {
        Node* node = m_firstChild;
        Node* result = nullptr;
        while (node) {
            float nx = x - node->computedRect().x();
            float ny = y - node->computedRect().y();
            Node* r = node->hitTest(nx, ny);
            if (r) {
                result = r;
            }
            node = node->nextSibling();
        }

        if (!result) {
            result = Node::hitTest(x, y);
        }
        return result;
    }

protected:
    Node* m_firstChild;
};


}

#endif
