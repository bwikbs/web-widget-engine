#ifndef __StarFishEventTarget__
#define __StarFishEventTarget__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Event;
class Node;
class Window;

class EventListener : public gc {
public:
    EventListener(ScriptValue fn, bool isAttribute = false, bool useCapture = false)
        : m_listener(fn)
        , m_isAttribute(isAttribute)
        , m_capture(useCapture)
    {
    }
    bool isAttribute() const
    {
        return m_isAttribute;
    }
    bool compare(const EventListener* other) const
    {
        return (m_isAttribute == other->m_isAttribute) && (m_listener == other->m_listener) && (m_capture == other->m_capture);
    }
    ScriptValue scriptValue()
    {
        return m_listener;
    }
    bool capture() const
    {
        return m_capture;
    }
    void setCapture(bool capture)
    {
        m_capture = capture;
    }

protected:
    ScriptValue m_listener;
    bool m_isAttribute;
    bool m_capture;
};

typedef std::vector<EventListener*, gc_allocator<EventListener*>> EventListenerVector;

class EventTarget : public ScriptWrappable {
protected:
    EventTarget()
        : ScriptWrappable(this)
    {
    }

    virtual ~EventTarget()
    {

    }

public:

    virtual bool isWindow()
    {
        return false;
    }

    Window* asWindow()
    {
        STARFISH_ASSERT(isWindow());
        return (Window*)this;
    }

    virtual bool isNode() const
    {
        return false;
    }

    Node* asNode()
    {
        STARFISH_ASSERT(isNode());
        return (Node*)this;
    }

    EventListenerVector* getEventListeners(const QualifiedName& eventType);

    bool addEventListener(const QualifiedName& eventType, EventListener* listener, bool useCapture = false);
    bool removeEventListener(const QualifiedName& eventType, EventListener* listener, bool useCapture = false);
    virtual bool dispatchEvent(Event* event);
    bool dispatchEvent(EventTarget* origin, Event* event);

    bool setAttributeEventListener(const QualifiedName& eventType, EventListener* listener);
    EventListener* getAttributeEventListener(const QualifiedName& eventType);
    bool clearAttributeEventListener(const QualifiedName& eventType);

protected:
    std::unordered_map<QualifiedName, EventListenerVector*, std::hash<QualifiedName>, std::equal_to<QualifiedName>,
        gc_allocator<std::pair<QualifiedName, EventListenerVector*>>> m_eventListeners;
};

}

#endif
