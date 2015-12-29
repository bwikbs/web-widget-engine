#include "StarFishConfig.h"
#include "Window.h"

#include "dom/DocumentElement.h"
#include "dom/builder/XMLDocumentBuilder.h"

#include <Elementary.h>
#include <Evas_Engine_Buffer.h>
#include <Ecore_X.h>
#include <Ecore_Input.h>
#include <Ecore_Input_Evas.h>
#include <efl_extension.h>

namespace StarFish {

class WindowImplEFL : public Window {
public:
    WindowImplEFL(StarFish* sf)
        : Window(sf)
    {

    }
    uintptr_t m_handle;
    Evas_Object* m_window;
    Evas_Object* m_background;
    Evas_Object* m_canvasAdpater;
    std::vector<Evas_Object*> m_objectList;
    std::unordered_map<ImageData*, std::vector<std::pair<Evas_Object*, bool>>> m_drawnImageList;
    Evas_Object* m_dummyBox;
};

void mainRenderingFunction(Evas_Object *o, Evas_Object_Box_Data *priv, void *user_data)
{
    WindowImplEFL* wnd= (WindowImplEFL*)user_data;
    wnd->setNeedsRendering();
}

Window* Window::create(StarFish* sf, size_t w, size_t h)
{
    auto wnd = new WindowImplEFL(sf);
    wnd->m_starFish = sf;
    wnd->m_window = elm_win_add(NULL, "", ELM_WIN_BASIC);

    Evas* e = evas_object_evas_get(wnd->m_window);
    Ecore_Evas* ee = ecore_evas_ecore_evas_get(e);
    Ecore_Window ew = ecore_evas_window_get(ee);
    RELEASE_ASSERT(ew);
    wnd->m_handle = (uintptr_t)ew;

    elm_win_autodel_set(wnd->m_window, EINA_TRUE);

    wnd->m_dummyBox = elm_box_add(wnd->m_window);
    evas_object_size_hint_weight_set (wnd->m_dummyBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(wnd->m_window, wnd->m_dummyBox);
    elm_box_layout_set(wnd->m_dummyBox,mainRenderingFunction,wnd,NULL);
    evas_object_show(wnd->m_dummyBox);

    if (w != SIZE_MAX && h != SIZE_MAX)
        evas_object_resize(wnd->m_window, (int)w, (int)h);
    evas_object_show(wnd->m_window);

    return wnd;
}


Window::Window(StarFish* starFish)
    : m_starFish(starFish)
{
    ASSERT(m_starFish->scriptBindingInstance());
    m_document = new DocumentElement(this, m_starFish->scriptBindingInstance());
    initScriptWrappableWindow(this);
    m_document->initScriptWrappable(m_document);
    m_timeoutCounter = 0;
    m_needsRendering = false;
    m_activeNodeWithTouchDown = nullptr;
    setNeedsRendering();


    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventDown);
        return EINA_TRUE;
    }, this);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventUp);
        return EINA_TRUE;
    }, this);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Move* d = (Ecore_Event_Mouse_Move*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventMove);
        return EINA_TRUE;
    } ,this);

    ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventDown);
        return EINA_TRUE;
    } ,this);

    ecore_event_handler_add(ECORE_EVENT_KEY_UP,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventUp);
        return EINA_TRUE;
    } ,this);

#ifdef STARFISH_TIZEN_WEARABLE
    WindowImplEFL* eflWindow = (WindowImplEFL*)this;
    eext_rotary_event_handler_add([](void *data, Eext_Rotary_Event_Info *info) -> Eina_Bool {
        Window* sf = (Window*)data;
        if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE) {
            sf->dispatchKeyEvent(String::createASCIIString("rotaryClockWise"), Window::KeyEventDown);
            sf->dispatchKeyEvent(String::createASCIIString("rotaryClockWise"), Window::KeyEventUp);
        } else {
            sf->dispatchKeyEvent(String::createASCIIString("rotaryCounterClockWise"), Window::KeyEventDown);
            sf->dispatchKeyEvent(String::createASCIIString("rotaryCounterClockWise"), Window::KeyEventUp);
        }
        return EINA_TRUE;
    }, this);
#endif
}

