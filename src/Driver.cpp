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

#include <sstream>
#include <CL/cl_icd.h>

#include "Util.h"
#include "Dispatch.h"
#include "Platform.h"
#include "Device.h"
#include "Context.h"
#include "CommandQueue.h"
#include "Memory.h"
#include "Buffer.h"
#include "Image.h"
#include "Program.h"
#include "Kernel.h"
#include "Event.h"
#include "Sampler.h"

/***********************************************************************************************************************
* OpenCL Core APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
* Platform APIs
***********************************************************************************************************************/

cl_int clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms) {
    if (!num_entries && platforms) {
        return CL_INVALID_VALUE;
    }

    if (!platforms && !num_platforms) {
        return CL_INVALID_VALUE;
    }

    if (platforms) {
        platforms[0] = cml::Platform::GetSingleton();
    }

    if (num_platforms) {
        num_platforms[0] = 1;
    }

    return CL_SUCCESS;
}

cl_int clGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name, size_t param_value_size,
                         void *param_value, size_t *param_value_size_ret) {
    if (platform != cml::Platform::GetSingleton()) {
        return CL_INVALID_PLATFORM;
    }

    std::string info;

    switch (param_name) {
        case CL_PLATFORM_PROFILE:
            info = cml::Platform::GetProfile();
            break;
        case CL_PLATFORM_VERSION:
            info = cml::Platform::GetVersion();
            break;
        case CL_PLATFORM_NAME:
            info = cml::Platform::GetName();
            break;
        case CL_PLATFORM_VENDOR:
            info = cml::Platform::GetVendor();
            break;
        case CL_PLATFORM_EXTENSIONS:
            info = cml::Platform::GetExtensions();
            break;
        case CL_PLATFORM_ICD_SUFFIX_KHR:
            info = cml::Platform::GetSuffix();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size <= info.size()) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info.c_str(), info.size() + 1);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = info.size() + 1;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Device APIs
***********************************************************************************************************************/

cl_int clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices,
                      cl_uint *num_devices) {
    if (platform && !cml::Platform::DownCast(platform)) {
        return CL_INVALID_PLATFORM;
    }

    if (!num_entries && devices) {
        return CL_INVALID_VALUE;
    }

    if (!devices && !num_devices) {
        return CL_INVALID_VALUE;
    }

    if (device_type != CL_DEVICE_TYPE_ALL &&
        cml::Util::TestAnyFlagSet(device_type, CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_ACCELERATOR)) {
        return CL_DEVICE_NOT_FOUND;
    }

    if (devices) {
        if (num_entries < 1) {
            return CL_INVALID_VALUE;
        } else {
            devices[0] = cml::Device::GetSingleton();
        }
    }

    if (num_devices) {
        num_devices[0] = 1;
    }

    return CL_SUCCESS;
}

cl_int clGetDeviceInfo(cl_device_id device, cl_device_info param_name, size_t param_value_size, void *param_value,
                       size_t *param_value_size_ret) {
    auto cmlDevice = cml::Device::DownCast(device);

    if (!cmlDevice) {
        return CL_INVALID_DEVICE;
    }

    auto limits = cmlDevice->GetLimits();
    void *info;
    size_t size;

    switch (param_name) {
        case CL_DEVICE_TYPE:
            info = &limits.Type;
            size = sizeof(cl_device_type);
            break;
        case CL_DEVICE_VENDOR_ID:
            info = &limits.VendorId;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_COMPUTE_UNITS:
            info = &limits.MaxComputeUnits;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
            info = &limits.MaxWorkItemDimensions;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_WORK_GROUP_SIZE:
            info = &limits.MaxWorkGroupSize;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_MAX_WORK_ITEM_SIZES:
            info = limits.MaxWorkItemSizes;
            size = sizeof(size_t) * 3;
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
            info = &limits.PreferredVectorWidthChar;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
            info = &limits.PreferredVectorWidthShort;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
            info = &limits.PreferredVectorWidthInt;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
            info = &limits.PreferredVectorWidthLong;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
            info = &limits.PreferredVectorWidthFloat;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
            info = &limits.PreferredVectorWidthDouble;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_CLOCK_FREQUENCY:
            info = &limits.MaxClockFrequency;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_ADDRESS_BITS:
            info = &limits.AddressBits;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_READ_IMAGE_ARGS:
            info = &limits.MaxReadImageArgs;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
            info = &limits.MaxWriteImageArgs;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
            info = &limits.MaxMemAllocSize;
            size = sizeof(cl_ulong);
            break;
        case CL_DEVICE_IMAGE2D_MAX_WIDTH:
            info = &limits.Image2DMaxWidth;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
            info = &limits.Image2DMaxHeight;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_IMAGE3D_MAX_WIDTH:
            info = &limits.Image3DMaxWidth;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
            info = &limits.Image3DMaxHeight;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_IMAGE3D_MAX_DEPTH:
            info = &limits.Image3DMaxDepth;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_IMAGE_SUPPORT:
            info = &limits.ImageSupport;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_MAX_PARAMETER_SIZE:
            info = &limits.MaxParameterSize;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_MAX_SAMPLERS:
            info = &limits.MaxSamplers;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MEM_BASE_ADDR_ALIGN:
            info = &limits.MemBaseAddrAlign;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
            info = &limits.MinDataTypeAlignSize;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_SINGLE_FP_CONFIG:
            info = &limits.SingleFpConfig;
            size = sizeof(cl_device_fp_config);
            break;
        case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
            info = &limits.GlobalMemCacheType;
            size = sizeof(cl_device_mem_cache_type);
            break;
        case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
            info = &limits.GlobalMemCachelineSize;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
            info = &limits.GlobalMemCacheSize;
            size = sizeof(cl_ulong);
            break;
        case CL_DEVICE_GLOBAL_MEM_SIZE:
            info = &limits.GlobalMemSize;
            size = sizeof(cl_ulong);
            break;
        case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
            info = &limits.MaxConstantBufferSize;
            size = sizeof(cl_ulong);
            break;
        case CL_DEVICE_MAX_CONSTANT_ARGS:
            info = &limits.MaxConstantArgs;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_LOCAL_MEM_TYPE:
            info = &limits.LocalMemType;
            size = sizeof(cl_device_local_mem_type);
            break;
        case CL_DEVICE_LOCAL_MEM_SIZE:
            info = &limits.LocalMemSize;
            size = sizeof(cl_ulong);
            break;
        case CL_DEVICE_ERROR_CORRECTION_SUPPORT:
            info = &limits.ErrorCorrectionSupport;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_PROFILING_TIMER_RESOLUTION:
            info = &limits.ProfilingTimeResolution;
            size = sizeof(size_t);
            break;
        case CL_DEVICE_ENDIAN_LITTLE:
            info = &limits.EndianLittle;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_AVAILABLE:
            info = &limits.Available;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_COMPILER_AVAILABLE:
            info = &limits.CompilerAvailable;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_EXECUTION_CAPABILITIES:
            info = &limits.ExecCapabilities;
            size = sizeof(cl_device_exec_capabilities);
            break;
        case CL_DEVICE_QUEUE_ON_HOST_PROPERTIES:
            info = &limits.QueueOnHostProperties;
            size = sizeof(cl_command_queue_properties);
            break;
        case CL_DEVICE_NAME:
            info = limits.Name.data();
            size = limits.Name.size() + 1;
            break;
        case CL_DEVICE_VENDOR:
            info = limits.Vendor.data();
            size = limits.Vendor.size() + 1;
            break;
        case CL_DRIVER_VERSION:
            info = limits.DriverVersion.data();
            size = limits.DriverVersion.size() + 1;
            break;
        case CL_DEVICE_PROFILE:
            info = limits.Profile.data();
            size = limits.Profile.size() + 1;
            break;
        case CL_DEVICE_VERSION:
            info = limits.Version.data();
            size = limits.Version.size() + 1;
            break;
        case CL_DEVICE_EXTENSIONS:
            info = limits.Extensions.data();
            size = limits.Extensions.size() + 1;
            break;
        case CL_DEVICE_PLATFORM:
            info = limits.Platform;
            size = sizeof(cl_platform_id);
            break;
        case CL_DEVICE_DOUBLE_FP_CONFIG:
            info = &limits.DoubleFpConfig;
            size = sizeof(cl_device_fp_config);
            break;
        case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:
            info = &limits.PreferredVectorWidthHalf;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_HOST_UNIFIED_MEMORY:
            info = &limits.HostUnifiedMemory;
            size = sizeof(cl_bool);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
            info = &limits.NativeVectorWidthChar;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
            info = &limits.NativeVectorWidthShort;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:
            info = &limits.NativeVectorWidthInt;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:
            info = &limits.NativeVectorWidthLong;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
            info = &limits.NativeVectorWidthFloat;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:
            info = &limits.NativeVectorWidthDouble;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:
            info = &limits.NativeVectorWidthHalf;
            size = sizeof(cl_uint);
            break;
        case CL_DEVICE_OPENCL_C_VERSION:
            info = limits.CVersion.data();
            size = limits.CVersion.size() + 1;
            break;
        case CL_DEVICE_PARTITION_AFFINITY_DOMAIN:
            info = &limits.PartitionAffinityDomain;
            size = sizeof(cl_device_affinity_domain);
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

cl_int clCreateSubDevices(cl_device_id in_device, const cl_device_partition_property *properties, cl_uint num_devices,
                          cl_device_id *out_devices, cl_uint *num_devices_ret) {
    return CL_INVALID_DEVICE;
}

cl_int clRetainDevice(cl_device_id device) {
    return CL_INVALID_DEVICE;
}

cl_int clReleaseDevice(cl_device_id device) {
    return CL_INVALID_DEVICE;
}

#ifdef CL_VERSION_2_1

cl_int clSetDefaultDeviceCommandQueue(cl_context context, cl_device_id device, cl_command_queue command_queue) {
    return CL_INVALID_CONTEXT;
}

cl_int clGetDeviceAndHostTimer(cl_device_id device, cl_ulong *device_timestamp, cl_ulong *host_timestamp) {
    return CL_INVALID_DEVICE;
}

cl_int clGetHostTimer(cl_device_id device, cl_ulong *host_timestamp) {
    return CL_INVALID_DEVICE;
}

#endif

/***********************************************************************************************************************
* Context APIs
***********************************************************************************************************************/

cl_context clCreateContext(const cl_context_properties *properties, cl_uint num_devices, const cl_device_id *devices,
                           void (*pfn_notify)(const char *errinfo, const void *private_info, size_t cb,
                                              void *user_data),
                           void *user_data, cl_int *errcode_ret) {
    if (properties) {
        auto platform = reinterpret_cast<cl_platform_id>(
                cml::Util::ReadProperty(properties, CL_CONTEXT_PLATFORM));

        if (!platform || platform != cml::Platform::GetSingleton()) {
            if (errcode_ret) {
                errcode_ret[0] = CL_INVALID_VALUE;
            }

            return nullptr;
        }
    }

    if (!num_devices || !devices) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (num_devices > 1) {
        if (errcode_ret) {
            errcode_ret[0] = CL_DEVICE_NOT_AVAILABLE;
        }

        return nullptr;
    }

    if (devices[0] != cml::Device::GetSingleton()) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_DEVICE;
        }

        return nullptr;
    }

    if (!pfn_notify && user_data) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Context();
}

