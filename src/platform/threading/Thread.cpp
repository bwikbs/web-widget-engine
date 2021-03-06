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

#include "StarFishConfig.h"
#include "Thread.h"
#include "platform/message_loop/MessageLoop.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace StarFish {

bool isMainThread()
{
#ifdef SYS_gettid
    pid_t tid = syscall(SYS_gettid);
#else
#error "SYS_gettid unavailable on this system"
#endif
    return getpid() == tid;
}

Thread::Thread()
{
    m_alive = false;
}

void Thread::run(MessageLoop* msgLoop, ThreadWorker fn, void* data)
{
    STARFISH_ASSERT(isMainThread());
    STARFISH_ASSERT(!m_alive);
    m_alive = true;

    struct ThreadData {
        Thread* thread;
        MessageLoop* messageLoop;
        ThreadWorker fn;
        void* data;
        pthread_t tid;
    };

    ThreadData* d = new(NoGC) ThreadData;
    d->thread = this;
    d->messageLoop = msgLoop;
    d->fn = fn;
    d->data = data;

    pthread_create(&d->tid, NULL, [](void* data) -> void* {
        ThreadData* d = (ThreadData*)data;
        auto ret = d->fn(d->data);
        d->thread->m_alive = false;
        d->messageLoop->addIdlerWithNoGCRootingInOtherThread([](size_t handle, void* data) {
            ThreadData* d = (ThreadData*)data;
            void* ret;
            pthread_join(d->tid, &ret);
            GC_FREE(data);
        }, d);
        pthread_exit(ret);
    }, d);
}


}
