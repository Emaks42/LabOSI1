#pragma once

#include "defines.h"
#include <functional>

namespace mt {
struct ThreadHandleDummy { };
typedef ThreadHandleDummy* ThreadHandle;

using FuncNoArgs = std::function<void()>;
typedef void (*ThreadFunc)(void*);

using ThreadID = uint64_t;
using ThreadNameType = std::string;

// Create/Terminate functions
ThreadHandle create_thread(ThreadFunc func, void* param = nullptr, const ThreadNameType& name = "", int stackSize = -1);
ThreadHandle create_thread(FuncNoArgs func, const ThreadNameType& name = "", int stackSize = -1);
int thread_terminate(ThreadHandle handle);
void destroy_thread_handle(ThreadHandle handle);

// Join/Detach
void thread_join(ThreadHandle handle, uint32_t msTimeout = -1);
void thread_detach(ThreadHandle handle);
};