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

#ifndef CLMTL_ARG_TABLE_H
#define CLMTL_ARG_TABLE_H

#include <vector>
#include <unordered_map>
#include <clspv/ArgKind.h>

namespace cml {

struct Binding {
    std::string Kernel;
    uint32_t Ordinal;
    clspv::ArgKind Kind;
    uint32_t Index;
    uint32_t Size;
    uint32_t Offset;
    uint32_t Spec;
};

class Reflector {
public:
    static std::unordered_map<std::string, std::vector<Binding>> Reflect(const std::vector<uint32_t> &binary);
};

} //namespace cml

#endif //CLMTL_ARG_TABLE_H
