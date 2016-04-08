#include "StarFishConfig.h"
#include "Window.h"

#include "dom/builder/XMLDocumentBuilder.h"
#include "dom/HTMLDocument.h"
#include "layout/FrameTreeBuilder.h"
#include "platform/canvas/font/Font.h"
#include "platform/message_loop/MessageLoop.h"

#include "layout/Frame.h"
#include "layout/FrameBox.h"

#include <Elementary.h>
#include <Evas_Engine_Buffer.h>
#include <Ecore_X.h>
#include <Ecore_Input.h>
#include <Ecore_Input_Evas.h>

#ifdef STARFISH_TIZEN_WEARABLE
#include <efl_extension.h>
#include <tizen.h>
#endif

#ifndef STARFISH_TIZEN_WEARABLE
extern __thread Evas* g_internalCanvas;
#else
extern Evas* g_internalCanvas;
#endif

#ifdef STARFISH_ENABLE_PIXEL_TEST

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

int writeImage(const char* filename, int width, int height, void *buffer)
{
    int code = 0;
    FILE* fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    uint32_t* buf = (uint32_t*)buffer;

    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
        8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    for (int y = 0; y < height; y ++) {
        png_write_row(png_ptr, (unsigned char*)&buf[width * y]);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL)
        fclose(fp);
    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL)
        free(row);

    return code;
}
#endif


namespace StarFish {

Evas* internalCanvas();
void initInternalCanvas();

namespace {
    class __GET_TICK_COUNT {
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

class WindowImplEFL : public Window {
public:
    WindowImplEFL(StarFish* sf)
        : Window(sf)
    {
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
    std::vector<Evas_Object*> m_surfaceList;
    std::unordered_map<ImageData*, std::vector<std::pair<Evas_Object*, bool> > > m_drawnImageList;
    Evas_Object* m_dummyBox;
};

class CanvasSurfaceEFL : public CanvasSurface {
public:
    CanvasSurfaceEFL(Window* wnd, size_t w, size_t h)
    {
        m_window = (WindowImplEFL*)wnd;
        m_image = evas_object_image_add(evas_object_evas_get(m_window->m_window));
        evas_object_image_size_set(m_image, w, h);
        evas_object_image_filled_set(m_image, EINA_TRUE);
        evas_object_image_colorspace_set(m_image, Evas_Colorspace::EVAS_COLORSPACE_ARGB8888);
        evas_object_image_alpha_set(m_image, EINA_TRUE);
        evas_object_anti_alias_set(m_image, EINA_TRUE);
        STARFISH_RELEASE_ASSERT(evas_object_image_colorspace_get(m_image) == EVAS_COLORSPACE_ARGB8888);
        m_width = w;
        m_height = h;
        // STARFISH_LOG_INFO("create CanvasSurfaceEFL %p\n", m_image);

        STARFISH_ASSERT(evas_object_visible_get(m_image) == EINA_FALSE);
    }

    ~CanvasSurfaceEFL()
    {
        // STARFISH_LOG_INFO("release CanvasSurfaceEFL %p\n", m_image);
        evas_object_hide(m_image);
        STARFISH_RELEASE_ASSERT(evas_object_ref_get(m_image) == 0);
        evas_object_del(m_image);

        auto iter = std::find(m_window->m_surfaceList.begin(), m_window->m_surfaceList.end(), m_image);
        if (m_window->m_surfaceList.end() != iter)
            m_window->m_surfaceList.erase(iter);
    }

    virtual void* unwrap()
    {
        return m_image;
    }

    virtual size_t width()
    {
        return m_width;
    }

    virtual size_t height()
    {
        return m_height;
    }

