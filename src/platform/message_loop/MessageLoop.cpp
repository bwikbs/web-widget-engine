#include "StarFishConfig.h"
#include "MessageLoop.h"

#include <Elementary.h>

namespace StarFish {

void MessageLoop::run()
{
    elm_run();
}

}