cl_context clCreateContextFromType(const cl_context_properties *properties, cl_device_type device_type,
                                   void (*pfn_notify)(const char *errinfo, const void *private_info, size_t cb,
                                                      void *user_data),
                                   void *user_data, cl_int *errcode_ret) {
    if (!cml::Util::TestAnyFlagSet(device_type, CL_DEVICE_TYPE_DEFAULT | CL_DEVICE_TYPE_GPU)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_DEVICE_NOT_FOUND;
        }

        return nullptr;
    }

    const cl_device_id devices[]{cml::Device::GetSingleton()};
    assert(devices[0]);

    return clCreateContext(properties, 1, devices, pfn_notify, user_data, errcode_ret);
}

cl_int clRetainContext(cl_context context) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        return CL_INVALID_CONTEXT;
    }

    cmlContext->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseContext(cl_context context) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        return CL_INVALID_CONTEXT;
    }

    cmlContext->Release();

    if (!cmlContext->GetReferenceCount()) {
        delete cmlContext;
    }

    return CL_SUCCESS;
}

cl_int clGetContextInfo(cl_context context, cl_context_info param_name, size_t param_value_size, void *param_value,
                        size_t *param_value_size_ret) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        return CL_INVALID_CONTEXT;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_CONTEXT_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlContext->GetReferenceCount();
            break;
        case CL_CONTEXT_DEVICES:
            size = sizeof(cl_device_id);
            *((cl_device_id *) info) = cmlContext->GetDevice();
            break;
        case CL_CONTEXT_PROPERTIES:
            size = sizeof(cl_context_properties);
            *((cl_context_properties *) info) = 0;
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

#ifdef CL_VERSION_3_0

cl_int clSetContextDestructorCallback(cl_context context, void (*pfn_notify)(cl_context context, void *user_data),
                                      void *user_data) {
    return CL_INVALID_CONTEXT;
}

#endif

/***********************************************************************************************************************
* Command Queue APIs
***********************************************************************************************************************/

#ifdef CL_VERSION_2_0

cl_command_queue clCreateCommandQueueWithProperties(cl_context context, cl_device_id device,
                                                    const cl_queue_properties *properties, cl_int *errcode_ret) {
    return nullptr;
}

#endif

cl_int clRetainCommandQueue(cl_command_queue command_queue) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    cmlCommandQueue->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseCommandQueue(cl_command_queue command_queue) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    cmlCommandQueue->Release();

    if (!cmlCommandQueue->GetReferenceCount()) {
        delete cmlCommandQueue;
    }

    return CL_SUCCESS;
}

cl_int clGetCommandQueueInfo(cl_command_queue command_queue, cl_command_queue_info param_name, size_t param_value_size,
                             void *param_value, size_t *param_value_size_ret) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_QUEUE_CONTEXT:
            size = sizeof(cl_context);
            *((cl_context *) info) = cmlCommandQueue->GetContext();
            break;
        case CL_QUEUE_DEVICE:
            size = sizeof(cl_device_id);
            *((cl_device_id *) info) = cmlCommandQueue->GetDevice();
            break;
        case CL_QUEUE_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlCommandQueue->GetReferenceCount();
            break;
        case CL_QUEUE_PROPERTIES:
            size = sizeof(cl_properties);
            *((cl_properties *) info) = cmlCommandQueue->GetProperties();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Memory Object APIs
***********************************************************************************************************************/

cl_mem clCreateBuffer(cl_context context, cl_mem_flags flags, size_t size, void *host_ptr, cl_int *errcode_ret) {
    if (!size || size > CL_INVALID_BUFFER_SIZE) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_BUFFER_SIZE;
        }

        return nullptr;
    }

    if (host_ptr && !cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR) && !host_ptr) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    if (!flags) {
        flags = CL_MEM_READ_WRITE;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    if (host_ptr) {
        return new cml::Buffer(cmlContext, flags, host_ptr, size);
    } else {
        return new cml::Buffer(cmlContext, flags, size);
    }
}

