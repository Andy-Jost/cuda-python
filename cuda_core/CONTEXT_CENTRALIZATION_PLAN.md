# Plan: Centralize Context Code into _context.pyx

## Current State

### Context-related code scattered across modules:

1. **`_context.pyx`**: Minimal Python Context class
   - `Context._from_ctx()` factory method
   - No Cython-level context operations

2. **`_device.pyx`**: 
   - `_get_primary_context()` - Gets primary context for a device
   - `Device._get_current_context()` - Gets current context with validation
   - `Device.set_current()` - Sets context (uses primary or provided context)
   - `Device.context` property - Returns current context

3. **`_stream.pyx`**:
   - `Stream._get_context()` - Gets context from stream via `cuStreamGetCtx`
   - `Stream.context` property - Returns stream's context

4. **`_utils/cuda_utils.pyx`**:
   - `get_device_from_ctx()` - Context switching logic for device queries

### CUDA Driver API calls used:
- `cuDevicePrimaryCtxRetain` - Get primary context
- `cuCtxGetCurrent` - Get current context
- `cuCtxSetCurrent` - Set current context
- `cuCtxPushCurrent` - Push context onto stack
- `cuCtxPopCurrent` - Pop context from stack
- `cuStreamGetCtx` - Get context from stream

## Proposed Changes

### 1. Create `_context.pxd` file
   - Declare `cdef class Context` for Cython imports
   - Export Cython-level functions for context operations

### 2. Enhance `_context.pyx` with Cython-level APIs

   **Module-level functions (for use in Cython code):**
   - `cdef cydriver.CUcontext get_primary_context(int dev_id) except?NULL`
     - Move from `_device.pyx::_get_primary_context()`
     - Uses thread-local storage for caching
   
   - `cdef cydriver.CUcontext get_current_context() except?NULL`
     - Wraps `cuCtxGetCurrent`
     - Returns NULL if no context
   
   - `cdef void set_current_context(cydriver.CUcontext ctx) except *`
     - Wraps `cuCtxSetCurrent`
   
   - `cdef cydriver.CUcontext push_context(cydriver.CUcontext ctx) except?NULL`
     - Wraps `cuCtxPushCurrent`
     - Returns previous context (or NULL)
   
   - `cdef cydriver.CUcontext pop_context() except?NULL`
     - Wraps `cuCtxPopCurrent`
     - Returns popped context
   
   - `cdef cydriver.CUcontext get_stream_context(cydriver.CUstream stream) except?NULL`
     - Wraps `cuStreamGetCtx`
     - Gets context associated with a stream

   **Context class methods (for Python API):**
   - Keep existing `Context._from_ctx()` factory method
   - Add `@classmethod Context from_primary(int device_id)` - Create Context from primary context
   - Add `@classmethod Context current()` - Get current context
   - Add `@classmethod Context from_stream(Stream stream)` - Get context from stream
   - Add `cpdef set_current(self)` - Set this context as current
   - Add `cpdef push(self)` - Push this context onto stack
   - Add `cpdef Context pop(self)` - Pop this context from stack

### 3. Update `_device.pyx`
   - Remove `_get_primary_context()` function
   - Update `Device.set_current()` to call `get_primary_context()` from context module
   - Update `Device._get_current_context()` to call `get_current_context()` from context module
   - Update `Device.context` property to use context module functions
   - Import Context from `_context.pxd` instead of Python import

### 4. Update `_stream.pyx`
   - Remove `Stream._get_context()` method
   - Update `Stream.context` property to call `get_stream_context()` from context module
   - Import Context from `_context.pxd` instead of Python import

### 5. Update `_utils/cuda_utils.pyx`
   - Consider moving `get_device_from_ctx()` to context module (or keep as utility)
   - Update to use context module functions for push/pop operations

## File Structure After Changes

```
_context.pxd:
  - cdef class Context declaration
  - cdef function declarations for context operations

_context.pyx:
  - Thread-local storage for primary context cache
  - Cython-level functions (get_primary_context, get_current_context, etc.)
  - Context class with Python API methods
  - Factory methods for creating Context objects

_device.pyx:
  - Uses context module functions instead of implementing them
  - Imports from _context.pxd

_stream.pyx:
  - Uses context module functions instead of implementing them
  - Imports from _context.pxd
```

## Benefits

1. **Single source of truth** for context operations
2. **Easier to maintain** - all context logic in one place
3. **Better encapsulation** - Device/Stream don't need to know context internals
4. **Consistent API** - All context operations follow same patterns
5. **Easier to extend** - Adding new context operations is centralized
6. **Cython-level access** - Other modules can use Cython functions directly

## Migration Steps

1. Create `_context.pxd` with Context declaration
2. Move `_get_primary_context()` to `_context.pyx` as `get_primary_context()`
3. Add other Cython-level context functions to `_context.pyx`
4. Update `_device.pyx` to import and use context module functions
5. Update `_stream.pyx` to import and use context module functions
6. Test all context-related functionality
7. Update any other modules that directly call context APIs
