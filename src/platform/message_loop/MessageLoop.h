#ifndef __StarFishMessageLoop__
#define __StarFishMessageLoop__


namespace StarFish {

class MessageLoop : public gc {
    friend class StarFish;
    friend class Window;
public:
    MessageLoop()
    {

    }
protected:
    void run();
};

}

#endif
