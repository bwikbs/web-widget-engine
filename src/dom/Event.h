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

#ifndef __StarFishEvent__
#define __StarFishEvent__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

struct EventInit {
    EventInit();
    EventInit(bool bubbles, bool cancelable);

    bool bubbles;
    bool cancelable;
};

// https://heycam.github.io/webidl/#common-DOMTimeStamp
typedef unsigned long long DOMTimeStamp;

class Event : public ScriptWrappable {
protected:

public:
    enum PhaseType {
        NONE                = 0,
        CAPTURING_PHASE     = 1,
        AT_TARGET           = 2,
        BUBBLING_PHASE      = 3
    };

    Event();
    Event(String* eventType, const EventInit& init = EventInit(false, false));

    virtual ~Event() { }

    virtual void initScriptObject(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this);
    }

    const String* type() const { return m_type; }
    EventTarget* target() const { return m_target; }
    void setTarget(EventTarget* target) { m_target = target; }

    EventTarget* currentTarget() const { return m_currentTarget; }
    void setCurrentTarget(EventTarget* currentTarget) { m_currentTarget = currentTarget; }

    unsigned short eventPhase() const { return m_eventPhase; }
    void setEventPhase(unsigned short phase) { m_eventPhase = phase; }

    bool stopPropagation() { return m_propagationStopped || m_immediatePropagationStopped; }
    void setStopPropagation() { m_propagationStopped = true; }
    bool stopImmediatePropagation() { return m_immediatePropagationStopped; }
    void setStopImmediatePropagation() { m_propagationStopped = true; m_immediatePropagationStopped = true; }

    bool bubbles() const { return m_bubbles; }
    bool cancelable() const { return m_cancelable; }

    void preventDefault()
    {
        if (m_cancelable)
            m_defaultPrevented = true; // canceled flag
    }
    bool defaultPrevented() const { return m_defaultPrevented; }
    void setDefaultPrevented(bool defaultPrevented) { m_defaultPrevented = defaultPrevented; }

    DOMTimeStamp timeStamp() const { return m_timeStamp; }

    bool isInitialized() const { return m_isInitialized; }
    void setIsInitialized(bool isInitialized) { m_isInitialized = isInitialized; }
    bool isDispatched() const { return m_isDispatched; }
    void setIsDispatched(bool isDispatched) { m_isDispatched = isDispatched; }

    /* Other methods (not in Event interface) */
    virtual bool isProgressEvent() const { return false; }

    ProgressEvent* asProgressEvent()
    {
        STARFISH_ASSERT(isProgressEvent());
        return (ProgressEvent*)this;
    }

private:
    bool m_isInitialized; // initialized flag

    String* m_type;
    EventTarget* m_target;
    EventTarget* m_currentTarget;

    unsigned short m_eventPhase;

    bool m_propagationStopped; // stop propagation flag
    bool m_immediatePropagationStopped; // stop immediate propagation flag

    bool m_bubbles;
    bool m_cancelable;
    bool m_defaultPrevented; // canceled flag

    DOMTimeStamp m_timeStamp;

    bool m_isDispatched; // dispatch flag
};

class UIEvent : public Event {
protected:
    UIEvent(ScriptBindingInstance* instance);
public:
};

class MouseEvent : public UIEvent {
public:
    MouseEvent(ScriptBindingInstance* instance);
};

struct ProgressEventInit : public EventInit {
    ProgressEventInit();
    ProgressEventInit(bool bubbles, bool cancelable, bool lengthComputable, unsigned long long loaded, unsigned long long total);

    bool lengthComputable;
    unsigned long long loaded;
    unsigned long long total;
};

class ProgressEvent : public Event {
public:
    ProgressEvent(String* eventType, const ProgressEventInit& init = ProgressEventInit(false, false, false, 0, 0));

    bool lengthComputable() const { return m_lengthComputable; }
    unsigned long long loaded() const { return m_loaded; }
    unsigned long long total() const { return m_total; }

    /* Other methods (not in ProgressEvent interface) */
    virtual bool isProgressEvent() const { return true; }

private:
    bool m_lengthComputable;
    unsigned long long m_loaded;
    unsigned long long m_total;
};

}

#endif
