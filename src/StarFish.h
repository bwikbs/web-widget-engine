#ifndef __StarFish__
#define __StarFish__

namespace StarFish {

class MessageLoop;
class Window;
class ScriptBindingInstance;

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

protected:
    MessageLoop* m_messageLoop;
    ScriptBindingInstance* m_scriptBindingInstance;
    Window* m_window;
    String* m_currentPath;
};

}
#endif
