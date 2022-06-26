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

#include "Kernel.h"

#include <cassert>
#include <sstream>
#include <Foundation/NSString.hpp>

#include "Dispatch.h"
#include "Device.h"
#include "Program.h"
#include "LibraryPool.h"

namespace cml {

std::string ConvertToString(const std::unordered_map<uint32_t, std::string> &defines) {
    std::stringstream stream;

    for (auto &[ordinal, define] : defines) {
        stream << define;
    }

    return stream.str();
}

uint64_t GetHash(const Size &size) {
    return (size.w << 42) | (size.h << 21) | (size.d << 0);
}

MTL::FunctionConstantValues *CreateConstantValues(const Size &workGroupSize) {
    auto values = MTL::FunctionConstantValues::alloc()->init();

    values->setConstantValue(&workGroupSize.w, MTL::DataTypeUInt, 0ul);
    values->setConstantValue(&workGroupSize.h, MTL::DataTypeUInt, 1ul);
    values->setConstantValue(&workGroupSize.d, MTL::DataTypeUInt, 2ul);

    return values;
}

Kernel *Kernel::DownCast(cl_kernel kernel) {
    return (Kernel *) kernel;
}

Kernel::Kernel(Program *program, const std::string &name)
    : _cl_kernel{Dispatch::GetTable()}, Object{}, mProgram{program}, mName{name}
    , mBindings{program->GetReflection().at(name)}, mPipelineStates{}, mArgTable{} {
    InitBindings();
    InitPipelineState();
    InitArgTable();
}

Kernel::~Kernel() {
    for (auto &[hash, pipelineStates] : mPipelineStates) {
        for (auto &[defines, pipelineState] : pipelineStates) {
            pipelineState->release();
        }
    }
}

void Kernel::SetArg(size_t index, const void *data, size_t size) {
    if (mBindings[index].Kind != clspv::ArgKind::Local) {
        if (data) {
            memcpy(mArgTable[index].Data, data, size);
        }

        mArgTable[index].Size = size;
    } else {
        std::stringstream stream;

        stream << "#define SPIRV_CROSS_CONSTANT_ID_" << mBindings[index].Spec << " "
               << size / mBindings[index].Size << "\n";
        mDefines[index] = stream.str();
    }
}

Context *Kernel::GetContext() const {
    return mProgram->GetContext();
}

Program *Kernel::GetProgram() const {
    return mProgram;
}

std::string Kernel::GetName() const {
    return mName;
}

MTL::ComputePipelineState *Kernel::GetPipelineState(const Size &workGroupSize) {
    auto hash = GetHash(workGroupSize);
    auto defines = ConvertToString(mDefines);

    if (!mPipelineStates.count(hash) || !mPipelineStates.at(hash).count(defines)) {
        AddPipelineState(hash, workGroupSize);
    }

    return mPipelineStates[hash][defines];
}

size_t Kernel::GetWorkGroupSize() const {
    return mPipelineStates.at(0).at("")->maxTotalThreadsPerThreadgroup();
}

size_t Kernel::GetWorkItemExecutionWidth() const {
    return mPipelineStates.at(0).at("")->threadExecutionWidth();
}

std::unordered_map<uint32_t, Arg> Kernel::GetArgTable() const {
    return mArgTable;
}

void Kernel::InitBindings() {
    std::sort(mBindings.begin(), mBindings.end(), [](auto &lhs, auto &rhs) { return lhs.Ordinal < rhs.Ordinal; });
}

void Kernel::InitPipelineState() {
    try {
        AddPipelineState(0, {1, 1, 1});
    } catch (std::exception &e) {
        Release();

        throw e;
    }
}

void Kernel::InitArgTable() {
    for (auto &binding : mBindings) {
        mArgTable[binding.Ordinal] = {.Kind = binding.Kind, .Binding = binding.Index};
    }
}

MTL::Function *Kernel::CreateFunction(const Size &workGroupSize) {
    auto name = NS::String::alloc()->init(mName.c_str(), NS::UTF8StringEncoding);
    auto constantValues = CreateConstantValues(workGroupSize);
    NS::Error *error = nullptr;

    auto function = Device::GetSingleton()->GetLibraryPool()->At(mProgram, ConvertToString(mDefines))->
        newFunction(name, constantValues, &error);
    assert(function);

    name->release();
    constantValues->release();

    if (error) {
        error->release();
    }

    return function;
}

void Kernel::AddPipelineState(uint64_t hash, const Size &workGroupSize) {
    auto function = CreateFunction(workGroupSize);
    NS::Error *error = nullptr;

    mPipelineStates[hash][ConvertToString(mDefines)] = Device::GetSingleton()->GetDevice()->
        newComputePipelineState(function, &error);

    function->release();

    if (error) {
        error->release();

        throw std::exception();
    }
}

} //namespace cml
