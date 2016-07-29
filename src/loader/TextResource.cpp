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
#include "TextResource.h"

#include "dom/Document.h"

namespace StarFish {

void TextResource::didDataReceived(const char* buffer, size_t length)
{
    Resource::didDataReceived(buffer, length);
    if (!m_converter)
        m_converter = new TextConverter(m_networkRequest->mimeType(), m_networkRequest->starFish()->window()->document()->charset(), buffer, length);
    m_text = m_text->concat(m_converter->convert(buffer, length, true));
}

}