    virtual void clear()
    {
        void* address = evas_object_image_data_get(m_image, EINA_TRUE);
        size_t end = m_width * m_height * sizeof(uint32_t);
        memset(address, 0, end);
        evas_object_image_data_set(m_image, address);
    }

protected:
    WindowImplEFL* m_window;
    Evas_Object* m_image;
    size_t m_width;
    size_t m_height;
};

CanvasSurface* CanvasSurface::create(Window* wnd, size_t w, size_t h)
{
    return new CanvasSurfaceEFL(wnd, w, h);
}

void mainRenderingFunction(Evas_Object* o, Evas_Object_Box_Data* priv, void* user_data)
{
    WindowImplEFL* wnd = (WindowImplEFL*)user_data;
    if (!wnd->inRendering())
        wnd->setNeedsLayout();
}

#ifndef STARFISH_TIZEN_WEARABLE
Window* Window::create(StarFish* sf, size_t w, size_t h)
{
    Evas_Object* wndObj = elm_win_add(NULL, "StarFish", ELM_WIN_BASIC);
    initInternalCanvas();
    auto wnd = new WindowImplEFL(sf);
    wnd->m_starFish = sf;
    wnd->m_window = wndObj;
    elm_win_title_set(wnd->m_window, "StarFish");

    Evas* e = evas_object_evas_get(wnd->m_window);
    Ecore_Evas* ee = ecore_evas_ecore_evas_get(e);
    Ecore_Window ew = ecore_evas_window_get(ee);
    wnd->m_handle = (uintptr_t)ew;

    elm_win_autodel_set(wnd->m_window, EINA_TRUE);

    wnd->m_dummyBox = elm_box_add(wnd->m_window);
    evas_object_size_hint_weight_set(wnd->m_dummyBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(wnd->m_window, wnd->m_dummyBox);
    elm_box_layout_set(wnd->m_dummyBox, mainRenderingFunction, wnd, NULL);
    evas_object_show(wnd->m_dummyBox);

    if (w != SIZE_MAX && h != SIZE_MAX)
        evas_object_resize(wnd->m_window, (int)w, (int)h);
#ifdef STARFISH_ENABLE_PIXEL_TEST
    {
        const char* path = getenv("SCREEN_SHOT");
        const char* hide = getenv("HIDE_WINDOW");
        if ((path && strlen(path)) || (hide && strlen(hide)))
            evas_object_hide(wnd->m_window);
        else
            evas_object_show(wnd->m_window);
    }
#else
    evas_object_show(wnd->m_window);
#endif
    /*
    evas_event_callback_add(e, EVAS_CALLBACK_RENDER_FLUSH_POST, [](void *data, Evas *e, void *event_info) {
    }, wnd);
*/
    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, [](void* data, int type, void* event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventDown);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, [](void* data, int type, void* event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Button* d = (Ecore_Event_Mouse_Button*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventUp);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, [](void* data, int type, void* event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Mouse_Move* d = (Ecore_Event_Mouse_Move*)event;
        sf->dispatchTouchEvent(d->x, d->y, Window::TouchEventMove);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, [](void* data, int type, void* event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventDown);
        return EINA_TRUE;
    }, wnd);

    ecore_event_handler_add(ECORE_EVENT_KEY_UP, [](void* data, int type, void* event) -> Eina_Bool {
        Window* sf = (Window*)data;
        Ecore_Event_Key* d = (Ecore_Event_Key*)event;
        sf->dispatchKeyEvent(String::createASCIIString(d->keyname), Window::KeyEventUp);
        return EINA_TRUE;
    }, wnd);

    return wnd;
}
#else
Window* Window::create(StarFish* sf, size_t w, size_t h, void* win)
{
    g_internalCanvas = evas_object_evas_get((Evas_Object*)win);
    auto wnd = new WindowImplEFL(sf);
    wnd->m_starFish = sf;
    wnd->m_window = (Evas_Object*)win;

    Evas* e = evas_object_evas_get(wnd->m_window);
    Evas_Object* mainBox = elm_box_add(wnd->m_window);
    evas_object_size_hint_weight_set(mainBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(wnd->m_window, mainBox);
    elm_box_layout_set(mainBox, mainRenderingFunction, wnd, NULL);
    evas_object_show(mainBox);

    wnd->m_dummyBox = evas_object_rectangle_add(e);
    evas_object_color_set(wnd->m_dummyBox, 0, 0, 0, 0); // opaque background
    evas_object_resize(wnd->m_dummyBox, w, h);
    evas_object_show(wnd->m_dummyBox);
    evas_object_show(wnd->m_window);

    evas_object_event_callback_add(wnd->m_dummyBox, EVAS_CALLBACK_MOUSE_DOWN, [](void* data, Evas* evas, Evas_Object* obj, void* event_info) -> void {
        Window* sf = (Window*)data;
        Evas_Event_Mouse_Down* ev = (Evas_Event_Mouse_Down*) event_info;
        sf->dispatchTouchEvent(ev->canvas.x, ev->canvas.y, Window::TouchEventDown);
        return;
    }, wnd);

    evas_object_event_callback_add(wnd->m_dummyBox, EVAS_CALLBACK_MOUSE_MOVE, [](void* data, Evas* evas, Evas_Object* obj, void* event_info) -> void {
        Window* sf = (Window*)data;
        Evas_Event_Mouse_Move* ev = (Evas_Event_Mouse_Move*) event_info;
        sf->dispatchTouchEvent(ev->cur.canvas.x, ev->cur.canvas.y, Window::TouchEventMove);
        return;
    }, wnd);

    evas_object_event_callback_add(wnd->m_dummyBox, EVAS_CALLBACK_MOUSE_UP, [](void* data, Evas* evas, Evas_Object* obj, void* event_info) -> void {
        Window* sf = (Window*)data;
        Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*) event_info;
        sf->dispatchTouchEvent(ev->canvas.x, ev->canvas.y, Window::TouchEventUp);
        return;
    }, wnd);

    evas_event_callback_add(e, EVAS_CALLBACK_RENDER_FLUSH_POST, [](void* data, Evas* e, void* event_info) {
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
    : m_touchDownPoint(0, 0)
    , m_starFish(starFish)
{
    STARFISH_ASSERT(m_starFish->scriptBindingInstance());

    m_document = new HTMLDocument(this, m_starFish->scriptBindingInstance());
    initScriptWrappable(this);
    m_document->initScriptWrappable(m_document);
    m_timeoutCounter = 0;

    m_needsRendering = false;
    m_needsStyleRecalc = true;
    m_needsStyleRecalcForWholeDocument = false;
    m_needsFrameTreeBuild = true;
    m_needsLayout = true;
    m_needsPainting = true;

    m_hasRootElementBackground = false;
    m_hasBodyElementBackground = false;
    m_isRunning = true;
    m_activeNodeWithTouchDown = nullptr;

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableBlackTheme) {
        m_document->style()->setColor(Color(255, 255, 255, 255));
    }

    setNeedsRendering();
}

// #define STARFISH_ENABLE_TIMER

#ifdef STARFISH_ENABLE_TIMER
static unsigned long getLongTickCount()
{
    static time_t secStart = timeStart.tv_.tv_sec;
    static time_t usecStart = timeStart.tv_.tv_usec;
    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec - secStart) * 1000 + (tv.tv_usec - usecStart);
}
#endif

class Timer {
public:
    Timer(const char* msg)
    {
#ifdef STARFISH_ENABLE_TIMER
        m_start = getLongTickCount();
        m_msg = msg;
#endif
    }
    ~Timer()
    {
#ifdef STARFISH_ENABLE_TIMER
        unsigned long end = getLongTickCount();
        STARFISH_LOG_INFO("did %s in %f ms\n", m_msg, (end - m_start) / 1000.f);
        fflush(stdout);
#endif
    }

protected:
#ifdef STARFISH_ENABLE_TIMER
    unsigned long m_start;
    const char* m_msg;
#endif
};

#ifdef STARFISH_ENABLE_PIXEL_TEST
static Evas_Object* g_imgBufferForScreehShot;
#endif

Canvas* preparePainting(WindowImplEFL* eflWindow)
{
#ifdef STARFISH_ENABLE_PIXEL_TEST
    {
        const char* path = getenv("SCREEN_SHOT");
        if (path && strlen(path)) {
            auto s = CanvasSurface::create(eflWindow, eflWindow->width(), eflWindow->height());
            g_imgBufferForScreehShot = (Evas_Object*)s->unwrap();
            return Canvas::create(s);
        }
    }
#endif
    int width, height;
    evas_object_geometry_get(eflWindow->m_window, NULL, NULL, &width, &height);
    Evas* evas = evas_object_evas_get(eflWindow->m_window);
    struct dummy {
        void* a;
        void* b;
        int w;
        int h;
        std::vector<Evas_Object*>* objList;
        std::vector<Evas_Object*>* surfaceList;
    };
    dummy* d = new dummy;
    d->a = evas;
    d->b = &eflWindow->m_drawnImageList;
    d->w = width;
    d->h = height;
    d->objList = &eflWindow->m_objectList;
    d->surfaceList = &eflWindow->m_surfaceList;
    auto iter = eflWindow->m_objectList.begin();
    while (iter != eflWindow->m_objectList.end()) {
        evas_object_del(*iter);
        iter++;
    }
    eflWindow->m_objectList.clear();
    eflWindow->m_objectList.shrink_to_fit();

    iter = eflWindow->m_surfaceList.begin();
    while (iter != eflWindow->m_surfaceList.end()) {
        evas_object_hide(*iter);
        iter++;
    }
    eflWindow->m_surfaceList.clear();
    eflWindow->m_surfaceList.shrink_to_fit();

    Canvas* canvas = Canvas::createDirect(d);
    delete d;

    return canvas;
}

void Window::paintWindowBackground(Canvas* canvas)
{
    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableBlackTheme)
        canvas->clearColor(Color(0, 0, 0, 255));
    else
        canvas->clearColor(Color(255, 255, 255, 255));

    if (m_hasRootElementBackground || m_hasBodyElementBackground) {
        WindowImplEFL* eflWindow = (WindowImplEFL*)this;
        int width, height;
        evas_object_geometry_get(eflWindow->m_window, NULL, NULL, &width, &height);
        LayoutRect rt(0, 0, width, height);
        if (m_hasRootElementBackground) {
            FrameBox::paintBackground(canvas, document()->rootElement()->style(), rt, rt);
        } else {
            FrameBox::paintBackground(canvas, document()->rootElement()->body()->style(), rt, rt);
        }

    }
}



void Window::rendering()
{
    if (!m_needsRendering)
        return;
    m_inRendering = true;
    WindowImplEFL* eflWindow = (WindowImplEFL*)this;

    Timer renderingTimer("Window::rendering");

    if (m_needsStyleRecalc || m_needsStyleRecalcForWholeDocument) {
        // resolve style
        Timer t("resolve style");
        document()->styleResolver()->resolveDOMStyle(m_document, m_needsStyleRecalcForWholeDocument);
        m_needsStyleRecalc = false;

        if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableComputedStyleDump) {
            // dump style
            document()->styleResolver()->dumpDOMStyle(m_document);
        }
    }

    bool shouldClearStackingContext = true;
    if (m_needsFrameTreeBuild) {

        if (m_document->frame()) {
            m_document->frame()->asFrameBox()->iterateChildBoxes([](FrameBox* box) {
                box->clearStackingContextIfNeeds();
            });
            shouldClearStackingContext = false;
        }

        // create frame tree
        Timer t("create frame tree");
        FrameTreeBuilder::buildFrameTree(m_document);
        m_needsFrameTreeBuild = false;
    }

    if (m_needsLayout) {
        // lay out frame tree
        Timer t("lay out frame tree");

        if (shouldClearStackingContext) {
            m_document->frame()->asFrameBox()->iterateChildBoxes([](FrameBox* box) {
                box->clearStackingContextIfNeeds();
            });
        }

        m_document->frame()->asFrameBox()->iterateChildBoxes([](FrameBox* box) {
            box->clearStackingContextIfNeeds();
        });

        LayoutContext ctx(starFish());
        m_document->frame()->layout(ctx);
        {
            Timer t("computeStackingContextProperties");
            m_document->frame()->asFrameBox()->iterateChildBoxes([](FrameBox* box) {
                box->establishesStackingContextIfNeeds();
            });
            m_document->frame()->firstChild()->asFrameBox()->stackingContext()->computeStackingContextProperties();
        }
        m_needsLayout = false;
    }

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableFrameTreeDump) {
        FrameTreeBuilder::dumpFrameTree(m_document);
    }

