#ifndef __StarFishWindow__
#define __StarFishWindow__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class StarFish;
class DocumentElement;
class Window;

typedef void (*WindowSetTimeoutHandler)(Window* window, void* data);

class Window : public ScriptWrappable {
public:
    static Window* create(StarFish* sf, size_t w = SIZE_MAX, size_t h = SIZE_MAX);
    void setNeedsRendering();
    void renderingIfNeeds();

    DocumentElement* document()
    {
        return m_document;
    }

    StarFish* starFish()
    {
        return m_starFish;
    }

    void loadXMLDocument(String* filePath);
    uint32_t setTimeout(WindowSetTimeoutHandler handler, uint32_t delay, void* data);

    enum TouchEventKind {
        TouchEventDown,
        TouchEventMove,
        TouchEventUp
    };
    void dispatchTouchEvent(float x, float y, TouchEventKind kind);

    enum KeyEventKind {
        KeyEventDown,
        KeyEventUp
    };
    void dispatchKeyEvent(String* key, KeyEventKind kind);

    Node* hitTest(float x, float y);
    Node* activeNodeWithTouchDown()
    {
        return m_activeNodeWithTouchDown;
    }

protected:
    Window(StarFish* starFish);
    void rendering();

    bool m_needsRendering;
    DocumentElement* m_document;
    Node* m_activeNodeWithTouchDown;

    StarFish* m_starFish;

    uint32_t m_timeoutCounter;
    std::unordered_map<uint32_t, std::pair<WindowSetTimeoutHandler, void*> > m_timeoutHandler;
};

}

#endif
