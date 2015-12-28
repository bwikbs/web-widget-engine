#include "StarFishConfig.h"
#include "StarFish.h"
#include "platform/message_loop/MessageLoop.h"
#include "platform/window/Window.h"
#include "dom/binding/ScriptBindingInstance.h"

#include <Elementary.h>

namespace StarFish {

StarFish::StarFish()
{
    elm_init(0,0);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    m_messageLoop = new MessageLoop();
    m_scriptBindingInstance = new ScriptBindingInstance();
    m_scriptBindingInstance->initBinding(this);
    m_window = Window::create(this, 360, 360);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,[](void *data, int type, void *event) -> Eina_Bool {
        StarFish* sf = (StarFish*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->m_window->dispatchTouchEvent(d->x, d->y, Window::Down);
        return EINA_TRUE;
    }, this);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,[](void *data, int type, void *event) -> Eina_Bool {
        StarFish* sf = (StarFish*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->m_window->dispatchTouchEvent(d->x, d->y, Window::Up);
        return EINA_TRUE;
    }, this);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,[](void *data, int type, void *event) -> Eina_Bool {
        StarFish* sf = (StarFish*)data;
        Ecore_Event_Mouse_Move* d = (Ecore_Event_Mouse_Move*)event;
        sf->m_window->dispatchTouchEvent(d->x, d->y, Window::Move);
        return EINA_TRUE;
    } ,this);
}

void StarFish::run()
{
    m_messageLoop->run();
}


}
