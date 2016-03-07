#ifdef NDEBUG
#ifdef STARFISH_TIZEN_WEARABLE

#include "StarFishConfig.h"
#include "StarFish.h"

#include "StarFishPublic.h"

#include <cstdlib>

using namespace StarFish;

#define TO_STARFISH(instance) ((StarFish::StarFish*)instance->m_starfish)

extern "C" StarFishInstance* starfishInit(void* window, const char* workingDirectory)
{
    starfishGCAddRoots(String::emptyString, String::emptyString + sizeof(String*));
    starfishGCAddRoots(String::spaceString, String::spaceString + sizeof(String*));
    StarFishInstance* instance = (StarFishInstance*)malloc(sizeof(StarFishInstance));
    instance->m_starfish = new StarFish::StarFish((StarFish::StarFishStartUpFlag)0, String::fromUTF8(workingDirectory), window);
    starfishGCAddRoots(instance->m_starfish, (StarFish::StarFish*)instance->m_starfish + sizeof(StarFish::StarFish*));
    return instance;
}

extern "C" void starfishRemove(StarFishInstance* instance)
{
    starfishGCRemoveRoots(instance->m_starfish, (StarFish::StarFish*)instance->m_starfish + sizeof(StarFish::StarFish*));
    delete TO_STARFISH(instance);
    free(instance);
}

extern "C" void starfishLoadXMLDocument(StarFishInstance* instance, const char* xmlPath)
{
    TO_STARFISH(instance)->loadXMLDocument(String::fromUTF8(xmlPath));
}

extern "C" void starfishNotifyPause(StarFishInstance* instance)
{
    TO_STARFISH(instance)->pause();
}

extern "C" void starfishNotifyResume(StarFishInstance* instance)
{
    TO_STARFISH(instance)->resume();
}

extern "C" void starfishGCAddRoots(void* start, void* end)
{
    GC_add_roots(start, end);
}

extern "C" void starfishGCRemoveRoots(void* start, void* end)
{
    GC_remove_roots(start, end);
}


#endif
#endif