cl_mem clCreateSubBuffer(cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type buffer_create_type,
                         const void *buffer_create_info, cl_int *errcode_ret) {
    auto cmlBuffer = cml::Buffer::DownCast(buffer);

    if (!cmlBuffer) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_MEM_OBJECT;
        }

        return nullptr;
    }

    if (buffer_create_type != CL_BUFFER_CREATE_TYPE_REGION) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (!buffer_create_info) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Buffer(cmlBuffer, flags, static_cast<const cl_buffer_region *>(buffer_create_info));
}

cl_mem clCreateImage(cl_context context, cl_mem_flags flags, const cl_image_format *image_format,
                     const cl_image_desc *image_desc, void *host_ptr, cl_int *errcode_ret) {
    if (!image_format) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_IMAGE_FORMAT_DESCRIPTOR;
        }

        return nullptr;
    }

    if (!image_desc) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_IMAGE_DESCRIPTOR;
        }

        return nullptr;
    }

    if (host_ptr && !cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR) && !host_ptr) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_MEM_OBJECT_ALLOCATION_FAILURE;
        }

        return nullptr;
    }

    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    if (!flags) {
        flags = CL_MEM_READ_WRITE;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Image(cmlContext, flags, *image_format, image_desc->image_type,
                          std::max(image_desc->image_width, 1ul), std::max(image_desc->image_height, 1ul),
                          std::max(image_desc->image_depth, 1ul));
}

#ifdef CL_VERSION_2_0

cl_mem clCreatePipe(cl_context context, cl_mem_flags flags, cl_uint pipe_packet_size, cl_uint pipe_max_packets,
                    const cl_pipe_properties *properties, cl_int *errcode_ret) {
    return nullptr;
}

#endif

#ifdef CL_VERSION_3_0

cl_mem clCreateBufferWithProperties(cl_context context, const cl_mem_properties *properties, cl_mem_flags flags,
                                    size_t size, void *host_ptr, cl_int *errcode_ret) {
    return nullptr;
}

cl_mem clCreateImageWithProperties(cl_context context, const cl_mem_properties *properties, cl_mem_flags flags,
                                   const cl_image_format *image_format, const cl_image_desc *image_desc, void *host_ptr,
                                   cl_int *errcode_ret) {
    return nullptr;
}

#endif

cl_int clRetainMemObject(cl_mem memobj) {
    auto cmlMemory = cml::Memory::DownCast(memobj);

    if (!cmlMemory) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlMemory->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseMemObject(cl_mem memobj) {
    auto cmlMemory = cml::Memory::DownCast(memobj);

    if (!cmlMemory) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlMemory->Release();

    if (!cmlMemory->GetReferenceCount()) {
        delete cmlMemory;
    }

    return CL_SUCCESS;
}

cl_int clGetSupportedImageFormats(cl_context context, cl_mem_flags flags, cl_mem_object_type image_type,
                                  cl_uint num_entries, cl_image_format *image_formats, cl_uint *num_image_formats) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        return CL_INVALID_CONTEXT;
    }

    auto supportedImageFormats = cmlContext->GetSupportedImageFormats();

    if (image_formats) {
        memcpy(image_formats, supportedImageFormats.data(), sizeof(cl_image_format) * supportedImageFormats.size());
    }

    if (num_image_formats) {
        num_image_formats[0] = supportedImageFormats.size();
    }

    return CL_SUCCESS;
}

