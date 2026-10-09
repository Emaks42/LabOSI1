#include "thread.h"

using namespace mt;

Thread::Thread(std::string name, ThreadFunc threadFunc, int stackSize)
    : name(std::move(name))
    , func(threadFunc)
    , stackSize(stackSize)
{
}

Thread::~Thread()
{
    stop();
}

void Thread::run(void* context)
{
    stop();
    handle = create_thread(func, context, name, stackSize);
}

void Thread::stop()
{
    if (handle) {
        int ret = thread_terminate(handle);
        if (ret) {
            std::string err;
            err.resize(64);
            sprintf(err.data(), "Failed to terminate thread `%s`: 0x%x", name.c_str(), ret);
            LogErr("os/thread", err);
        }
        handle = nullptr;
    }
}

void Thread::join(uint32_t msTimeOut)
{
    thread_join(handle, msTimeOut);
}
