#ifndef __StarFishWindow__
#define __StarFishWindow__


namespace StarFish {

class DocumentElement;

class Window : public gc {
public:
    static Window* create(size_t w = SIZE_MAX, size_t h = SIZE_MAX);
    void setNeedsRendering();

    DocumentElement* document()
    {
        return m_document;
    }

protected:
    Window();
    void rendering();

    bool m_needsRendering;
    DocumentElement* m_document;
};

}

#endif
