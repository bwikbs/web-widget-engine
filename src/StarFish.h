#ifndef __StarFish__
#define __StarFish__

namespace StarFish {

class MessageLoop;
class Window;
class ScriptBindingInstance;

class StarFish : public gc {
public:
#ifndef STARFISH_TIZEN_WEARABLE
    StarFish();
#else
    StarFish(void* win);
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

    void loadXMLDocument(String* filePath);

protected:
    MessageLoop* m_messageLoop;
    ScriptBindingInstance* m_scriptBindingInstance;
    Window* m_window;
};

}
#endif