    if (m_starFish->startUpFlag() & StarFishStartUpFlag::enableStackingContextDump) {
        STARFISH_ASSERT(m_document->frame()->firstChild()->asFrameBox()->isRootElement());
        StackingContext* ctx = m_document->frame()->firstChild()->asFrameBox()->stackingContext();

        std::function<void(StackingContext*, int)> dumpSC = [&dumpSC](StackingContext* ctx, int depth)
        {
            for (int i = 0; i < depth; i ++) {
                printf("  ");
            }

            auto fr = ctx->owner()->visibleRect();


            std::string className;
            for (unsigned i = 0; i < ctx->owner()->node()->asElement()->classNames().size(); i++) {
                className += ctx->owner()->node()->asElement()->classNames()[i]->utf8Data();
                className += " ";
            }

            printf("StackingContext[%p, node %p %s id:%s className:%s , frame %p, buf %d, %d %d %d %d]\n",
                ctx, ctx->owner()->node(), ctx->owner()->node()->localName()->utf8Data(), ctx->owner()->node()->asElement()->id()->utf8Data(), className.data(), ctx->owner(), (int)ctx->needsOwnBuffer()
                , (int)fr.x(), (int)fr.y(), (int)fr.width(), (int)fr.height());

            auto iter = ctx->childContexts().begin();
            while (iter != ctx->childContexts().end()) {

                int32_t num = iter->first;

                for (int i = 0; i < depth + 1; i ++) {
                    printf("  ");
                }

                printf("z-index: %d\n", (int)num);

                auto iter2 = iter->second->begin();
                while (iter2 != iter->second->end()) {
                    dumpSC(*iter2, depth + 2);
                    iter2++;
                }

                iter++;
            }
        };

        dumpSC(ctx, 0);
    }

