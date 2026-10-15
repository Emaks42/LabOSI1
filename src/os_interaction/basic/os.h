#pragma once

#include "defines.h"

using BYTE = unsigned char;

namespace os {
struct FileHandleDummy { };
typedef FileHandleDummy* FileHandle;

struct ProcessHandleDummy { };
typedef ProcessHandleDummy* ProcessHandle;

struct ProcessStartInfo {

    std::string cmd;
    std::vector<std::string> args;

    ProcessStartInfo() = default;
};

std::string os::generate_app_cmd(const char* cmd, const std::vector<std::string>& args);

bool create_proc(const ProcessStartInfo& args);

void close_proc(ProcessHandle proc);

void kill_proc(ProcessHandle proc, uint32_t ret = -1);

bool create_pipe(FileHandle& readPipe, FileHandle& writePipe);

int pipe_write(FileHandle pipe, const void* buf, int count);

int pipe_read(FileHandle pipe, void* buf, int count);
} // namespace