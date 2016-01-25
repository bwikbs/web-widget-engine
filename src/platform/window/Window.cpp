#include "StarFishConfig.h"
#include "Window.h"

#include "dom/builder/XMLDocumentBuilder.h"
#include "dom/HTMLDocument.h"
#include "layout/FrameTreeBuilder.h"
#include "platform/canvas/font/Font.h"

#include "layout/Frame.h"

#include <Elementary.h>
#include <Evas_Engine_Buffer.h>
#include <Ecore_X.h>
#include <Ecore_Input.h>
#include <Ecore_Input_Evas.h>


#ifdef STARFISH_TIZEN_WEARABLE
#include <efl_extension.h>
#include <tizen.h>
#endif

#ifdef STARFISH_TIZEN_WEARABLE
EAPI Eina_Bool evas_render_async(Evas* obj);
#endif

extern Evas* g_internalCanvas;

namespace StarFish {

namespace
{
    class __GET_TICK_COUNT
    {
    public:
        __GET_TICK_COUNT()
        {
        if (gettimeofday(&tv_, NULL) != 0)
            throw 0;
        }
        timeval tv_;
    };
    __GET_TICK_COUNT timeStart;
}

static unsigned long getTickCount()
{
    static time_t   secStart    = timeStart.tv_.tv_sec;
    static time_t   usecStart   = timeStart.tv_.tv_usec;
                    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec - secStart) * 1000 + (tv.tv_usec - usecStart) / 1000;
}


class WindowImplEFL : public Window {
public:
    WindowImplEFL(StarFish* sf)
        : Window(sf)
    {
        m_lastRenderTime = 0;
    }

    virtual int width()
    {
        WindowImplEFL* eflWindow = (WindowImplEFL*)this;
        int width;
        evas_object_geometry_get(eflWindow->m_window, NULL, NULL, &width, NULL);
        return width;
    }

    virtual int height()
    {
        WindowImplEFL* eflWindow = (WindowImplEFL*)this;
        int height;
        evas_object_geometry_get(eflWindow->m_window, NULL, NULL, NULL, &height);
        return height;
    }

    uintptr_t m_handle;
    Evas_Object* m_window;
    Evas_Object* m_background;
    Evas_Object* m_canvasAdpater;
    std::vector<Evas_Object*> m_objectList;
    std::unordered_map<ImageData*, std::vector<std::pair<Evas_Object*, bool>>> m_drawnImageList;
    Evas_Object* m_dummyBox;
    unsigned long m_lastRenderTime;
};

void mainRenderingFunction(Evas_Object *o, Evas_Object_Box_Data *priv, void *user_data)
{
    WindowImplEFL* wnd= (WindowImplEFL*)user_data;
    wnd->setNeedsRendering();
}

