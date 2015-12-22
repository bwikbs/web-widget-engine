#include "StarFishConfig.h"
#include "StarFish.h"
#include "platform/message_loop/MessageLoop.h"
#include "platform/window/Window.h"

#include <Elementary.h>

namespace StarFish {

StarFish::StarFish()
{
    elm_init(0,0);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    m_messageLoop = new MessageLoop();
    m_window = Window::create(300,300);
}

void StarFish::run()
{
    m_messageLoop->run();
}


}
