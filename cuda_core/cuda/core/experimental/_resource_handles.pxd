# SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0

from libcpp.memory cimport shared_ptr

from cuda.bindings cimport cydriver

# Declare the C++ namespace and types
cdef extern from "include/resource_handles.hpp" namespace "cuda_core":
    # Handle type - shared_ptr to const CUcontext
    ctypedef shared_ptr[const cydriver.CUcontext] ContextH

    # Function to create a context handle
    ContextH create_context_handle(cydriver.CUcontext ctx)
