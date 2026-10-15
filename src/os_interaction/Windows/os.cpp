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

bool os::create_proc(const ProcessStartInfo& info)
{
    STARTUPINFO cif;
    ZeroMemory(&cif, sizeof(STARTUPINFO));
    BOOL redirectHandles = FALSE;
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

    if (pi.hProcess == INVALID_HANDLE_VALUE) {
        std::stringstream error;
        error << "Create process return invalid process handle, error: 0x" << std::hex << GetLastError();
        LogErr("os/win_32", error.str());
        return nullptr;
    }

    return reinterpret_cast<ProcessHandle>(pi.hProcess);
}

std::string os::generate_app_cmd(const char* cmd, const std::vector<std::string>& args)
{
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

void os::close_proc(ProcessHandle process)
{
    CloseHandle(process);
}

void os::kill_proc(ProcessHandle process, uint32_t ret)
{
    TerminateProcess(process, static_cast<DWORD>(ret));
}

bool os::create_pipe(FileHandle& outReadPipe, FileHandle& outWritePipe)
{
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE pipeRead, pipeWrite;
    SYS_CALL_AND_LOG(CreatePipe(&pipeRead, &pipeWrite, &sa, 0));
    outReadPipe = reinterpret_cast<FileHandle>(pipeRead);
    outWritePipe = reinterpret_cast<FileHandle>(pipeWrite);
    return outReadPipe && outWritePipe;
}

int os::pipe_write(FileHandle pipe, const void* buf, int count)
{
    DWORD countWritten;
    BOOL RES{};
    do {
        BOOL RES = WriteFile(pipe, buf, count, &countWritten, nullptr);
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

int os::pipe_read(FileHandle pipe, void* buf, int count)
{
    DWORD countRead;
    BOOL RES{};
    do {
        BOOL RES = ReadFile(pipe, buf, count, &countRead, nullptr);
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