#include "./os_api.h"
#include "os.h"
#include "logs.h"


using namespace os;

#define SYS_CALL_AND_LOG(call)                                                         \
    do {                                                                               \
        BOOL RES = call;                                                               \
        if (RES == FALSE) {                                                            \
            std::stringstream error;                                                   \
            error << #call << " failed. Error code: 0x" << std::hex << GetLastError(); \
            LogErr("os/win_32", error.str());                                          \
        }                                                                              \
    } while (0)

std::string generate_app_cmd(const char* cmd, const std::vector<std::string>& args) {
    static constexpr const char SEPARATOR = ' ';

    std::stringstream stream;
    stream << cmd;
    for (const std::string& arg : args) {
        bool needWrap = std::find(all(arg), SEPARATOR) != arg.end();

        if (needWrap) {
            stream << SEPARATOR << "\"" << arg << "\"";
        } else {
            stream << SEPARATOR << arg;
        }
    }
    return stream.str();
}

Process os::create_proc(ProcessStartInfo& info) {
    STARTUPINFO cif;
    ZeroMemory(&cif, sizeof(STARTUPINFO));

    BOOL redirectHandles = FALSE;
    if (info.redirectStdin.handle || info.redirectStdout.handle) {
        cif.dwFlags |= STARTF_USESTDHANDLES;
        redirectHandles = TRUE;

        if (info.redirectStdin.handle) {
            cif.hStdInput = info.redirectStdin.handle;
        }
        if (info.redirectStdout.handle) {
            cif.hStdOutput = info.redirectStdout.handle;
        }
    }

    BOOL isSuccess = FALSE;
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    std::string cmd = generate_app_cmd(info.cmd.c_str(), info.args);

    DWORD creationFlags = CREATE_DEFAULT_ERROR_MODE;
    creationFlags |= CREATE_NO_WINDOW;

    BOOL res = CreateProcess(nullptr,
        const_cast<WCHAR*>(std::wstring(cmd.begin(),cmd.end()).data()),
        nullptr,
        nullptr,
        redirectHandles,
        creationFlags,
        nullptr,
        nullptr,
        &cif,
        &pi);

    if (res == FALSE) {
        std::stringstream error;
        error << "Create process failed, error: 0x" << std::hex << GetLastError();
        LogErr("os/win_32", error.str());
    }

    if (pi.hThread != nullptr && pi.hThread != INVALID_HANDLE_VALUE) {
        CloseHandle(pi.hThread);
    }
    Process proc{0,nullptr};

    if (pi.hProcess == INVALID_HANDLE_VALUE) {
        std::stringstream error;
        error << "Create process return invalid process handle, error: 0x" << std::hex << GetLastError();
        LogErr("os/win_32", error.str());
        return proc;
    }

    proc.handle = reinterpret_cast<ProcessHandle>(pi.hProcess);

    return proc;
}

void os::close_proc(Process process) {
    CloseHandle(process.handle);
}

void os::kill_proc(Process process, uint32_t ret) {
    TerminateProcess(process.handle, static_cast<DWORD>(ret));
}

bool os::create_pipe(File& outReadPipe, File& outWritePipe) {
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE pipeRead, pipeWrite;
    SYS_CALL_AND_LOG(CreatePipe(&pipeRead, &pipeWrite, &sa, 0));
    outReadPipe.handle = reinterpret_cast<FileHandle>(pipeRead);
    outWritePipe.handle = reinterpret_cast<FileHandle>(pipeWrite);
    return outReadPipe.handle && outWritePipe.handle;
}

int os::pipe_write(File pipe, const void* buf, int count) {
    DWORD countWritten;
    BOOL RES{};
    do {
        BOOL RES = WriteFile(pipe.handle, buf, count, &countWritten, nullptr);
        if (RES == FALSE) {
            std::stringstream error;
            error << "WriteFile failed. Error code: 0x" << std::hex << GetLastError();
            LogErr("os/win_32", error.str());
        }
    } while (0);
    if (RES) {
        return countWritten;
    }
    return -1;
}

int os::pipe_read(File pipe, void* buf, int count) {
    DWORD countRead;
    BOOL RES{};
    do {
        BOOL RES = ReadFile(pipe.handle, buf, count, &countRead, nullptr);
        if (RES == FALSE) {
            std::stringstream error;
            error << "ReadFile failed. Error code: 0x" << std::hex << GetLastError();
            LogErr("os/win_32", error.str());
        }
    } while (0);
    if (RES) {
        return countRead;
    }
    return -1;
}

int os::get_current_pid() {
    return (int)GetCurrentProcessId();
}

File os::get_stdin() {
    return File{0, (FileHandle)GetStdHandle(STD_INPUT_HANDLE)};
}

File os::get_stdout() {
    return File{0, (FileHandle)GetStdHandle(STD_OUTPUT_HANDLE)};
}

void os::close_file(File f) {
    CloseHandle(f.handle);
}