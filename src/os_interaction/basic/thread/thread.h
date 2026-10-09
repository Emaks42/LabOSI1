#pragma once

#include "../defines.h"
#include "../logs.h"
#include "../mt.h"

namespace mt {
class Thread {
public:
    static constexpr int defaultStackSize = 128 * 1024;

    Thread(std::string name, ThreadFunc threadFunc, int stackSize = defaultStackSize);
    ~Thread();

    void run(void* context);
    void stop();
    void join(uint32_t msTimeOut = -1);

private:
    std::string name;
    ThreadFunc func;
    int stackSize;
    ThreadHandle handle = nullptr;
};
};