cl_int clGetMemObjectInfo(cl_mem memobj, cl_mem_info param_name, size_t param_value_size, void *param_value,
                          size_t *param_value_size_ret) {
    auto cmlMemory = cml::Memory::DownCast(memobj);

    if (!cmlMemory) {
        return CL_INVALID_MEM_OBJECT;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_MEM_TYPE:
            size = sizeof(cl_mem_object_type);
            ((cl_mem_object_type *) info)[0] = cmlMemory->GetType();
            break;
        case CL_MEM_FLAGS:
            size = sizeof(cl_mem_flags);
            ((cl_mem_flags *) info)[0] = cmlMemory->GetFlags();
            break;
        case CL_MEM_SIZE:
            size = sizeof(size_t);
            ((size_t *) info)[0] = cmlMemory->GetSize();
            break;
        case CL_MEM_HOST_PTR:
            size = sizeof(void *);
            memset(info, 0, size);
            break;
        case CL_MEM_MAP_COUNT:
            size = sizeof(cl_uint);
            ((cl_uint *) info)[0] = cmlMemory->GetMapCount();
            break;
        case CL_MEM_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            ((cl_uint *) info)[0] = cmlMemory->GetReferenceCount();
            break;
        case CL_MEM_CONTEXT:
            size = sizeof(cl_context);
            ((cl_context *) info)[0] = cmlMemory->GetContext();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

cl_int clGetImageInfo(cl_mem image, cl_image_info param_name, size_t param_value_size, void *param_value,
                      size_t *param_value_size_ret) {
    auto cmlImage = cml::Image::DownCast(image);

    if (!cmlImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_IMAGE_FORMAT:
            size = sizeof(cl_image_format);
            *((cl_image_format *) info) = cmlImage->GetFormat();
            break;
        case CL_IMAGE_ELEMENT_SIZE:
            size = sizeof(size_t);
            *((size_t *) info) = cml::Util::GetFormatSize(cmlImage->GetFormat());
            break;
        case CL_IMAGE_ROW_PITCH:
            size = sizeof(size_t);
            *((size_t *) info) = 0;
            break;
        case CL_IMAGE_SLICE_PITCH:
            size = sizeof(size_t);
            *((size_t *) info) = 0;
            break;
        case CL_IMAGE_WIDTH:
            size = sizeof(size_t);
            *((size_t *) info) = cmlImage->GetWidth();
            break;
        case CL_IMAGE_HEIGHT:
            size = sizeof(size_t);
            *((size_t *) info) = cmlImage->GetHeight();
            break;
        case CL_IMAGE_DEPTH:
            size = sizeof(size_t);
            *((size_t *) info) = cmlImage->GetDepth();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

#ifdef CL_VERSION_2_0

cl_int clGetPipeInfo(cl_mem pipe, cl_pipe_info param_name, size_t param_value_size, void *param_value,
                     size_t *param_value_size_ret) {
    return CL_INVALID_MEM_OBJECT;
}

#endif

cl_int clSetMemObjectDestructorCallback(cl_mem memobj, void (*pfn_notify)(cl_mem memobj, void *user_data),
                                        void *user_data) {
    return CL_INVALID_MEM_OBJECT;
}

/***********************************************************************************************************************
* SVM Allocation APIs
***********************************************************************************************************************/

#ifdef CL_VERSION_2_0

void *clSVMAlloc(cl_context context, cl_svm_mem_flags flags, size_t size, cl_uint alignment) {
    return nullptr;
}

void clSVMFree(cl_context context, void *svm_pointer) {
}

#endif

/***********************************************************************************************************************
* Sampler APIs
***********************************************************************************************************************/

#ifdef CL_VERSION_2_0

cl_sampler clCreateSamplerWithProperties(cl_context context, const cl_sampler_properties *sampler_properties,
                                         cl_int *errcode_ret) {
    return nullptr;
}

#endif

cl_int clRetainSampler(cl_sampler sampler) {
    auto cmlSampler = cml::Sampler::DownCast(sampler);

    if (!cmlSampler) {
        return CL_INVALID_SAMPLER;
    }

    cmlSampler->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseSampler(cl_sampler sampler) {
    auto cmlSampler = cml::Sampler::DownCast(sampler);

    if (!cmlSampler) {
        return CL_INVALID_SAMPLER;
    }

    cmlSampler->Release();

    if (!cmlSampler->GetReferenceCount()) {
        delete cmlSampler;
    }

    return CL_SUCCESS;
}

cl_int clGetSamplerInfo(cl_sampler sampler, cl_sampler_info param_name, size_t param_value_size, void *param_value,
                        size_t *param_value_size_ret) {
    auto cmlSampler = cml::Sampler::DownCast(sampler);

    if (!cmlSampler) {
        return CL_INVALID_SAMPLER;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_SAMPLER_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlSampler->GetReferenceCount();
            break;
        case CL_SAMPLER_CONTEXT:
            size = sizeof(cl_context);
            *((cl_context *) info) = cmlSampler->GetContext();
            break;
        case CL_SAMPLER_NORMALIZED_COORDS:
            size = sizeof(cl_bool);
            *((cl_bool *) info) = cmlSampler->GetNormalizedCoords();
            break;
        case CL_SAMPLER_ADDRESSING_MODE:
            size = sizeof(cl_addressing_mode);
            *((cl_addressing_mode *) info) = cmlSampler->GetAddressingMode();
            break;
        case CL_SAMPLER_FILTER_MODE:
            size = sizeof(cl_filter_mode);
            *((cl_filter_mode *) info) = cmlSampler->GetFilterMode();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Program Object APIs
***********************************************************************************************************************/

cl_program clCreateProgramWithSource(cl_context context, cl_uint count, const char **strings, const size_t *lengths,
                                     cl_int *errcode_ret) {
    if (!count) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    for (auto i = 0; i != count; ++i) {
        if (!strings[i]) {
            if (errcode_ret) {
                errcode_ret[0] = CL_INVALID_VALUE;
            }

            return nullptr;
        }
    }

    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    auto cmlProgram = new cml::Program(cmlContext);
    assert(cmlProgram);

    for (auto i = 0; i != count; ++i) {
        if (lengths && lengths[i]) {
            cmlProgram->AddSource({strings[i], lengths[i]});
        } else {
            cmlProgram->AddSource(strings[i]);
        }
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return cmlProgram;
}

cl_program clCreateProgramWithBinary(cl_context context, cl_uint num_devices, const cl_device_id *device_list,
                                     const size_t *lengths, const unsigned char **binaries, cl_int *binary_status,
                                     cl_int *errcode_ret) {

    if (!num_devices && !device_list) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (num_devices > 1) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_DEVICE;
        }

        return nullptr;
    }

    if (!lengths || !binaries) {
        if (binary_status) {
            binary_status[0] = CL_INVALID_BINARY;
        }

        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    if (!lengths[0] || !binaries[0]) {
        if (binary_status) {
            binary_status[0] = CL_INVALID_BINARY;
        }

        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }


    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    auto cmlProgram = new cml::Program(cmlContext);
    assert(cmlProgram);

    std::vector<uint32_t> binary(reinterpret_cast<const uint32_t *>(binaries[0]),
                                 reinterpret_cast<const uint32_t *>(binaries[0] + lengths[0]));

    if (binary.size() * sizeof(uint32_t) != lengths[0]) {
        if (binary_status) {
            binary_status[0] = CL_INVALID_BINARY;
        }

        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_BINARY;
        }

        return nullptr;
    }

    cmlProgram->SetBinary(binary);

    if (binary_status) {
        binary_status[0] = CL_SUCCESS;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return cmlProgram;
}

cl_program clCreateProgramWithBuiltInKernels(cl_context context, cl_uint num_devices, const cl_device_id *device_list,
                                             const char *kernel_names, cl_int *errcode_ret) {
    return nullptr;
}

#ifdef CL_VERSION_2_1

cl_program clCreateProgramWithIL(cl_context context, const void *il, size_t length, cl_int *errcode_ret) {
    return nullptr;
}

#endif

cl_int clRetainProgram(cl_program program) {
    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    cmlProgram->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseProgram(cl_program program) {
    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    cmlProgram->Retain();

    if (!cmlProgram->GetReferenceCount()) {
        delete cmlProgram;
    }

    return CL_SUCCESS;
}

cl_int clBuildProgram(cl_program program, cl_uint num_devices, const cl_device_id *device_list, const char *options,
                      void (*pfn_notify)(cl_program program, void *user_data), void *user_data) {
    if (num_devices && !device_list) {
        return CL_INVALID_VALUE;
    }

    if (!num_devices && device_list) {
        return CL_INVALID_VALUE;
    }

    if (!pfn_notify && user_data) {
        return CL_INVALID_VALUE;
    }

    for (auto i = 0; i != num_devices; ++i) {
        if (!cml::Device::DownCast(device_list[i])) {
            return CL_INVALID_DEVICE;
        }
    }

    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    if (options) {
        cmlProgram->SetOptions(options);
    }

    cmlProgram->Compile();

    if (cmlProgram->GetBuildStatus() != CL_BUILD_SUCCESS) {
        return CL_BUILD_PROGRAM_FAILURE;
    }

    cmlProgram->Reflect();

    return CL_SUCCESS;
}

cl_int clCompileProgram(cl_program program, cl_uint num_devices, const cl_device_id *device_list, const char *options,
                        cl_uint num_input_headers, const cl_program *input_headers, const char **header_include_names,
                        void (CL_CALLBACK *pfn_notify)(cl_program program, void *user_data), void *user_data) {
    return CL_INVALID_PROGRAM;
}

cl_program clLinkProgram(cl_context context, cl_uint num_devices, const cl_device_id *device_list, const char *options,
                         cl_uint num_input_programs, const cl_program *input_programs,
                         void (*pfn_notify)(cl_program program, void *user_data), void *user_data,
                         cl_int *errcode_ret) {
    return nullptr;
}

#ifdef CL_VERSION_2_2

cl_int clSetProgramReleaseCallback(cl_program program, void (*pfn_notify)(cl_program program, void *user_data),
                                   void *user_data) {
    return CL_INVALID_PROGRAM;
}

cl_int clSetProgramSpecializationConstant(cl_program program, cl_uint spec_id, size_t spec_size,
                                          const void *spec_value) {
    return CL_INVALID_PROGRAM;
}

#endif

cl_int clUnloadPlatformCompiler(cl_platform_id platform) {
    return CL_INVALID_PLATFORM;
}

cl_int clGetProgramInfo(cl_program program, cl_program_info param_name, size_t param_value_size, void *param_value,
                        size_t *param_value_size_ret) {
    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_PROGRAM_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlProgram->GetReferenceCount();
            break;
        case CL_PROGRAM_CONTEXT:
            size = sizeof(cl_context);
            *((cl_context *) info) = cmlProgram->GetContext();
            break;
        case CL_PROGRAM_NUM_DEVICES:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = 1;
            break;
        case CL_PROGRAM_DEVICES:
            size = sizeof(cl_device_id);
            *((cl_device_id *) info) = cmlProgram->GetContext()->GetDevice();
            break;
        case CL_PROGRAM_SOURCE:
            size = cmlProgram->GetSource().size() + 1;
            memcpy(info, cmlProgram->GetSource().data(), size);
            break;
        case CL_PROGRAM_BINARY_SIZES:
            size = sizeof(size_t);
            ((size_t *) info)[0] = sizeof(uint32_t) * cmlProgram->GetBinary().size();
            break;
        case CL_PROGRAM_BINARIES:
            size = sizeof(unsigned char **);
            if (param_value && param_value_size == size) {
                memcpy(((unsigned char **)param_value)[0], cmlProgram->GetBinary().data(),
                       sizeof(uint32_t) * cmlProgram->GetBinary().size());
                memcpy(info, param_value, size);
            }
            break;
        case CL_PROGRAM_NUM_KERNELS:
            size = sizeof(size_t);
            ((size_t *) info)[0] = cmlProgram->GetReflection().Arguments.size();
            break;
        case CL_PROGRAM_KERNEL_NAMES: {
            std::stringstream stream;
            for (auto &[name, arguments] : cmlProgram->GetReflection().Arguments) {
                stream << name << ';';
            }
            auto names = stream.str();
            names.pop_back();
            size = names.size() + 1;
            memcpy(info, names.data(), size);
            break;
        }
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

cl_int clGetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info param_name,
                             size_t param_value_size, void *param_value, size_t *param_value_size_ret) {
    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_PROGRAM_BUILD_STATUS:
            size = sizeof(cl_build_status);
            *((cl_build_status *) info) = cmlProgram->GetBuildStatus();
            break;
        case CL_PROGRAM_BUILD_OPTIONS:
            size = cmlProgram->GetOptions().size() + 1;
            memcpy(info, cmlProgram->GetOptions().data(), size);
            break;
        case CL_PROGRAM_BUILD_LOG:
            size = cmlProgram->GetLog().size() + 1;
            memcpy(info, cmlProgram->GetLog().data(), size);
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Kernel Object APIs
***********************************************************************************************************************/

cl_kernel clCreateKernel(cl_program program, const char *kernel_name, cl_int *errcode_ret) {
    if (!kernel_name) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_VALUE;
        }

        return nullptr;
    }

    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_PROGRAM;
        }

        return nullptr;
    }

    cml::Kernel *cmlKernel;

    try {
        cmlKernel = new cml::Kernel(cmlProgram, kernel_name);
    } catch (std::exception &e) {
        cmlKernel = nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = cmlKernel ? CL_SUCCESS : CL_INVALID_PROGRAM_EXECUTABLE;
    }

    return cmlKernel;
}

cl_int clCreateKernelsInProgram(cl_program program, cl_uint num_kernels, cl_kernel *kernels, cl_uint *num_kernels_ret) {
    if (!num_kernels && kernels) {
        return CL_INVALID_VALUE;
    }

    auto cmlProgram = cml::Program::DownCast(program);

    if (!cmlProgram) {
        return CL_INVALID_PROGRAM;
    }

    auto reflection = cmlProgram->GetReflection();

    if (kernels) {
        if (num_kernels < reflection.Arguments.size()) {
            return CL_INVALID_VALUE;
        }

        std::transform(reflection.Arguments.begin(), reflection.Arguments.end(), kernels, [cmlProgram](auto iter) {
            return new cml::Kernel(cmlProgram, iter.first);
        });
    }

    if (num_kernels_ret) {
        num_kernels_ret[0] = reflection.Arguments.size();
    }

    return CL_SUCCESS;
}

#ifdef CL_VERSION_2_1

cl_kernel clCloneKernel(cl_kernel source_kernel, cl_int *errcode_ret) {
    return nullptr;
}

#endif

cl_int clRetainKernel(cl_kernel kernel) {
    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    cmlKernel->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseKernel(cl_kernel kernel) {
    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    cmlKernel->Release();

    if (!cmlKernel->GetReferenceCount()) {
        delete cmlKernel;
    }

    return CL_SUCCESS;
}

cl_int clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void *arg_value) {
    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    auto cmlArgTable = cmlKernel->GetArgTable();

    if (arg_index >= cmlArgTable.size()) {
        return CL_INVALID_ARG_INDEX;
    }

    cmlKernel->SetArg(arg_index, arg_value, arg_size);

    return CL_SUCCESS;
}

#ifdef CL_VERSION_2_0

cl_int clSetKernelArgSVMPointer(cl_kernel kernel, cl_uint arg_index, const void *arg_value) {
    return CL_INVALID_KERNEL;
}

cl_int clSetKernelExecInfo(cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size,
                           const void *param_value) {
    return CL_INVALID_KERNEL;
}

#endif

cl_int clGetKernelInfo(cl_kernel kernel, cl_kernel_info param_name, size_t param_value_size, void *param_value,
                       size_t *param_value_size_ret) {
    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_KERNEL_FUNCTION_NAME:
            size = cmlKernel->GetName().size() + 1;
            memcpy(info, cmlKernel->GetName().data(), size);
            break;
        case CL_KERNEL_NUM_ARGS:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlKernel->GetArgTable().size();
            break;
        case CL_KERNEL_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            *((cl_uint *) info) = cmlKernel->GetReferenceCount();
            break;
        case CL_KERNEL_CONTEXT:
            size = sizeof(cl_context);
            *((cl_context *) info) = cmlKernel->GetContext();
            break;
        case CL_KERNEL_PROGRAM:
            size = sizeof(cl_program);
            *((cl_program *) info) = cmlKernel->GetProgram();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

cl_int clGetKernelArgInfo(cl_kernel kernel, cl_uint arg_indx, cl_kernel_arg_info param_name, size_t param_value_size,
                          void *param_value, size_t *param_value_size_ret) {
    return CL_INVALID_KERNEL;
}

cl_int clGetKernelWorkGroupInfo(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name,
                                size_t param_value_size, void *param_value, size_t *param_value_size_ret) {
    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    auto cmlDevice = cml::Device::DownCast(device);

    if (!cmlDevice) {
        return CL_INVALID_DEVICE;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_KERNEL_WORK_GROUP_SIZE:
            size = sizeof(size_t);
            *((size_t *) info) = cmlKernel->GetWorkGroupSize();
            break;
        case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
            size = sizeof(cml::Size);
            ((cml::Size *) info)[0] = cmlKernel->GetCompileWorkGroupSize();
            break;
        case CL_KERNEL_LOCAL_MEM_SIZE:
            size = sizeof(cl_ulong);
            *((cl_ulong *) info) = 0;
            break;
        case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
            size = sizeof(size_t);
            *((size_t *) info) = cmlKernel->GetWorkItemExecutionWidth();
            break;
        case CL_KERNEL_PRIVATE_MEM_SIZE:
            size = sizeof(cl_ulong);
            *((cl_ulong *) info) = 0;
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

#ifdef CL_VERSION_2_1

cl_int clGetKernelSubGroupInfo(cl_kernel kernel, cl_device_id device, cl_kernel_sub_group_info param_name,
                               size_t input_value_size, const void *input_value, size_t param_value_size,
                               void *param_value, size_t *param_value_size_ret) {
    return CL_INVALID_KERNEL;
}

#endif

/***********************************************************************************************************************
* Event Object APIs
***********************************************************************************************************************/

cl_int clWaitForEvents(cl_uint num_events, const cl_event *event_list) {
    if (!num_events || !event_list) {
        return CL_INVALID_VALUE;
    }

    for (auto i = 0; i != num_events; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlEvent->WaitComplete();
    }

    return CL_SUCCESS;
}

cl_int clGetEventInfo(cl_event event, cl_event_info param_name, size_t param_value_size, void *param_value,
                      size_t *param_value_size_ret) {
    auto cmlEvent = cml::Event::DownCast(event);

    if (!cmlEvent) {
        return CL_INVALID_EVENT;
    }

    size_t size;
    uint8_t info[2048];

    switch (param_name) {
        case CL_EVENT_COMMAND_QUEUE:
            size = sizeof(cl_command_queue);
            ((cl_command_queue *) info)[0] = cmlEvent->GetCommandQueue();
            break;
        case CL_EVENT_CONTEXT:
            size = sizeof(cl_context);
            ((cl_context *) info)[0] = cmlEvent->GetContext();
            break;
        case CL_EVENT_COMMAND_TYPE:
            size = sizeof(cl_command_type);
            ((cl_command_type *) info)[0] = 0;
            break;
        case CL_EVENT_COMMAND_EXECUTION_STATUS:
            size = sizeof(cl_int);
            ((cl_int *) info)[0] = cmlEvent->GetStatus();
            break;
        case CL_EVENT_REFERENCE_COUNT:
            size = sizeof(cl_uint);
            ((cl_uint *) info)[0] = cmlEvent->GetReferenceCount();
            break;
        default:
            return CL_INVALID_VALUE;
    }

    if (param_value) {
        if (param_value_size < size) {
            return CL_INVALID_VALUE;
        } else {
            memcpy(param_value, info, size);
        }
    }

    if (param_value_size_ret) {
        param_value_size_ret[0] = size;
    }

    return CL_SUCCESS;
}

cl_event clCreateUserEvent(cl_context context, cl_int *errcode_ret) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_EVENT;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Event(cmlContext);
}

cl_int clRetainEvent(cl_event event) {
    auto cmlEvent = cml::Event::DownCast(event);

    if (!cmlEvent) {
        return CL_INVALID_EVENT;
    }

    cmlEvent->Retain();

    return CL_SUCCESS;
}

cl_int clReleaseEvent(cl_event event) {
    auto cmlEvent = cml::Event::DownCast(event);

    if (!cmlEvent) {
        return CL_INVALID_EVENT;
    }

    cmlEvent->Release();

    if (!cmlEvent->GetReferenceCount()) {
        delete cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clSetUserEventStatus(cl_event event, cl_int execution_status) {
    if (execution_status != CL_COMPLETE && execution_status > 0) {
        return CL_INVALID_VALUE;
    }

    auto cmlEvent = cml::Event::DownCast(event);

    if (!cmlEvent) {
        return CL_INVALID_EVENT;
    }

    cmlEvent->SetStatus(execution_status);

    return CL_SUCCESS;
}

cl_int clSetEventCallback(cl_event event, cl_int command_exec_callback_type,
                          void (*pfn_notify)(cl_event event, cl_int event_command_status, void *user_data),
                          void *user_data) {
    if (!pfn_notify) {
        return CL_INVALID_VALUE;
    }

    if (!cml::Util::TestAnyFlagSet(command_exec_callback_type, CL_COMPLETE | CL_RUNNING | CL_SUBMITTED)) {
        return CL_INVALID_VALUE;
    }

    auto cmlEvent = cml::Event::DownCast(event);

    if (!cmlEvent) {
        return CL_INVALID_EVENT;
    }

    cmlEvent->SetCallback(command_exec_callback_type, [=](cl_int event_command_status) {
        pfn_notify(event, event_command_status, user_data);
    });

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Profiling APIs
***********************************************************************************************************************/

cl_int clGetEventProfilingInfo(cl_event event, cl_profiling_info param_name, size_t param_value_size, void *param_value,
                               size_t *param_value_size_ret) {
    return CL_INVALID_EVENT;
}

/***********************************************************************************************************************
* Flush and Finish APIs
***********************************************************************************************************************/

cl_int clFlush(cl_command_queue command_queue) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    cmlCommandQueue->Flush();

    return CL_SUCCESS;
}

cl_int clFinish(cl_command_queue command_queue) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    cmlCommandQueue->Flush();
    cmlCommandQueue->WaitIdle();

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* Enqueued Commands APIs
***********************************************************************************************************************/

cl_int clEnqueueReadBuffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, size_t offset,
                           size_t size, void *ptr, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                           cl_event *event) {
    if (!ptr) {
        return CL_INVALID_VALUE;
    }

    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlBuffer = cml::Buffer::DownCast(buffer);

    if (!cmlBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueReadBuffer(cmlBuffer, offset, ptr, size);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    if (blocking_read) {
        cmlCommandQueue->Flush();
        cmlCommandQueue->WaitIdle();
    }

    return CL_SUCCESS;
}

cl_int clEnqueueReadBufferRect(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read,
                               const size_t *buffer_origin, const size_t *host_origin, const size_t *region,
                               size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch,
                               size_t host_slice_pitch, void *ptr, cl_uint num_events_in_wait_list,
                               const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueWriteBuffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, size_t offset,
                            size_t size, const void *ptr, cl_uint num_events_in_wait_list,
                            const cl_event *event_wait_list, cl_event *event) {
    if (!ptr) {
        return CL_INVALID_VALUE;
    }

    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlBuffer = cml::Buffer::DownCast(buffer);

    if (!cmlBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueWriteBuffer(ptr, cmlBuffer, offset, size);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    if (blocking_write) {
        cmlCommandQueue->Flush();
        cmlCommandQueue->WaitIdle();
    }

    return CL_SUCCESS;
}

cl_int clEnqueueWriteBufferRect(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write,
                                const size_t *buffer_origin, const size_t *host_origin, const size_t *region,
                                size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch,
                                size_t host_slice_pitch, const void *ptr, cl_uint num_events_in_wait_list,
                                const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueFillBuffer(cl_command_queue command_queue, cl_mem buffer, const void *pattern, size_t pattern_size,
                           size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                           cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    auto cmlBuffer = cml::Buffer::DownCast(buffer);

    if (!cmlBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    cmlCommandQueue->EnqueueFillBuffer(pattern, pattern_size, cmlBuffer, offset, size);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueCopyBuffer(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, size_t src_offset,
                           size_t dst_offset, size_t size, cl_uint num_events_in_wait_list,
                           const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlSrcBuffer = cml::Buffer::DownCast(src_buffer);

    if (!cmlSrcBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    auto cmlDstBuffer = cml::Buffer::DownCast(dst_buffer);

    if (!cmlDstBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueCopyBuffer(cmlSrcBuffer, src_offset, cmlDstBuffer, dst_offset, size);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueCopyBufferRect(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer,
                               const size_t *src_origin, const size_t *dst_origin, const size_t *region,
                               size_t src_row_pitch, size_t src_slice_pitch, size_t dst_row_pitch,
                               size_t dst_slice_pitch, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                               cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueReadImage(cl_command_queue command_queue, cl_mem image, cl_bool blocking_read, const size_t *origin,
                          const size_t *region, size_t row_pitch, size_t slice_pitch, void *ptr,
                          cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    if (!ptr) {
        return CL_INVALID_VALUE;
    }

    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlImage = cml::Image::DownCast(image);

    if (!cmlImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    if (cmlImage->GetType() == CL_MEM_OBJECT_IMAGE2D) {
        if (origin[2] != 0 || region[2] != 1) {
            return CL_INVALID_VALUE;
        }
    }

    if (!row_pitch) {
        row_pitch = region[0] * cml::Util::GetFormatSize(cmlImage->GetFormat());
    }

    if (!slice_pitch) {
        slice_pitch = row_pitch * region[1];
    }

    cmlCommandQueue->EnqueueReadImage(cmlImage, {origin[0], origin[1], origin[2]}, {region[0], region[1], region[2]},
                                      ptr, row_pitch, slice_pitch);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    if (blocking_read) {
        cmlCommandQueue->Flush();
        cmlCommandQueue->WaitIdle();
    }

    return CL_SUCCESS;
}

cl_int clEnqueueWriteImage(cl_command_queue command_queue, cl_mem image, cl_bool blocking_write, const size_t *origin,
                           const size_t *region, size_t input_row_pitch, size_t input_slice_pitch, const void *ptr,
                           cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    if (!ptr) {
        return CL_INVALID_VALUE;
    }

    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlImage = cml::Image::DownCast(image);

    if (!cmlImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    if (cmlImage->GetType() == CL_MEM_OBJECT_IMAGE2D) {
        if (origin[2] != 0 || region[2] != 1) {
            return CL_INVALID_VALUE;
        }
    }

    if (!input_row_pitch) {
        input_row_pitch = region[0] * cml::Util::GetFormatSize(cmlImage->GetFormat());
    }

    if (!input_slice_pitch) {
        input_slice_pitch = input_row_pitch * region[1];
    }

    cmlCommandQueue->EnqueueWriteImage(ptr, input_row_pitch, input_slice_pitch, {region[0], region[1], region[2]},
                                       cmlImage, {origin[0], origin[1], origin[2]});

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    if (blocking_write) {
        cmlCommandQueue->Flush();
        cmlCommandQueue->WaitIdle();
    }

    return CL_SUCCESS;
}

cl_int clEnqueueFillImage(cl_command_queue command_queue, cl_mem image, const void *fill_color, const size_t *origin,
                          const size_t *region, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                          cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueCopyImage(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image, const size_t *src_origin,
                          const size_t *dst_origin, const size_t *region, cl_uint num_events_in_wait_list,
                          const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlSrcImage = cml::Image::DownCast(src_image);

    if (!cmlSrcImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    auto cmlDstImage = cml::Image::DownCast(dst_image);

    if (!cmlDstImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueCopyImage(cmlSrcImage, {src_origin[0], src_origin[1], src_origin[2]},
                                      {region[0], region[1], region[2]}, cmlDstImage,
                                      {dst_origin[0], dst_origin[1], dst_origin[2]});

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueCopyImageToBuffer(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_buffer,
                                  const size_t *src_origin, const size_t *region, size_t dst_offset,
                                  cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlImage = cml::Image::DownCast(src_image);

    if (!cmlImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    auto cmlBuffer = cml::Buffer::DownCast(dst_buffer);

    if (!cmlBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueCopyImageToBuffer(cmlImage, {src_origin[0], src_origin[1], src_origin[2]},
                                              {region[0], region[1], region[2]}, cmlBuffer, dst_offset);

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueCopyBufferToImage(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_image,
                                  size_t src_offset, const size_t *dst_origin, const size_t *region,
                                  cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlSrcBuffer = cml::Buffer::DownCast(src_buffer);

    if (!cmlSrcBuffer) {
        return CL_INVALID_MEM_OBJECT;
    }

    auto cmlDstImage = cml::Image::DownCast(dst_image);

    if (!cmlDstImage) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlCommandQueue->EnqueueCopyBufferToImage(cmlSrcBuffer, src_offset, {region[0], region[1], region[2]}, cmlDstImage,
                                              {dst_origin[0], dst_origin[1], dst_origin[2]});

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

void *clEnqueueMapBuffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_map, cl_map_flags map_flags,
                         size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                         cl_event *event, cl_int *errcode_ret) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_COMMAND_QUEUE;
        }

        return nullptr;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            if (errcode_ret) {
                errcode_ret[0] = CL_INVALID_EVENT;
            }

            return nullptr;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlBuffer = cml::Buffer::DownCast(buffer);

    if (!cmlBuffer) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_MEM_OBJECT;
        }

        return nullptr;
    }

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    if (blocking_map) {
        cmlCommandQueue->Flush();
        cmlCommandQueue->WaitIdle();
    }

    auto data = cmlBuffer->Map();

    if (errcode_ret) {
        errcode_ret[0] = data ? CL_SUCCESS : CL_MAP_FAILURE;
    }

    return static_cast<uint8_t *>(data) + offset;
}

void *clEnqueueMapImage(cl_command_queue command_queue, cl_mem image, cl_bool blocking_map, cl_map_flags map_flags,
                        const size_t *origin, const size_t *region, size_t *image_row_pitch, size_t *image_slice_pitch,
                        cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event,
                        cl_int *errcode_ret) {
    return nullptr;
}

cl_int clEnqueueUnmapMemObject(cl_command_queue command_queue, cl_mem memobj, void *mapped_ptr,
                               cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    auto cmlMemory = cml::Memory::DownCast(memobj);

    if (!cmlMemory) {
        return CL_INVALID_MEM_OBJECT;
    }

    cmlMemory->Unmap();

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueMigrateMemObjects(cl_command_queue command_queue, cl_uint num_mem_objects, const cl_mem *mem_objects,
                                  cl_mem_migration_flags flags, cl_uint num_events_in_wait_list,
                                  const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueNDRangeKernel(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim,
                              const size_t *global_work_offset, const size_t *global_work_size,
                              const size_t *local_work_size, cl_uint num_events_in_wait_list,
                              const cl_event *event_wait_list, cl_event *event) {
    if (work_dim > 3) {
        return CL_INVALID_WORK_DIMENSION;
    }

    if (global_work_offset) {
        return CL_INVALID_GLOBAL_OFFSET;
    }

    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    if (local_work_size) {
        if (cmlKernel->GetCompileWorkGroupSize() != cml::Size{0, 0, 0} &&
            cmlKernel->GetCompileWorkGroupSize() != cml::Util::ConvertToSize(work_dim, local_work_size)) {
            return CL_INVALID_WORK_GROUP_SIZE;
        }

        cmlCommandQueue->EnqueueDispatch(cmlKernel, cml::Util::ConvertToSize(work_dim, global_work_size),
                                         cml::Util::ConvertToSize(work_dim, local_work_size));
    } else {
        cmlCommandQueue->EnqueueDispatch(cmlKernel, cml::Util::ConvertToSize(work_dim, global_work_size));
    }

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

cl_int clEnqueueNativeKernel(cl_command_queue command_queue, void (*user_func)(void *), void *args, size_t cb_args,
                             cl_uint num_mem_objects, const cl_mem *mem_list, const void **args_mem_loc,
                             cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueMarkerWithWaitList(cl_command_queue command_queue, cl_uint num_events_in_wait_list,
                                   const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueBarrierWithWaitList(cl_command_queue command_queue, cl_uint num_events_in_wait_list,
                                    const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

#ifdef CL_VERSION_2_0

cl_int clEnqueueSVMFree(cl_command_queue command_queue, cl_uint num_svm_pointers, void *svm_pointers[],
                        void (*pfn_free_func)(cl_command_queue queue, cl_uint num_svm_pointers, void *svm_pointers[],
                                              void *user_data),
                        void *user_data, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                        cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueSVMMemcpy(cl_command_queue command_queue, cl_bool blocking_copy, void *dst_ptr, const void *src_ptr,
                          size_t size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                          cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueSVMMemFill(cl_command_queue command_queue, void *svm_ptr, const void *pattern, size_t pattern_size,
                           size_t size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                           cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueSVMMap(cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags, void *svm_ptr,
                       size_t size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueSVMUnmap(cl_command_queue command_queue, void *svm_ptr, cl_uint num_events_in_wait_list,
                         const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

#endif

#ifdef CL_VERSION_2_1

cl_int clEnqueueSVMMigrateMem(cl_command_queue command_queue, cl_uint num_svm_pointers, const void **svm_pointers,
                              const size_t *sizes, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list,
                              const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

#endif

void *clGetExtensionFunctionAddressForPlatform(cl_platform_id platform, const char *func_name) {
    return nullptr;
}

cl_int clSetCommandQueueProperty(cl_command_queue command_queue, cl_command_queue_properties properties, cl_bool enable,
                                 cl_command_queue_properties *old_properties) {
    return CL_INVALID_COMMAND_QUEUE;
}

/***********************************************************************************************************************
* Deprecated OpenCL 1.1 APIs
***********************************************************************************************************************/

cl_mem clCreateImage2D(cl_context context, cl_mem_flags flags, const cl_image_format *image_format, size_t image_width,
                       size_t image_height, size_t image_row_pitch, void *host_ptr, cl_int *errcode_ret) {
    if (!image_format) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_IMAGE_FORMAT_DESCRIPTOR;
        }

        return nullptr;
    }

    if (host_ptr && !cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR) && !host_ptr) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_MEM_OBJECT_ALLOCATION_FAILURE;
        }

        return nullptr;
    }

    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Image(cmlContext, flags, *image_format, CL_MEM_OBJECT_IMAGE2D, image_width, image_height, 1);
}

cl_mem clCreateImage3D(cl_context context, cl_mem_flags flags, const cl_image_format *image_format, size_t image_width,
                       size_t image_height, size_t image_depth, size_t image_row_pitch, size_t image_slice_pitch,
                       void *host_ptr, cl_int *errcode_ret) {
    if (!image_format) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_IMAGE_FORMAT_DESCRIPTOR;
        }

        return nullptr;
    }

    if (host_ptr && !cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR) && !host_ptr) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_HOST_PTR;
        }

        return nullptr;
    }

    if (cml::Util::TestAnyFlagSet(flags, CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR)) {
        if (errcode_ret) {
            errcode_ret[0] = CL_MEM_OBJECT_ALLOCATION_FAILURE;
        }

        return nullptr;
    }

    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Image(cmlContext, flags, *image_format, CL_MEM_OBJECT_IMAGE3D, image_width, image_height,
                          image_height);
}

cl_int clEnqueueMarker(cl_command_queue command_queue, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueWaitForEvents(cl_command_queue command_queue, cl_uint num_events, const cl_event *event_list) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueBarrier(cl_command_queue command_queue) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    return CL_SUCCESS;
}

cl_int clUnloadCompiler(void) {
    return CL_SUCCESS;
}

void *clGetExtensionFunctionAddress(const char *func_name) {
    return func_name ? cml::Dispatch::GetExtensionSymbol(func_name) : nullptr;
}

/***********************************************************************************************************************
* Deprecated OpenCL 2.0 APIs
***********************************************************************************************************************/

cl_command_queue clCreateCommandQueue(cl_context context, cl_device_id device, cl_command_queue_properties properties,
                                      cl_int *errcode_ret) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }

        return nullptr;
    }

    auto cmlDevice = cml::Device::DownCast(device);

    if (!cmlDevice || cmlDevice != cmlContext->GetDevice()) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_DEVICE;
        }

        return nullptr;
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::CommandQueue(cmlContext, cmlDevice, properties);
}

cl_sampler clCreateSampler(cl_context context, cl_bool normalized_coords, cl_addressing_mode addressing_mode,
                           cl_filter_mode filter_mode, cl_int *errcode_ret) {
    auto cmlContext = cml::Context::DownCast(context);

    if (!cmlContext) {
        if (errcode_ret) {
            errcode_ret[0] = CL_INVALID_CONTEXT;
        }
    }

    if (errcode_ret) {
        errcode_ret[0] = CL_SUCCESS;
    }

    return new cml::Sampler(cmlContext, normalized_coords, addressing_mode, filter_mode);
}

cl_int clEnqueueTask(cl_command_queue command_queue, cl_kernel kernel, cl_uint num_events_in_wait_list,
                     const cl_event *event_wait_list, cl_event *event) {
    auto cmlCommandQueue = cml::CommandQueue::DownCast(command_queue);

    if (!cmlCommandQueue) {
        return CL_INVALID_COMMAND_QUEUE;
    }

    for (auto i = 0; i != num_events_in_wait_list; ++i) {
        auto cmlEvent = cml::Event::DownCast(event_wait_list[i]);

        if (!cmlEvent) {
            return CL_INVALID_EVENT;
        }

        cmlCommandQueue->EnqueueWaitEvent(cmlEvent);
    }

    auto cmlKernel = cml::Kernel::DownCast(kernel);

    if (!cmlKernel) {
        return CL_INVALID_KERNEL;
    }

    cmlCommandQueue->EnqueueDispatch(cmlKernel, {1, 1, 1}, {1, 1, 1});

    if (event) {
        auto cmlEvent = new cml::Event(cmlCommandQueue);
        assert(cmlEvent);

        cmlCommandQueue->EnqueueSignalEvent(cmlEvent);
        event[0] = cmlEvent;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* OpenCL EGL APIs
***********************************************************************************************************************/

cl_mem clCreateFromEGLImageKHR(cl_context context, CLeglDisplayKHR egldisplay, CLeglImageKHR eglimage,
                               cl_mem_flags flags, const cl_egl_image_properties_khr *properties, cl_int *errcode_ret) {
    return nullptr;
}

cl_int clEnqueueAcquireEGLObjectsKHR(cl_command_queue command_queue, cl_uint num_objects, const cl_mem *mem_objects,
                                     cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                                     cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueReleaseEGLObjectsKHR(cl_command_queue command_queue, cl_uint num_objects, const cl_mem *mem_objects,
                                     cl_uint num_events_in_wait_list, const cl_event *event_wait_list,
                                     cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_event clCreateEventFromEGLSyncKHR(cl_context context, CLeglSyncKHR sync, CLeglDisplayKHR display,
                                     cl_int *errcode_ret) {
    return nullptr;
}

/***********************************************************************************************************************
* OpenCL Extension APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
* cl_khr_icd extension
***********************************************************************************************************************/

cl_int clIcdGetPlatformIDsKHR(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms) {
    if (!num_entries && platforms) {
        return CL_INVALID_VALUE;
    }

    if (!platforms && !num_platforms) {
        return CL_INVALID_VALUE;
    }

    if (!cml::Platform::GetSingleton()) {
        return CL_PLATFORM_NOT_FOUND_KHR;
    }

    if (platforms) {
        platforms[0] = cml::Platform::GetSingleton();
    }

    if (num_platforms) {
        num_platforms[0] = 1;
    }

    return CL_SUCCESS;
}

/***********************************************************************************************************************
* OpenCL OpenGL APIs
***********************************************************************************************************************/

cl_mem clCreateFromGLBuffer(cl_context context, cl_mem_flags flags, cl_GLuint bufobj, cl_int *errcode_ret) {
    return nullptr;
}

cl_mem clCreateFromGLTexture(cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
                             cl_GLuint texture, cl_int *errcode_ret) {
    return nullptr;
}

cl_mem clCreateFromGLRenderbuffer(cl_context context, cl_mem_flags flags, cl_GLuint renderbuffer, cl_int *errcode_ret) {
    return nullptr;
}

cl_int clGetGLObjectInfo(cl_mem memobj, cl_gl_object_type *gl_object_type, cl_GLuint *gl_object_name) {
    return CL_INVALID_MEM_OBJECT;
}

cl_int clGetGLTextureInfo(cl_mem memobj, cl_gl_texture_info param_name, size_t param_value_size, void *param_value,
                          size_t *param_value_size_ret) {
    return CL_INVALID_MEM_OBJECT;
}

cl_int clEnqueueAcquireGLObjects(cl_command_queue command_queue, cl_uint num_objects, const cl_mem *mem_objects,
                                 cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

cl_int clEnqueueReleaseGLObjects(cl_command_queue command_queue, cl_uint num_objects, const cl_mem *mem_objects,
                                 cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event) {
    return CL_INVALID_COMMAND_QUEUE;
}

/***********************************************************************************************************************
* Deprecated OpenCL 1.1 APIs
***********************************************************************************************************************/

cl_mem clCreateFromGLTexture2D(cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
                               cl_GLuint texture, cl_int *errcode_ret) {
    return nullptr;
}

cl_mem clCreateFromGLTexture3D(cl_context context, cl_mem_flags flags, cl_GLenum target, cl_GLint miplevel,
                               cl_GLuint texture, cl_int *errcode_ret) {
    return nullptr;
}

/***********************************************************************************************************************
* cl_khr_gl_sharing extension
***********************************************************************************************************************/

cl_int clGetGLContextInfoKHR(const cl_context_properties *properties, cl_gl_context_info param_name,
                             size_t param_value_size, void *param_value, size_t *param_value_size_ret) {
    return CL_INVALID_OPERATION;
}

/***********************************************************************************************************************
* cl_khr_gl_event extension
***********************************************************************************************************************/

cl_event clCreateEventFromGLsyncKHR(cl_context context, cl_GLsync sync, cl_int *errcode_ret) {
    return nullptr;
}
