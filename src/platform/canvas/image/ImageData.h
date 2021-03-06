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

#ifndef __ImageData__
#define __ImageData__

namespace StarFish {

class ImageData : public gc {
protected:
    ImageData()
    {

    }
public:
    static ImageData* create(String* localImageSrc);
    static ImageData* create(const char* buf, size_t len);
    virtual size_t bufferSize() = 0;
    virtual void* unwrap() = 0;
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual ~ImageData()
    {

    }
};

}

#endif
