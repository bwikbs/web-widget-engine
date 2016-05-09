#ifndef __StarFish__
#define __StarFish__

#include "util/AtomicString.h"
#include "util/QualifiedName.h"
#include "platform/canvas/font/Font.h"

namespace StarFish {

class MessageLoop;
class Window;
class ScriptBindingInstance;
class ImageData;


#define STARFISH_ENUM_HTML_TAG_NAMES(F) \
F(abbr) \
F(acronym) \
F(address) \
F(article) \
F(aside) \
F(b) \
F(basefont) \
F(bdo) \
F(bgsound) \
F(big) \
F(center) \
F(cite) \
F(code) \
F(command) \
F(dd) \
F(dfn) \
F(dt) \
F(em) \
F(figcaption) \
F(figure) \
F(footer) \
F(header) \
F(hgroup) \
F(i) \
F(kbd) \
F(layer) \
F(main) \
F(mark) \
F(nav) \
F(nobr) \
F(noframes) \
F(nolayer) \
F(plaintext) \
F(rp) \
F(s) \
F(samp) \
F(section) \
F(small) \
F(strike) \
F(strong) \
F(sub) \
F(sup) \
F(tt) \
F(u) \
F(var) \
F(a) \
F(area) \
F(audio) \
F(br) \
F(base) \
F(body) \
F(canvas) \
F(content) \
F(dl) \
F(datalist) \
F(details) \
F(dir) \
F(div) \
F(font) \
F(form) \
F(frame) \
F(frameset) \
F(hr) \
F(head) \
F(h1) \
F(h2) \
F(h3) \
F(h4) \
F(h5) \
F(h6) \
F(html) \
F(iframe) \
F(li) \
F(label) \
F(legend) \
F(map) \
F(marquee) \
F(menu) \
F(meta) \
F(meter) \
F(del) \
F(ins) \
F(ol) \
F(optgroup) \
F(option) \
F(p) \
F(param) \
F(pre) \
F(listing) \
F(xmp) \
F(progress) \
F(blockquote) \
F(q) \
F(shadow) \
F(source) \
F(span) \
F(caption) \
F(td) \
F(th) \
F(col) \
F(colgroup) \
F(table) \
F(tr) \
F(tbody) \
F(tfoot) \
F(thead) \
F(template) \
F(title) \
F(track) \
F(ul) \
F(img) \
F(image) \
F(video) \
F(dialog) \
F(button) \
F(fieldset) \
F(keygen) \
F(output) \
F(select) \
F(textarea) \
F(applet) \
F(embed) \
F(link) \
F(script) \
F(style) \
F(input) \
F(object) \
F(bdi) \
F(noembed) \
F(noscript) \
F(rt) \
F(ruby) \
F(summary) \
F(wbr)

class StaticStrings : public gc {
    friend class QualifiedName;
    friend class AtomicString;
public:
    StaticStrings(StarFish* sf);
    StarFish* m_starFish;
    AtomicString m_xhtmlNamespaceURI;
    AtomicString m_documentLocalName;
    AtomicString m_documentFragmentLocalName;
    AtomicString m_textLocalName;
    AtomicString m_commentLocalName;

    // HTML Tag Names
#define DEFINE_HTML_LOCAL_NAMES(name) \
    QualifiedName m_##name##TagName;
    STARFISH_ENUM_HTML_TAG_NAMES(DEFINE_HTML_LOCAL_NAMES)
#undef DEFINE_HTML_LOCAL_NAMES

    // Attribute Names
    QualifiedName m_id;
    QualifiedName m_class;
    QualifiedName m_localName;
    QualifiedName m_style;
    QualifiedName m_src;
    QualifiedName m_width;
    QualifiedName m_height;
    QualifiedName m_rel;
    QualifiedName m_href;
    QualifiedName m_type;
    QualifiedName m_dir;
    QualifiedName m_color;
    QualifiedName m_face;
    QualifiedName m_size;

