/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifdef STARFISH_ENABLE_TEST
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#endif
#include "StarFishConfig.h"
#include "StarFish.h"
#include "dom/Document.h"
#include "platform/threading/ThreadPool.h"
#include "platform/message_loop/MessageLoop.h"
#include "platform/window/Window.h"
#include "platform/canvas/image/ImageData.h"
#include "dom/binding/ScriptBindingInstance.h"
#include "extra/Console.h"

#include <malloc.h>
#include <Elementary.h>
#if defined(STARFISH_TIZEN_3_0) || defined(STARFISH_TIZEN_OBS)
#include <Ecore.h>
#else
#include <Ecore_X.h>
#endif

extern Evas* g_internalCanvas;

#ifdef STARFISH_TIZEN_WEARABLE
#include <tizen.h>
#endif
namespace StarFish {

#ifdef STARFISH_ENABLE_TEST
bool g_enablePixelTest = false;
bool g_memLogDump = false;
FILE* fp_mem = NULL;
static double process_mem_usage()
{
    double vm_usage     = 0.0;
    double resident_set = 0.0;

    // 'file' stat seems to give the most reliable results

    std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    std::string pid, comm, state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    std::string utime, stime, cutime, cstime, priority, nice;
    std::string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
    >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
    >> utime >> stime >> cutime >> cstime >> priority >> nice
    >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage     = vsize / 1024.0;
    resident_set = rss * page_size_kb;

    return resident_set;
}
#endif

static bool g_starFishGlobalInit = false;

StarFish::StarFish(StarFishStartUpFlag flag, const char* locale, const char* timezoneID, void* win, int w, int h, float defaultFontSizeMultiplier)
    : m_locale(icu::Locale::createFromName(locale))
    , m_lineBreaker(nullptr)
    , m_timezoneID(String::fromUTF8(timezoneID))
    , m_defaultFontSizeMultiplier(defaultFontSizeMultiplier)
    , m_console(new Console(this))
    , m_enterCount(0)
    , m_seed((unsigned int)time(NULL))
{
    if (!g_starFishGlobalInit) {
        g_starFishGlobalInit = true;

        GC_set_abort_func([](const char* msg) {
            STARFISH_LOG_ERROR("gc abort called\n");
            STARFISH_LOG_ERROR("%s\n", msg);
        });

        GC_set_warn_proc([](char *msg, GC_word arg)
        {
            STARFISH_LOG_ERROR(msg, arg);
        });
        GC_set_on_collection_event([](GC_EventType evtType) {
            if (GC_EVENT_PRE_START_WORLD == evtType) {
#ifdef STARFISH_ENABLE_TEST
                if (fp_mem&&g_memLogDump)
                    fprintf(fp_mem, "%f %f\n", GC_get_memory_use() / 1024.f / 1024.f, process_mem_usage() / 1024.f);
                STARFISH_LOG_INFO("did GC. GC heapSize[%f MB , %f MB] RSS[%.1f MB]\n", GC_get_memory_use() / 1024.f / 1024.f, GC_get_heap_size() / 1024.f / 1024.f, process_mem_usage() / 1024.f);
#else
                STARFISH_LOG_INFO("did GC. GC heapSize[%f MB , %f MB]\n", GC_get_memory_use() / 1024.f / 1024.f, GC_get_heap_size() / 1024.f / 1024.f);
#endif
                // malloc_stats();
            }
        });
        GC_set_free_space_divisor(64);
        GC_set_force_unmap_on_gcollect(1);

    }

    if (!win) {
        Evas_Object* wndObj = elm_win_add(NULL, "StarFish", ELM_WIN_BASIC);
        elm_win_title_set(wndObj, "StarFish");
        elm_win_autodel_set(wndObj, EINA_TRUE);
        evas_object_resize(wndObj, w, h);
        win = wndObj;
    } else {
        evas_object_resize((Evas_Object*)win, w, h);
    }

    m_nativeWindow = win;

    m_deviceKind = deviceKindUseTouchScreen;
    m_startUpFlag = flag;

    String* s = String::emptyString;
    AtomicString emptyAtom(s);
    m_atomicStringMap.insert(std::make_pair(std::string(), emptyAtom));
    m_staticStrings = new StaticStrings(this);
    UErrorCode code = U_ZERO_ERROR;
    m_lineBreaker = icu::BreakIterator::createLineInstance(m_locale, code);
    STARFISH_RELEASE_ASSERT(code <= U_ZERO_ERROR);
    m_messageLoop = new MessageLoop(this);
    m_threadPool = new ThreadPool(2, m_messageLoop);
}

StarFish::~StarFish()
{
    STARFISH_LOG_INFO("StarFish::~StarFish\n");
#ifdef STARFISH_ENABLE_TEST
    if (fp_mem)
        fclose(fp_mem);
#endif
    close();
    delete m_lineBreaker;
    delete m_window;
}

void StarFish::run()
{
    m_messageLoop->run();
}

void StarFish::enter()
{
    if (m_enterCount == 0) {
        g_internalCanvas = evas_object_evas_get((Evas_Object*)m_window->unwrap());
        m_window->scriptBindingInstance()->enter();
    }
    m_enterCount++;
}

void StarFish::exit()
{
    if (m_enterCount == 1) {
        g_internalCanvas = nullptr;
        m_window->scriptBindingInstance()->exit();
    }
    m_enterCount--;
}

void StarFish::loadHTMLDocument(String* filePath)
{
    std::string path;
    if (filePath->startsWith("http")) {
        path = filePath->utf8Data();
    } else {
        std::string d = filePath->utf8Data();
        if (d.length() && d[0] == '/') {
            path = std::string("file://") + d;
        } else {
            std::string fileName;
            if (d.find('/') == std::string::npos) {
                path = "./";
                fileName = d;
            } else {
                path += d.substr(0, d.find_last_of('/'));
                fileName = d.substr(d.find_last_of('/') + 1);
                path += "/";
            }

            char* p = realpath(path.c_str(), NULL);
            if (p) {
                path = p;
                free(p);
            }
            path += "/";

            path = std::string("file://") + path + fileName;
#ifdef STARFISH_ENABLE_TEST
            std::string mem_log =  fileName.substr(0, fileName.length()-5)+"_mem.txt";
            if (g_memLogDump)
                fp_mem = fopen(mem_log.c_str(), "w");
#endif
        }
    }


    int width;
    int height;
    evas_object_geometry_get((Evas_Object*)m_nativeWindow, NULL, NULL, &width, &height);
    m_window = Window::create(this, m_nativeWindow, width, height);
    m_window->navigate(URL::createURL(String::emptyString, String::fromUTF8(path.c_str())));
}

void StarFish::resume()
{
    StarFishEnterer enter(this);
    m_window->resume();
}

void StarFish::pause()
{
    StarFishEnterer enter(this);
    m_window->pause();
    GC_gcollect_and_unmap();
    GC_gcollect_and_unmap();
    GC_gcollect_and_unmap();
    GC_gcollect_and_unmap();
}

void StarFish::close()
{
    m_window->close();
}

String* StarFish::evaluate(String* s)
{
    return m_window->scriptBindingInstance()->evaluate(s);
}

void StarFish::addPointerInRootSet(void *ptr)
{
    auto iter = m_rootMap.find(ptr);
    if (iter == m_rootMap.end()) {
        m_rootMap.insert(std::make_pair(ptr, 1));
    } else {
        iter->second++;
    }
}

void StarFish::removePointerFromRootSet(void *ptr)
{
    auto iter = m_rootMap.find(ptr);
    if (iter != m_rootMap.end()) {
        if (iter->second == 1) {
            m_rootMap.erase(iter);
        } else {
            iter->second--;
        }
    }
}

BlobURLStore StarFish::addBlobInBlobURLStore(Blob* ptr)
{
#ifndef NDEBUG
    {
        BlobURLStore s;
        s.m_blob = ptr;
        STARFISH_ASSERT(m_urlBlobStore.find(s) == m_urlBlobStore.end());
    }
#endif
    BlobURLStore a;
    a.m_blob = ptr;

#ifdef STARFISH_32
    a.m_a = rand_r(&m_seed);
    a.m_b = rand_r(&m_seed);
    a.m_c = rand_r(&m_seed);
#else
    a.m_a = rand_r(&m_seed);
    a.m_b = rand_r(&m_seed);
#endif

    m_urlBlobStore.insert(a);

    return a;
}

void StarFish::removeBlobFromBlobURLStore(Blob* ptr)
{
#ifndef NDEBUG
    {
        BlobURLStore s;
        s.m_blob = ptr;
        STARFISH_ASSERT(m_urlBlobStore.find(s) != m_urlBlobStore.end());
    }
#endif
    BlobURLStore s;
    s.m_blob = ptr;
    m_urlBlobStore.erase(s);
}

bool StarFish::isValidBlobURL(BlobURLStore ptr)
{
    auto iter = m_urlBlobStore.find(ptr);
#ifdef STARFISH_32
    return iter != m_urlBlobStore.end() && ptr.m_a == iter->m_a && ptr.m_b == iter->m_b && ptr.m_c == iter->m_c;
#else
    return iter != m_urlBlobStore.end() && ptr.m_a == iter->m_a && ptr.m_b == iter->m_b;
#endif
}

bool StarFish::isValidBlobURL(Blob* ptr)
{
    BlobURLStore s;
    s.m_blob = ptr;
    auto iter = m_urlBlobStore.find(s);
    return iter != m_urlBlobStore.end();
}

BlobURLStore StarFish::findBlobURL(Blob* ptr)
{
    BlobURLStore s;
    s.m_blob = ptr;
    auto iter = m_urlBlobStore.find(s);
    return *iter;
}

StaticStrings::StaticStrings(StarFish* sf)
    : m_starFish(sf)
    , m_xhtmlNamespaceURI(AtomicString::createAtomicString(sf, "http://www.w3.org/1999/xhtml"))
    , m_documentLocalName(AtomicString::createAtomicString(sf, "#document"))
    , m_documentFragmentLocalName(AtomicString::createAtomicString(sf, "#document-fragment"))
    , m_textLocalName(AtomicString::createAtomicString(sf, "#text"))
    , m_commentLocalName(AtomicString::createAtomicString(sf, "#comment"))
{
#define DEFINE_HTML_LOCAL_NAMES(name) \
    m_##name##TagName = QualifiedName(m_xhtmlNamespaceURI, AtomicString::createAtomicString(sf, #name));
    STARFISH_ENUM_HTML_TAG_NAMES(DEFINE_HTML_LOCAL_NAMES)
#undef DEFINE_HTML_LOCAL_NAMES
    m_id = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "id"));
    m_name = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "name"));
    m_class = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "class"));
    m_localName = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "localName"));
    m_style = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "style"));
    m_src = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "src"));
    m_width = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "width"));
    m_height = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "height"));
    m_rel = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "rel"));
    m_href = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "href"));
    m_type = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "type"));
    m_dir = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "dir"));
    m_color = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "color"));
    m_face = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "face"));
    m_size = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "size"));
    m_charset = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "charset"));
    m_content = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "content"));

    m_click = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "click"));
    m_onclick = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "onclick"));
    m_load = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "load"));
    m_error = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "error"));
    m_onload = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "onload"));
    m_unload = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "unload"));
    m_onunload = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "onunload"));
    m_visibilitychange = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "visibilitychange"));
    m_DOMContentLoaded = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "DOMContentLoaded"));
    m_readystatechange = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "readystatechange"));
    m_progress = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "progress"));
    m_abort = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "abort"));
    m_timeout = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "timeout"));
    m_loadend = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "loadend"));
    m_loadstart = QualifiedName(AtomicString::emptyAtomicString(), AtomicString::createAtomicString(sf, "loadstart"));
}

}
