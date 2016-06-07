/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

(function () {
    'use strict';

    var PlatformMode = {
        PM_USER_READ : 0x0100,
        PM_USER_WRITE : 0x0080,
        PM_USER_EXEC : 0x0040,
        PM_GROUP_READ : 0x0020,
        PM_GROUP_WRITE : 0x0010,
        PM_GROUP_EXEC : 0x0008,
        PM_OTHER_READ : 0x0004,
        PM_OTHER_WRITE : 0x0002,
        PM_OTHER_EXEC : 0x0001,
        PM_NONE : 0x0000
    };

    module.exports = PlatformMode;
})();