void Window::rendering()
{
    if (!m_needsRendering)
        return;

    m_document->computeStyle();

    WindowImplEFL* eflWindow = (WindowImplEFL*)this;
    int width, height;
    evas_object_geometry_get(eflWindow->m_window, NULL, NULL, &width, &height);
    m_document->mutableComputedRect().setX(0);
    m_document->mutableComputedRect().setY(0);
    m_document->mutableComputedRect().setWidth(width);
    m_document->mutableComputedRect().setHeight(height);
    m_document->computeLayout();

    Evas* evas = evas_object_evas_get(eflWindow->m_window);
    struct dummy {
        void* a;
        void* b;
        int w;
        int h;
        std::vector<Evas_Object*>* objList;
    };
    dummy* d = new dummy;
    d->a = evas;
    d->b = &eflWindow->m_drawnImageList;
    d->w = width;
    d->h = height;
    d->objList = &eflWindow->m_objectList;
    auto iter = eflWindow->m_objectList.begin();
    while(iter != eflWindow->m_objectList.end()) {
        evas_object_del(*iter);
        iter++;
    }
    eflWindow->m_objectList.clear();
    eflWindow->m_objectList.shrink_to_fit();
    Canvas* canvas = Canvas::createDirect(d);
    delete d;

    canvas->setColor(Color(255,255,255,255));
    canvas->clearColor(Color(0,0,0,255));

    m_document->paint(canvas);

    delete canvas;
    m_needsRendering = false;
}

void Window::setNeedsRendering()
{
    if (m_needsRendering) {
        return;
    }
    m_needsRendering = true;

    ecore_idler_add([](void *data) -> Eina_Bool {
        // FIXME
        // we pass window data as idler.
        // but, bdwgc is cannot see ecore's memory
        // we should add window as root set
        // or pointer live check
        Window* wnd = (Window*) data;
        wnd->rendering();

        return ECORE_CALLBACK_CANCEL;
    }, this);
}

void Window::renderingIfNeeds()
{
    if (m_needsRendering) {
        rendering();
        m_needsRendering = false;
    }
}

void Window::loadXMLDocument(String* filePath)
{
    XMLDocumentBuilder* builder = new XMLDocumentBuilder;
    builder->build(m_document, filePath);
}

struct TimeoutData {
    Window* m_window;
    uint32_t m_id;
};

uint32_t Window::setTimeout(WindowSetTimeoutHandler handler, uint32_t delay, void* data)
{
    TimeoutData* td = new TimeoutData;
    td->m_window = this;
    uint32_t id = m_timeoutCounter++;
    td->m_id = id;
    m_timeoutHandler.insert(std::make_pair(id, std::make_pair(handler, data)));

    // FIXME
    // instance of window is not rooted.
    // because timeoutdata is stored in memory area in ecore.
    // this implemention is very unsafe
    ecore_timer_add(delay/1000.0,[](void *data) -> Eina_Bool {
        TimeoutData* td = (TimeoutData*)data;
        auto a = td->m_window->m_timeoutHandler[td->m_id];
        a.first(td->m_window, a.second);
        td->m_window->m_timeoutHandler.erase(td->m_window->m_timeoutHandler.find(td->m_id));
        delete td;
        return ECORE_CALLBACK_DONE;
    }, td);

    return id;
}

Node* Window::hitTest(float x, float y)
{
    renderingIfNeeds();

    Node* node = m_document->hitTest(x, y);
    return node;
}

void Window::dispatchTouchEvent(float x, float y,TouchEventKind kind)
{
    Node* node = hitTest(x, y);
    if (kind == TouchEventDown) {
        m_activeNodeWithTouchDown = node;
    }
    while (node) {
        // TODO
        // translate x, y
        if (node->onTouchEvent(kind, x, y))
            break;
        node = node->parentElement();
    }
    if (kind == TouchEventUp)
        m_activeNodeWithTouchDown = nullptr;
}

void Window::dispatchKeyEvent(String* key, KeyEventKind kind)
{
    if (kind == KeyEventDown) {
        callFunction(String::createASCIIString("onKeyDown"), key);
    } else if (kind == KeyEventUp) {
        callFunction(String::createASCIIString("onKeyUp"), key);
    }

}

}
