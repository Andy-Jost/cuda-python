// SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <cuda.h>

namespace cuda_core {

// Forward declarations
struct ContextBox;

// Handle type aliases - expose only the raw CUDA resource
using ContextH = std::shared_ptr<const CUcontext>;

// Internal box structure for Context
// This holds the resource and any dependencies needed for lifetime management
struct ContextBox {
    CUcontext resource;
    // Context doesn't depend on other CUDA resources, but we keep the structure
    // extensible for future needs
};

// Function to create a context handle
// This will be implemented in the .cpp file
ContextH create_context_handle(CUcontext ctx);

}  // namespace cuda_core
