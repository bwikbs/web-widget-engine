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
#include "DOMException.h"

namespace StarFish {

const char* DOMException::s_names[] = {
    "DOMException",
    "IndexSizeError",
    "",
    "HierarchyRequestError",
    "WrongDocumentError",
    "InvalidCharacterError",
    "",
    "NoModificationAllowedError",
    "NotFoundError",
    "NotSupportedError",
    "InUseAttributeError",
    "InvalidStateError",
    "SyntaxError",
    "InvalidModificationError",
    "NamespaceError",
    "InvalidAccessError",
    "",
    "",
    "SecurityError",
    "NetworkError",
    "AbortError",
    "URLMismatchError",
    "QuotaExceededError",
    "TimeoutError",
    "InvalidNodeTypeError",
    "DataCloneError",
};

const char* DOMException::s_descriptions[] = {
    "Unspecified DOM Exception.",
    "The index is not in the allowed range.",
    "",
    "The operation would yield an incorrect node tree.",
    "The object is in the wrong document.",
    "The string contains invalid characters.",
    "",
    "The object can not be modified.",
    "The object can not be found here.",
    "The operation is not supported.",
    "The attribute is in use.",
    "The object is in an invalid state.",
    "The string did not match the expected pattern.",
    "The object can not be modified in this way.",
    "The operation is not allowed by Namespaces in XML.",
    "The object does not support the operation or argument.",
    "",
    "",
    "The operation is insecure.",
    "A network error occurred.",
    "The operation was aborted.",
    "The given URL does not match another URL.",
    "The quota has been exceeded.",
    "The operation timed out.",
    "The supplied node is incorrect or has an incorrect ancestor for this operation.",
    "The object can not be cloned.",
};

DOMException::DOMException(ScriptBindingInstance* instance, Code code, const char* message)
    : ScriptWrappable(this)
    , m_instance(instance)
    , m_code(code)
{
    if (!message)
        message = s_descriptions[code];
    m_message = String::fromUTF8(message);

}

}
