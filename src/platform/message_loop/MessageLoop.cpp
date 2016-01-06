#include "StarFishConfig.h"
#include "MessageLoop.h"

#include <Elementary.h>

namespace StarFish {

void MessageLoop::run()
{
#ifndef STARFISH_TIZEN_WEARABLE
    elm_run();
#endif
}

}
