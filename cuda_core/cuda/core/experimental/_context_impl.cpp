// SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0

#include "include/resource_handles.hpp"
#include <cuda.h>

namespace cuda_core {

ContextH create_context_handle(CUcontext ctx) {
    // Allocate the box containing the context resource
    ContextBox* box = new ContextBox();
    box->resource = ctx;

    // Use default deleter - it will delete the box, but not touch the CUcontext
    // CUcontext lifetime is managed by CUDA driver, not by us
    std::shared_ptr<const ContextBox> box_ptr(box);

    // Use aliasing constructor to create handle that exposes only CUcontext
    // The handle's reference count is tied to box_ptr, but it points to &box_ptr->resource
    return ContextH(box_ptr, &box_ptr->resource);
}

}  // namespace cuda_core