    if (m_needsPainting) {
        Timer t("painting");

        // painting
        Canvas* canvas = preparePainting(eflWindow);

        paintWindowBackground(canvas);
        m_document->frame()->paint(canvas, PaintingStageEnd);
        m_needsPainting = false;
        m_needsComposite = m_document->frame()->firstChild()->asFrameBox()->stackingContext()->needsOwnBuffer();

        delete canvas;
#ifdef STARFISH_TIZEN_WEARABLE
        evas_object_raise(eflWindow->m_dummyBox);
#endif
    }

    if (m_needsComposite) {
        Timer t("composite");
        if (m_document->frame()->firstChild()->asFrameBox()->stackingContext()->needsOwnBuffer()) {
            Canvas* canvas = preparePainting(eflWindow);
            paintWindowBackground(canvas);
            m_document->frame()->firstChild()->asFrameBox()->stackingContext()->compositeStackingContext(canvas);

            delete canvas;
#ifdef STARFISH_TIZEN_WEARABLE
            evas_object_raise(eflWindow->m_dummyBox);
#endif
        }
        m_needsComposite = false;
    }

#ifdef STARFISH_TIZEN_WEARABLE
/*
    canvas->save();
    canvas->setColor(Color(255, 0, 0, 255));
    canvas->drawRect(Rect(180-20, 320, 40, 20));
    canvas->setColor(Color(255, 255, 255, 255));
    String* txt = String::createASCIIString("SWC");
    Font* fnt = FontSelector::loadFont(String::createASCIIString(""), 12);
    Size siz = fnt->measureText(txt);
    canvas->setFont(fnt);
    canvas->drawText(180 - siz.width()/2, 324, txt);
    canvas->restore();
    */
#endif

