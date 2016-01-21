#ifndef __Font__
#define __Font__

namespace StarFish {



enum FontStyle {
    FontStyleNormal = 1,
    FontStyleBold = 1 << 1,
    FontStyleItalic = 1 << 2,
    FontStyleUnderline = 1 << 3,
};

class Font : public gc {
    friend class FontSelector;
protected:
    Font()
    {

    }
public:
    virtual ~Font() {

    }

    virtual float measureText(String*) = 0;
    virtual void* unwrap() = 0;
    virtual float size() = 0;
    virtual String* familyName() = 0;

    struct FontMetrics {
        float m_ascender;
        float m_descender;
        float m_fontHeight;
        float m_lineGap;
    };
    const FontMetrics& metrics()
    {
        return m_metrics;
    }
protected:
    FontMetrics m_metrics;
};

class FontSelector {
protected:
    friend class StarFish;
    FontSelector()
    {

    }
    ~FontSelector()
    {

    }
    Font* loadFont(String* familyName,float size, int style);
    void clearCache()
    {
        m_fontCache.clear();
        m_fontCache.shrink_to_fit();
    }
public:
    std::vector<std::tuple<Font*, String*, float, int>, gc_allocator<std::tuple<Font*, String*, float, int>>> m_fontCache;
};

};
#endif