    // Event Names
    QualifiedName m_click;
    QualifiedName m_onclick;
    QualifiedName m_load;
    QualifiedName m_onload;
    QualifiedName m_unload;
    QualifiedName m_onunload;
    QualifiedName m_visibilitychange;
protected:
};

enum StarFishStartUpFlag {
    enableBlackTheme = 1,
    enableComputedStyleDump = 1 << 1,
    enableFrameTreeDump = 1 << 2,
    enableStackingContextDump = 1 << 3,
    enableHitTestDump = 1 << 4,
    enableRegressionTest = 1 << 5,
};


enum StarFishDeviceKind {
    deviceKindUseMouse = 0,
    deviceKindUseTouchScreen = 1 << 0,
};

class StarFish : public gc_cleanup {
    friend class AtomicString;
    friend class StaticStrings;
public:
#ifndef STARFISH_TIZEN_WEARABLE_APP
    StarFish(StarFishStartUpFlag flag, String* currentPath, const char* locale, int w, int h);
#else
    StarFish(StarFishStartUpFlag flag, String* currentPath, const char* locale, void* win, int w, int h);
#endif
    ~StarFish()
    {
        delete m_lineBreaker;
    }
    void run();

    Window* window()
    {
        return m_window;
    }

    ScriptBindingInstance* scriptBindingInstance()
    {
        return m_scriptBindingInstance;
    }

    String* currentPath()
    {
        return m_currentPath;
    }

    String* makeResourcePath(String* src)
    {
        std::string p = m_currentPath->utf8Data();
        std::string strSrc = src->utf8Data();

        if (posPrefix(strSrc, "file://") != std::string::npos)
            p = strSrc.substr(7);
        else if (posPrefix(strSrc, "blob:") != std::string::npos)
            p = strSrc;
        else
            p.append(strSrc);

        return String::fromUTF8(p.data());
    }

    void loadPreprocessedXMLDocument(String* filePath);
    void loadHTMLDocument(String* filePath);

    void resume();
    void pause();
    void close();
    void evaluate(String* s);

    ImageData* fetchImage(String* str);

    Font* fetchFont(String* familyName, float size, char style = FontStyle::FontStyleNormal, char weight = FontWeight::FontWeightNormal)
    {
        Font* f = nullptr;
        f = m_fontSelector.loadFont(familyName, size, style, weight);
        return f;
    }

    StaticStrings* staticStrings()
    {
        return m_staticStrings;
    }

    MessageLoop* messageLoop()
    {
        return m_messageLoop;
    }

    StarFishStartUpFlag startUpFlag()
    {
        return (StarFishStartUpFlag)m_startUpFlag;
    }

    StarFishDeviceKind deviceKind()
    {
        return m_deviceKind;
    }

    const icu::Locale& locale()
    {
        return m_locale;
    }

    icu::BreakIterator* lineBreaker()
    {
        return m_lineBreaker;
    }

    void addPointerInRootSet(void *ptr);
    void removePointerFromRootSet(void *ptr);

protected:
    void init(int w, int h);
    size_t posPrefix(std::string str, std::string prefix)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str.find(prefix);
    }

    StaticStrings* m_staticStrings;
    icu::Locale m_locale;
    icu::BreakIterator* m_lineBreaker;
    unsigned int m_startUpFlag;
    StarFishDeviceKind m_deviceKind;
    MessageLoop* m_messageLoop;
    ScriptBindingInstance* m_scriptBindingInstance;
    Window* m_window;
    String* m_currentPath;
    std::unordered_map<std::string, ImageData*, std::hash<std::string>, std::equal_to<std::string>,
        gc_allocator<std::pair<std::string, ImageData*>>> m_imageCache;
    FontSelector m_fontSelector;
    std::unordered_map<void*, size_t, std::hash<void*>, std::equal_to<void*>,
        gc_allocator<std::pair<void*, size_t>>> m_rootMap;

    std::unordered_map<std::string, AtomicString,
        std::hash<std::string>, std::equal_to<std::string>, gc_allocator<std::pair<std::string, AtomicString>>> m_atomicStringMap;
};

#ifdef STARFISH_ENABLE_PIXEL_TEST
extern bool g_enablePixelTest;
#endif

}
#endif
