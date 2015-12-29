#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "util/Unit.h"
#include "style/Length.h"
#include "style/Drawable.h"
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
        m_id = nullptr;

        m_angle = 0;
        m_scale = 1;

        m_state = NodeStateNormal;
    }

    Node(DocumentElement* documentElement)
        : m_computedRect(0, 0, 0, 0)
    {
        m_documentElement = documentElement;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_parentElement = nullptr;
        m_id = nullptr;

        m_angle = 0;
        m_scale = 1;

        m_state = NodeStateNormal;
    }
public:
    enum NodeState {
        NodeStateNormal,
        NodeStateActive,
    };

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

    void setId(String* id)
    {
        m_id = id;
        // TODO needs re-rendering?
    }

    String* id()
    {
        return m_id;
    }

    void setState(NodeState state)
    {
        m_state = state;
    }

    NodeState state()
    {
        return m_state;
    }

    void setBackground(const Drawable& d)
    {
        m_background = d;
    }

    const Drawable& background()
    {
        return m_background;
    }

    void setBackgroundWhenActive(const Drawable& d)
    {
        m_backgroundWhenActive = d;
    }

    const Drawable& backgroundWhenActive()
    {
        return m_backgroundWhenActive;
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

    const Length& x() const
    {
        return m_x;
    }

    const Length& y() const
    {
        return m_y;
    }

    const Length& width() const
    {
        return m_width;
    }

    const Length& height() const
    {
        return m_height;
    }

    virtual void computeStyle()
    {
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
        canvas->scale(m_scale, m_scale, m_computedRect.width()/2, m_computedRect.height()/2);
        canvas->rotate(m_angle, m_computedRect.width()/2, m_computedRect.height()/2);

        Drawable* target = &m_background;
        if ((m_state == NodeStateActive && m_backgroundWhenActive.type() != Drawable::None)) {
            target = &m_backgroundWhenActive;
        }
        if (target->type() == Drawable::SolidColor) {
            canvas->save();
            canvas->setColor(target->color());
            canvas->drawRect(Rect(0, 0, m_computedRect.width(), m_computedRect.height()));
            canvas->restore();
        } else if (target->type() == Drawable::Image) {
            canvas->drawImage(target->image(), Rect(0, 0, m_computedRect.width(), m_computedRect.height()));
        }
    }

    virtual Node* hitTest(float x, float y)
    {
        if (x > 0 && y > 0 && x < m_computedRect.width() && y < m_computedRect.height())
            return this;
        return NULL;
    }

    virtual bool onTouchEvent(Window::TouchEventKind kind, float x, float y);

    float scale()
    {
        return m_scale;
    }

    void setScale(float f)
    {
        m_scale = f;
        setNeedsRendering();
    }

    float angle()
    {
        return m_angle;
    }

    void setAngle(float f)
    {
        m_angle = f;
        setNeedsRendering();
    }


protected:
    Node* m_nextSibling;
    Element* m_parentElement;
    DocumentElement* m_documentElement;

    String* m_id;

    // inline style
    Length m_x, m_y, m_width, m_height;

    // transform
    float m_angle;
    float m_scale;

    //computed layout
    Rect m_computedRect;

    NodeState m_state;
    Drawable m_background;
    Drawable m_backgroundWhenActive;
};

}

#endif
