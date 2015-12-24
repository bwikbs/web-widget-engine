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


protected:
    Window(StarFish* starFish);
    void rendering();

    bool m_needsRendering;
    DocumentElement* m_document;

    StarFish* m_starFish;

    uint32_t m_timeoutCounter;
    std::unordered_map<uint32_t, std::pair<WindowSetTimeoutHandler, void*> > m_timeoutHandler;
};

}

#endif
