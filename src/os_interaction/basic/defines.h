#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
    #define OS_WIN
    #define _USE_MATH_DEFINES
    #define NOMINMAX
    #include <Windows.h>
    #include <corecrt_math_defines.h>
    #include <windows.h>
    #include <processthreadsapi.h>
#elif defined(__unix__)
    #define OS_LINUX
    #include <stdint.h>
    #include <memory>
    #include <unistd.h>
    #include <errno.h>
    #include <signal.h>
    #include <spawn.h>
#endif

#define all(v) v.begin(), v.end()