#include "StarFishConfig.h"
#include "StarFish.h"
#include "platform/message_loop/MessageLoop.h"
#include "platform/window/Window.h"
#include "platform/canvas/image/ImageData.h"
#include "dom/binding/ScriptBindingInstance.h"

#include <Elementary.h>

namespace StarFish {

#ifndef STARFISH_TIZEN_WEARABLE
StarFish::StarFish(String* currentPath)
{
    m_currentPath = currentPath;
    GC_add_roots(String::emptyString, String::emptyString + sizeof(String*));
    elm_init(0,0);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    m_messageLoop = new MessageLoop();
    m_scriptBindingInstance = new ScriptBindingInstance();
    m_scriptBindingInstance->initBinding(this);
    m_window = Window::create(this, 360, 360);
}
#else
StarFish::StarFish(String* currentPath, void* win)
{
    m_currentPath = currentPath;
    GC_add_roots(String::emptyString, String::emptyString + sizeof(String*));
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

void StarFish::resume()
{
    m_window->resume();
}

void StarFish::pause()
{
    m_window->pause();
}

void StarFish::evaluate(String* s)
{
    m_scriptBindingInstance->evaluate(s);
}

ImageData* StarFish::fetchImage(String* str)
{
    auto iter = m_imageCache.find(str->utf8Data());
    if (iter == m_imageCache.end()) {
        ImageData* id = ImageData::create(str);
        m_imageCache.insert(std::make_pair(std::string(str->utf8Data()), id));
        return id;
    }
    return iter->second;
}

StaticStrings::StaticStrings()
{
    m_documentLocalName = String::createASCIIString("#document");
    m_textLocalName = String::createASCIIString("#text");
    m_htmlLocalName = String::createASCIIString("html");
    m_headLocalName = String::createASCIIString("head");
    m_styleLocalName = String::createASCIIString("style");
    m_scriptLocalName = String::createASCIIString("script");
    m_bodyLocalName = String::createASCIIString("body");
    m_divLocalName = String::createASCIIString("div");
    m_imageLocalName = String::createASCIIString("img");

    m_id = String::createASCIIString("id");
    m_class = String::createASCIIString("class");
    m_src = String::createASCIIString("src");

    m_click = String::createASCIIString("click");
    m_onclick = String::createASCIIString("onclick");
}

}

