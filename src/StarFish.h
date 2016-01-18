#ifndef __StarFish__
#define __StarFish__

namespace StarFish {

class MessageLoop;
class Window;
class ScriptBindingInstance;
class ImageData;

class StaticStrings {
public:
    StaticStrings();

    // Node Names
    String* m_documentLocalName;
    String* m_textLocalName;
    String* m_htmlLocalName;
    String* m_headLocalName;
    String* m_styleLocalName;
    String* m_scriptLocalName;
    String* m_bodyLocalName;
    String* m_divLocalName;
    String* m_imageLocalName;

    // Attribute Names
    String* m_id;
    String* m_class;
    String* m_src;

    // Event Names
    String* m_click;
    String* m_onclick;
};

class StarFish : public gc {
public:
#ifndef STARFISH_TIZEN_WEARABLE
    StarFish(String* currentPath = String::createASCIIString(""));
#else
    StarFish(String* currentPath, void* win);
#endif
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
        std::string p = m_currentPath->asASCIIString()->data();
        p.append(src->asASCIIString()->data());
        return String::createASCIIString(p.data());
    }

    void loadXMLDocument(String* filePath);

    void resume();
    void pause();
    void evaluate(String* s);

    ImageData* fetchImage(String* str);

    StaticStrings* staticStrings()
    {
        return &m_staticStrings;
    }
protected:
    MessageLoop* m_messageLoop;
    ScriptBindingInstance* m_scriptBindingInstance;
    Window* m_window;
    String* m_currentPath;
    std::unordered_map<std::string, ImageData*, std::hash<std::string>, std::equal_to<std::string>,
        gc_allocator<std::pair<std::string, ImageData*>>> m_imageCache;
    StaticStrings m_staticStrings;
};

}
#endif
