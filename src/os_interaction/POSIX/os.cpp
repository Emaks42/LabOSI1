#include "os_api.h"
#include "os.h"
#include "logs.h"


using namespace os;

#define SYS_CALL_AND_LOG(call)                                                        \
    do {                                                                              \
        bool RES = call;                                                              \
        if (RES == false) {                                                           \
            std::stringstream error;                                                  \
            error << #call << " failed. Error code: 0x" << std::hex << errno;         \
            LogErr("os/posix", error.str());                                          \
        }                                                                             \
    } while (0)

bool os::create_proc(ProcessStartInfo& info)
{
    char* argv[info.args.size()];
    for (std::size_t i = 0; i < info.args.size(); ++i) {
        argv[i] = info.args[i].data();
    }
    pid_t process_id{};
    int status = posix_spawn(&process_id, const_cast<char*>(info.cmd.data()), NULL, NULL, argv, environ);
    if (status != 0) {
        std::stringstream error;
        error << "Create process failed, error: 0x" << std::hex << status;
        LogErr("os/posix", error.str());
    }
    return reinterpret_cast<ProcessHandle>(&process_id);
}

void os::close_proc(ProcessHandle process)
{
    kill(*reinterpret_cast<int*>(process), SIGTERM);
}

void os::kill_proc(ProcessHandle process, uint32_t ret)
{
    kill(*reinterpret_cast<int*>(process), SIGKILL);
}

bool os::create_pipe(FileHandle& outReadPipe, FileHandle& outWritePipe)
{
    int pipefd[2];
    SYS_CALL_AND_LOG(pipe(pipefd));
    outReadPipe = reinterpret_cast<FileHandle>(&pipefd[0]);
    outWritePipe = reinterpret_cast<FileHandle>(&pipefd[1]);
    return outReadPipe && outWritePipe;
}

int os::pipe_write(FileHandle pipe, const void* buf, int count)
{
    const int fd = *reinterpret_cast<int*>(pipe);
    int res = write(fd, buf, (size_t)count);
    if (res == -1) {
        std::stringstream error;
        error << "Pipe write failed, error: 0x" << std::hex << errno;
        LogErr("os/posix", error.str());
    }
    return res;
}

int os::pipe_read(FileHandle pipe, void* buf, int count)
{
    const int fd = *reinterpret_cast<int*>(pipe);
    int res = read(fd, buf, (size_t)count);
    if (res == -1) {
        std::stringstream error;
        error << "Pipe read failed, error: 0x" << std::hex << errno;
        LogErr("os/posix", error.str());
    }
    return res;
}