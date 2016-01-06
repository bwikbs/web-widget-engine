#include "StarFishConfig.h"
#include "StarFish.h"
#include "platform/message_loop/MessageLoop.h"
#include "platform/window/Window.h"
#include "dom/binding/ScriptBindingInstance.h"

#include <Elementary.h>

namespace StarFish {

#ifndef STARFISH_TIZEN_WEARABLE
StarFish::StarFish()
{
    elm_init(0,0);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    m_messageLoop = new MessageLoop();
    m_scriptBindingInstance = new ScriptBindingInstance();
    m_scriptBindingInstance->initBinding(this);
    m_window = Window::create(this, 360, 360);
}
#else
StarFish::StarFish(void* win)
{
    m_messageLoop = new MessageLoop();
    m_scriptBindingInstance = new ScriptBindingInstance();
    m_scriptBindingInstance->initBinding(this);
    m_window = Window::create(this, 360, 360, win);
}
#endif

void StarFish::run()
{
    m_messageLoop->run();
}

void StarFish::loadXMLDocument(String* filePath)
{
    m_window->loadXMLDocument(filePath);
}

}