#ifndef STARFISH_TIZEN_WEARABLE
Window* Window::create(StarFish* sf, size_t w, size_t h)
{
    auto wnd = new(GC) WindowImplEFL(sf);
    wnd->m_starFish = sf;
    wnd->m_window = elm_win_add(NULL, "", ELM_WIN_BASIC);

    Evas* e = evas_object_evas_get(wnd->m_window);
    g_internalCanvas = e;
    Ecore_Evas* ee = ecore_evas_ecore_evas_get(e);
    Ecore_Window ew = ecore_evas_window_get(ee);
    STARFISH_RELEASE_ASSERT(ew);
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

    evas_event_callback_add(e, EVAS_CALLBACK_RENDER_FLUSH_POST, [](void *data, Evas *e, void *event_info) {
        WindowImplEFL* eflWindow = (WindowImplEFL*)data;
        eflWindow->m_lastRenderTime  = getTickCount();
        STARFISH_LOG_INFO("GC heapSize...%f MB\n", GC_get_heap_size()/1024.f/1024.f);
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventDown);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventUp);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Move* d = (Ecore_Event_Mouse_Move*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventMove);
        return EINA_TRUE;
    } ,wnd);

    ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventDown);
        return EINA_TRUE;
    } ,wnd);

    ecore_event_handler_add(ECORE_EVENT_KEY_UP,[](void *data, int type, void *event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventUp);
        return EINA_TRUE;
    } ,wnd);

    return wnd;
}
#else
Window* Window::create(StarFish* sf, size_t w, size_t h, void* win)
{
    auto wnd = new(GC) WindowImplEFL(sf);
    wnd->m_starFish = sf;
    wnd->m_window = (Evas_Object*)win;

    Evas* e = evas_object_evas_get(wnd->m_window);
    g_internalCanvas = e;
    Evas_Object* mainBox = elm_box_add(wnd->m_window);
    evas_object_size_hint_weight_set (mainBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(wnd->m_window, mainBox);
    elm_box_layout_set(mainBox,mainRenderingFunction, wnd, NULL);
    evas_object_show(mainBox);

    wnd->m_dummyBox =  evas_object_rectangle_add(e);
    evas_object_color_set(wnd->m_dummyBox, 0, 0, 0, 0); // opaque background
    evas_object_resize(wnd->m_dummyBox, w, h);
    evas_object_show(wnd->m_dummyBox);
    evas_object_show(wnd->m_window);

    evas_object_event_callback_add(wnd->m_dummyBox , EVAS_CALLBACK_MOUSE_DOWN, [](void *data, Evas *evas, Evas_Object *obj, void *event_info) -> void {
        Window* sf = (Window*)data;
        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
        if (sf->m_isRunning)
            sf->dispatchTouchEvent(ev->canvas.x, ev->canvas.y, Window::TouchEventDown);
        return ;
    }, wnd);

    evas_object_event_callback_add(wnd->m_dummyBox , EVAS_CALLBACK_MOUSE_UP, [](void *data, Evas *evas, Evas_Object *obj, void *event_info) -> void {
        Window* sf = (Window*)data;
        Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event_info;
        if (sf->m_isRunning)
            sf->dispatchTouchEvent(ev->canvas.x, ev->canvas.y, Window::TouchEventUp);
        return ;
    }, wnd);

    evas_event_callback_add(e, EVAS_CALLBACK_RENDER_FLUSH_POST, [](void *data, Evas *e, void *event_info) {
        WindowImplEFL* eflWindow = (WindowImplEFL*)data;
        eflWindow->m_lastRenderTime  = getTickCount();
        STARFISH_LOG_INFO("GC heapSize...%f MB\n", GC_get_heap_size()/1024.f/1024.f);
    }, wnd);

    /*
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
    */

    return wnd;
}
#endif


Window::Window(StarFish* starFish)
    : m_starFish(starFish)
{
    STARFISH_ASSERT(m_starFish->scriptBindingInstance());

    CSSStyleSheet* userAgentStyleSheet = new CSSStyleSheet;

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("html"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "block"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("head"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "none"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("style"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "none"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("script"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "none"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("body"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "block"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("div"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "block"));
        userAgentStyleSheet->addRule(rule);
    }

    {
        CSSStyleRule rule(CSSStyleRule::Kind::TypeSelector, String::createASCIIString("img"), CSSStyleRule::PseudoClass::None);
        rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString("display", "inline"));
        userAgentStyleSheet->addRule(rule);
    }

    m_styleResolver.addSheet(userAgentStyleSheet);

    m_document = new HTMLDocument(this, m_starFish->scriptBindingInstance(), m_styleResolver.resolveDocumentStyle(m_starFish));

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableBlackTheme) {
        m_document->style()->setColor(Color(255, 255, 255, 255));
    }

    initScriptWrappableWindow(this);
    m_document->initScriptWrappable(m_document);
    m_timeoutCounter = 0;
    m_needsRendering = false;
    m_isRunning = true;
    m_activeNodeWithTouchDown = nullptr;
    setNeedsRendering();
}


void Window::rendering()
{
    if (!m_needsRendering)
        return;

    STARFISH_LOG_INFO("Window::rendering\n");

    // resolve style
    m_styleResolver.resolveDOMStyle(m_document);

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableComputedStyleDump) {
        // dump style
        m_styleResolver.dumpDOMStyle(m_document);
    }

    // create frame tree
    FrameTreeBuilder::buildFrameTree(m_document);

    // lay out frame tree
    LayoutContext ctx;
    m_document->frame()->layout(ctx);

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableFrameTreeDump) {
        FrameTreeBuilder::dumpFrameTree(m_document);
    }

    // painting
    WindowImplEFL* eflWindow = (WindowImplEFL*)this;
    int width, height;
    evas_object_geometry_get(eflWindow->m_window, NULL, NULL, &width, &height);

    /*
    if (!m_isRunning) {
        m_needsRendering = false;
        return;
    }
    */

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

    canvas->setColor(Color(0,0,0,255));

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableBlackTheme)
        canvas->clearColor(Color(0,0,0,255));
    else
        canvas->clearColor(Color(255,255,255,255));

    m_document->frame()->paint(canvas);

