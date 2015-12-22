#ifndef __StarFish__
#define __StarFish__

namespace StarFish {

class MessageLoop;
class Window;

class StarFish : public gc {
public:
    StarFish();
    void run();

    Window* window()
    {
        return m_window;
    }

protected:
    MessageLoop* m_messageLoop;
    Window* m_window;
};

}
#endif
