#pragma once

#include "defines.h"

using BYTE = unsigned char;

namespace os {
struct FileHandleDummy { };
typedef FileHandleDummy* FileHandle;

struct ProcessHandleDummy { };
typedef ProcessHandleDummy* ProcessHandle;

struct Process {
    int pid = -1;
    ProcessHandle handle = nullptr;
};

struct File {
    int id = -1;
    FileHandle handle = nullptr;
};

struct ProcessStartInfo {

    std::string cmd;
    std::vector<std::string> args;

    File redirectStdin = {-1,nullptr};
    File redirectStdout = {-1,nullptr};

    ProcessStartInfo() = default;
};

Process create_proc(ProcessStartInfo& args);

void close_proc(Process proc);

void kill_proc(Process proc, uint32_t ret = -1);

bool create_pipe(File& readPipe, File& writePipe);

int pipe_write(File pipe, const void* buf, int count);

int pipe_read(File pipe, void* buf, int count);

int get_current_pid();

File get_stdin();

File get_stdout();

void close_file(File f);
} // namespace