#ifdef STARFISH_TIZEN_WEARABLE
    /*
    canvas->save();
    canvas->setColor(Color(255,0,0,255));
    canvas->drawRect(Rect(180-20, 320, 40, 20));
    canvas->setColor(Color(255,255,255,255));
    String* txt = String::createASCIIString("SWC");
    Font* fnt = FontSelector::loadFont(String::createASCIIString(""), 12);
    Size siz = fnt->measureText(txt);
    canvas->setFont(fnt);
    canvas->drawText(180 - siz.width()/2, 324, txt);
    canvas->restore();
    */
#endif

    delete canvas;
    m_needsRendering = false;

    auto t = getTickCount();
    if (t - eflWindow->m_lastRenderTime > 33) {
        evas_render(evas);
    }
#ifdef STARFISH_TIZEN_WEARABLE
    evas_object_raise(eflWindow->m_dummyBox);
#endif

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
        auto a = td->m_window->m_timeoutHandler.find(td->m_id);
        a->second.first(td->m_window, a->second.second);
        td->m_window->m_timeoutHandler.erase(td->m_window->m_timeoutHandler.find(td->m_id));
        delete td;
        return ECORE_CALLBACK_DONE;
    }, td);

    return id;
}

Node* Window::hitTest(float x, float y)
{
    renderingIfNeeds();

    if (document() && document()->frame()) {
        Frame* frame = document()->frame()->hitTest(x, y);
        while (!frame->node()) {
            frame = frame->parent();
        }

        if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableHitTestDump) {
            printf("hitTest Result-> ");
            frame->node()->dump();
            puts("");
        }

        return frame->node();
    }

    return nullptr;
}

void Window::dispatchTouchEvent(float x, float y,TouchEventKind kind)
{
    Node* node = hitTest(x, y);
    if (kind == TouchEventDown) {
        m_activeNodeWithTouchDown = node;
        Node* t = node;
        while (t) {
            t->setState(Node::NodeStateActive);
            t = t->parentNode();
        }
   }

    if (kind == TouchEventUp) {
        if (m_activeNodeWithTouchDown == node) {
            node->callFunction(m_document->window()->starFish()->staticStrings()->m_onclick);
        }

        Node* t = m_activeNodeWithTouchDown;
        while (t) {
            t->setState(Node::NodeStateNormal);
            t = t->parentNode();
        }
        m_activeNodeWithTouchDown = nullptr;
    }
}

void Window::dispatchKeyEvent(String* key, KeyEventKind kind)
{
    if (kind == KeyEventDown) {
        // callFunction(String::createASCIIString("onKeyDown"));
    } else if (kind == KeyEventUp) {
        // callFunction(String::createASCIIString("onKeyUp"));
    }
}

void Window::pause()
{
    STARFISH_LOG_INFO("onPause");
    callFunction(String::createASCIIString("onpause"));
    m_isRunning = false;
}

void Window::resume()
{
    STARFISH_LOG_INFO("onResume");
    callFunction(String::createASCIIString("onresume"));
    m_isRunning = true;
    setNeedsRendering();
    WindowImplEFL* eflWindow = (WindowImplEFL*)this;

    auto a = eflWindow->m_drawnImageList.begin();
    while(a != eflWindow->m_drawnImageList.end()) {
        evas_object_unref((Evas_Object*)a->first->unwrap());
        a++;
    }
    eflWindow->m_drawnImageList.clear();
    eflWindow->m_lastRenderTime = 0;
    rendering();
}

}
