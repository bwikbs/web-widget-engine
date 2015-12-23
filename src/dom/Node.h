#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "util/Unit.h"
#include "style/Length.h"
#include "platform/window/Window.h"
#include "platform/canvas/Canvas.h"
#include "platform/canvas/font/Font.h"
#include "platform/canvas/image/ImageData.h"
#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Element;

class Node : public ScriptWrappable {
protected:
    Node(DocumentElement* documentElement, ScriptBindingInstance* instance)
        : m_computedRect(0, 0, 0, 0)
    {
        m_documentElement = documentElement;
        initScriptWrappable(this, instance);
        m_nextSibling = nullptr;
        m_parentElement = nullptr;
    }

    Node(DocumentElement* documentElement)
        : m_computedRect(0, 0, 0, 0)
    {
        m_documentElement = documentElement;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_parentElement = nullptr;
    }
public:
    virtual ~Node()
    {

    }

    virtual bool isElement()
    {
        return false;
    }

    virtual bool isDocumentElement()
    {
        return false;
    }

    Node* nextSibling()
    {
        return m_nextSibling;
    }

    void setNextSibling(Node* s)
    {
        m_nextSibling = s;
    }

    Element* parentElement()
    {
        return m_parentElement;
    }

    void setParentElement(Element* s)
    {
        m_parentElement = s;
    }

    Element* asElement()
    {
        ASSERT(isElement());
        return (Element*)this;
    }

    DocumentElement* documentElement()
    {
        return m_documentElement;
    }

    void setX(const Length& l)
    {
        m_x = l;
        setNeedsRendering();
    }

    void setY(const Length& l)
    {
        m_y = l;
        setNeedsRendering();
    }

    void setWidth(const Length& l)
    {
        m_width = l;
        setNeedsRendering();
    }

    void setHeight(const Length& l)
    {
        m_height = l;
        setNeedsRendering();
    }


    const Length& computedX() const
    {
        return m_computedX;
    }

    const Length& computedY() const
    {
        return m_computedY;
    }

    const Length& computedWidth() const
    {
        return m_computedWidth;
    }

    const Length& computedHeight() const
    {
        return m_computedHeight;
    }

    virtual void computeStyle()
    {
        // self
        m_computedX = m_x;
        m_computedY = m_y;
        m_computedHeight = m_height;
        m_computedWidth = m_width;
    }

    virtual void computeLayout()
    {

    }

    virtual void measureMinimumPreferredSize(float& width,float& height)
    {
        width = 0;
        height = 0;
    }

    void setComputedRect(const Rect& rt)
    {
        m_computedRect = rt;
    }

    Rect& mutableComputedRect()
    {
        return m_computedRect;
    }

    const Rect& computedRect() const
    {
        return m_computedRect;
    }

    inline void setNeedsRendering();

    virtual void paint(Canvas* canvas)
    {

    }
protected:
    Node* m_nextSibling;
    Element* m_parentElement;
    DocumentElement* m_documentElement;

    // inline style
    Length m_x, m_y, m_width, m_height;

    //computed style
    Length m_computedX, m_computedY, m_computedWidth, m_computedHeight;
    Rect m_computedRect;
};

}

#endif