    m_needsRendering = false;
    m_inRendering = false;

#ifdef STARFISH_ENABLE_PIXEL_TEST
    {
        const char* path = getenv("SCREEN_SHOT");
        if (path && strlen(path)) {
            evas_object_image_save(g_imgBufferForScreehShot, path, NULL, NULL);
            // int writeImage(char* filename, int width, int height, void *buffer)
            // writeImage(path, width(), height(), evas_object_image_data_get(g_imgBufferForScreehShot, EINA_FALSE));
            exit(0);
        }
    }

#endif
}

void Window::setNeedsRenderingSlowCase()
{
    STARFISH_ASSERT(!m_needsRendering);
    m_needsRendering = true;

    ecore_animator_add([](void* data) -> Eina_Bool {
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

void Window::setWholeDocumentNeedsStyleRecalc()
{
    m_needsStyleRecalcForWholeDocument = true;
    setNeedsRendering();
}

void Window::loadXMLDocument(String* filePath)
{
    XMLDocumentBuilder* builder = new XMLDocumentBuilder;
    builder->build(m_document, filePath);
    dispatchLoadEvent();
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
    ecore_timer_add(delay / 1000.0, [](void* data) -> Eina_Bool {
        TimeoutData* td = (TimeoutData*)data;
        auto a = td->m_window->m_timeoutHandler.find(td->m_id);

        if (a->second.second != nullptr)
            a->second.first(td->m_window, a->second.second);

        td->m_window->m_timeoutHandler.erase(td->m_window->m_timeoutHandler.find(td->m_id));
        delete td;
        return ECORE_CALLBACK_DONE;
    }, td);

    return id;
}

void Window::clearTimeout(uint32_t id)
{
    // TODO : Use ecore_timer_del(Ecore_Timer *timer)
    auto handlerData = m_timeoutHandler.find(id);
    if (handlerData != m_timeoutHandler.end())
        handlerData->second.second = nullptr;
}

uint32_t Window::setInterval(WindowSetTimeoutHandler handler, uint32_t delay, void* data)
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
    ecore_timer_add(delay / 1000.0, [](void* data) -> Eina_Bool {
        TimeoutData* td = (TimeoutData*)data;
        auto a = td->m_window->m_timeoutHandler.find(td->m_id);

        if (a->second.second != nullptr)
            a->second.first(td->m_window, a->second.second);
        return ECORE_CALLBACK_RENEW;
    }, td);

    return id;
}

void Window::clearInterval(uint32_t id)
{
    // TODO : Use ecore_timer_del(Ecore_Timer *timer)
    auto handlerData = m_timeoutHandler.find(id);
    if (handlerData != m_timeoutHandler.end())
        handlerData->second.second = nullptr;
}

uint32_t Window::requestAnimationFrame(WindowSetTimeoutHandler handler, void* data)
{
    TimeoutData* td = new TimeoutData;
    td->m_window = this;
    uint32_t id = m_requestAnimationFrameCounter++;
    td->m_id = id;
    m_requestAnimationFrameHandler.insert(std::make_pair(id, std::make_pair(handler, data)));

    ecore_animator_add([](void* data) -> Eina_Bool {
        TimeoutData* td = (TimeoutData*)data;
        auto a = td->m_window->m_requestAnimationFrameHandler.find(td->m_id);

        if (a->second.second != nullptr)
            a->second.first(td->m_window, a->second.second);

        td->m_window->m_requestAnimationFrameHandler.erase(td->m_window->m_requestAnimationFrameHandler.find(td->m_id));
        delete td;
        return ECORE_CALLBACK_DONE;
    }, td);

    return id;
}

void Window::cancelAnimationFrame(uint32_t reqID)
{
    auto handlerData = m_requestAnimationFrameHandler.find(reqID);

    if (handlerData != m_requestAnimationFrameHandler.end())
        handlerData->second.second = nullptr;
}

Node* Window::hitTest(float x, float y)
{
    renderingIfNeeds();

    if (document() && document()->frame()) {
        Frame* frame = document()->frame()->hitTest(x, y, HitTestStageEnd);
        if (!frame)
            return nullptr;

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

void Window::setActiveNode(Node* n)
{
    Node* t = n;
    while (t) {
        t->setState(Node::NodeStateActive);
        m_activeNodes.push_back(t);
        t = t->parentNode();
    }
    m_activeNodeWithTouchDown = n;
}

void Window::releaseActiveNode()
{
    for (size_t i = 0; i < m_activeNodes.size() ; i ++) {
        m_activeNodes[i]->setState(Node::NodeStateNormal);
    }
    m_activeNodes.clear();
    m_activeNodes.shrink_to_fit();
    m_activeNodeWithTouchDown = nullptr;
}

void Window::dispatchTouchEvent(float x, float y, TouchEventKind kind)
{
    // STARFISH_LOG_INFO("Window::dispatchTouchEvent %f %f kind %d\n", x, y, (int)kind);
    if (!m_isRunning)
        return;

    if (kind == TouchEventDown) {
        Node* node = hitTest(x, y);
        m_touchDownPoint = Location(x, y);
        setActiveNode(node);
    } else if (kind == TouchEventMove) {
        if ((starFish()->deviceKind() & deviceKindUseTouchScreen) && m_activeNodeWithTouchDown && ((abs(m_touchDownPoint.x() - x) > 30) || (abs(m_touchDownPoint.y() - y) > 30))) {
            releaseActiveNode();
            m_activeNodeWithTouchDown = nullptr;
        }
    } else {
        STARFISH_ASSERT(kind == TouchEventUp);
        bool shouldCallOnClick = false;
        Node* node = hitTest(x, y);
        if (m_activeNodeWithTouchDown == node) {
            shouldCallOnClick = true;
        }

        Node* t = m_activeNodeWithTouchDown;

        bool shouldDispatchEvent = shouldCallOnClick;
        while (t) {
            if (shouldDispatchEvent && (t->isElement() && t->asElement()->isHTMLElement())) {
                String* eventType = starFish()->staticStrings()->m_click.string();
                Event* e = new Event(eventType, EventInit(true, true));
                EventTarget::dispatchEvent(t->asNode(), e);
                shouldDispatchEvent = false;
            }
            t = t->parentNode();
        }

        releaseActiveNode();

        m_activeNodeWithTouchDown = nullptr;
    }
}

void Window::dispatchKeyEvent(String* key, KeyEventKind kind)
{
}

void Window::dispatchLoadEvent()
{
    starFish()->messageLoop()->addIdler([](void* data) {
        Window* wnd = (Window*)data;
        String* eventType = wnd->starFish()->staticStrings()->m_load.string();
        Event* e = new Event(eventType, EventInit(false, false));
        wnd->EventTarget::dispatchEvent(wnd->document()->bodyElement(), e);
    }, this);
}

void Window::dispatchUnloadEvent()
{
    String* eventType = starFish()->staticStrings()->m_unload.string();
    Event* e = new Event(eventType, EventInit(false, false));
    EventTarget::dispatchEvent(document()->bodyElement(), e);
}

void Window::pause()
{
    STARFISH_LOG_INFO("onPause");
    m_isRunning = false;

    document()->setVisibleState(PageVisibilityState::PageVisibilityStateHidden);
    document()->visibilityStateChanged();
}

void Window::resume()
{
    StackingContext* ctx = m_document->frame()->firstChild()->asFrameBox()->stackingContext();

    std::function<void(StackingContext*)> clearSC = [&clearSC](StackingContext* ctx)
    {
        ctx->clearOwnBuffer();
        auto iter = ctx->childContexts().begin();
        while (iter != ctx->childContexts().end()) {
            auto iter2 = iter->second->begin();
            while (iter2 != iter->second->end()) {
                clearSC(*iter2);
                iter2++;
            }
            iter++;
        }
    };

    clearSC(ctx);

    WindowImplEFL* eflWindow = (WindowImplEFL*)this;
    auto a = eflWindow->m_drawnImageList.begin();
    while (a != eflWindow->m_drawnImageList.end()) {
        std::vector<std::pair<Evas_Object*, bool> > & vec = a->second;

        for (size_t i = 0; i < vec.size(); i ++) {
            Evas_Object* obj = vec[i].first;
            evas_object_del(obj);
        }
        a++;
    }
    eflWindow->m_drawnImageList.clear();


    STARFISH_LOG_INFO("onResume");
    m_isRunning = true;
    m_needsRendering = true;
    m_needsPainting = true;
    rendering();

    evas_render(evas_object_evas_get(eflWindow->m_window));

    document()->setVisibleState(PageVisibilityState::PageVisibilityStateVisible);
    document()->visibilityStateChanged();
}

void Window::close()
{
    STARFISH_LOG_INFO("onClose");
    dispatchUnloadEvent();
}


}
