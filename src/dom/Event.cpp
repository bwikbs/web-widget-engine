#include "StarFishConfig.h"
#include "Event.h"

namespace StarFish {

Event::Event(ScriptBindingInstance* instance)
{
    initScriptWrappable(this, instance);
}

UIEvent::UIEvent(ScriptBindingInstance* instance)
    : Event(instance)
{
    initScriptWrappable(this, instance);
}

MouseEvent::MouseEvent(ScriptBindingInstance* instance)
    : UIEvent(instance)
{
    initScriptWrappable(this, instance);
}

ProgressEvent::ProgressEvent(ScriptBindingInstance* instance,uint32_t loaded,uint32_t total)
    : Event(instance)
{
    initScriptWrappable(this, instance,loaded,total);
}

}
