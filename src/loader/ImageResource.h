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

#ifndef __StarFishImageResource__
#define __StarFishImageResource__

#include "loader/Resource.h"

namespace StarFish {

class ImageData;
class ImageResource : public Resource {
    friend class ResourceLoader;
    ImageResource(URL* url, ResourceLoader* loader)
        : Resource(url, loader)
    {
        m_imageData = nullptr;
    }
public:
    virtual bool isImageResource()
    {
        return true;
    }

    ImageData* imageData()
    {
        return m_imageData;
    }

    virtual void request(ResourceRequestSyncLevel syncLevel = NeverSync);
    virtual void didLoadFinished();

    static void doLoadFile(void*);
protected:
    ImageData* m_imageData;
};

inline void ImageResourceDoLoadFile(void* data)
{
    ImageResource::doLoadFile(data);
}

}

#endif
