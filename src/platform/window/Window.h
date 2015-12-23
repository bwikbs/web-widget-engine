#ifndef __StarFishWindow__
#define __StarFishWindow__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class StarFish;
class DocumentElement;

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

protected:
    Window(StarFish* starFish);
    void rendering();

    bool m_needsRendering;
    DocumentElement* m_document;

    StarFish* m_starFish;
};

}

#endif
