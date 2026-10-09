#include "mt.h"
#include "./os_api.h"

using namespace mt;

struct CreateThread_DummyProc_Args {
    ThreadFunc func;
    void* param;
};

pthread_t create_thread_dummy_proc(void* args_)
{
    CreateThread_DummyProc_Args* argsPtr = reinterpret_cast<CreateThread_DummyProc_Args*>(args_);
    CreateThread_DummyProc_Args args = *argsPtr;
    delete argsPtr;

    args.func(args.param);
}

void SetThreadName(ThreadID id, const ThreadNameType& name){}

ThreadHandle mt::create_thread(ThreadFunc func, void* param, const ThreadNameType& name, int stackSize)
{

    CreateThread_DummyProc_Args* args = new CreateThread_DummyProc_Args;
    args->func = func;
    args->param = param;

    if (stackSize == -1) {
        stackSize = 0; // default
    }

    pthread_t threadID;
    HANDLE hThread = pthread_create(&threadID, NULL, *func, );

    if (hThread != nullptr) {
        ThreadNameType buffer;
        if (name != "") {
            buffer = name;
        } else {
            buffer = sprintf(buffer.data(), "Thread<%lu>", threadID);
        }

        SetThreadName(threadID, buffer);
    }

    return reinterpret_cast<ThreadHandle>(hThread);
}

static void create_thread_func_no_args_proc(FuncNoArgs* func_)
{
    std::unique_ptr<FuncNoArgs> func(func_);

    (*func)();
}

ThreadHandle mt::create_thread(FuncNoArgs func, const ThreadNameType& name, int stackSize)
{
    FuncNoArgs* funcArg = new FuncNoArgs(std::move(func));
    return mt::create_thread((ThreadFunc)&create_thread_func_no_args_proc, (void*)funcArg, name, stackSize);
}

int mt::thread_terminate(ThreadHandle handle)
{
    BOOL ret = TerminateThread(handle, -1);
    destroy_thread_handle(handle);
    if (!ret) {
        return static_cast<int>(GetLastError());
    }
    return 0;
}

void mt::destroy_thread_handle(ThreadHandle handle)
{
    CloseHandle(handle);
}

void mt::thread_join(ThreadHandle handle, uint32_t msTimeout)
{
    WaitForSingleObject(handle, msTimeout);
}

void mt::thread_detach(ThreadHandle handle)
{
    CloseHandle(handle);
}
