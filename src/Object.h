/***********************************************************************************************************************
* Copyright (c) 2022-2022 Daemyung Jang.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***********************************************************************************************************************/

#ifndef CLMTL_OBJECT_H
#define CLMTL_OBJECT_H

#include <cstdint>
#include <atomic>

namespace cml {

class Object {
public:
    virtual ~Object();
    void Retain();
    void Release();
    uint64_t GetReferenceCount() const;

protected:
    Object();

private:
    std::atomic<uint64_t> mReferenceCount;
};

} //namespace cml

#endif //CLMTL_OBJECT_H
