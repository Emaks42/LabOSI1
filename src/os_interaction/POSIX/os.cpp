#include "os_api.h"
#include "os.h"
#include "logs.h"


using namespace os;

#define SYS_CALL_AND_LOG(call)                                                        \
    do {                                                                              \
        int RES = call;                                                              \
        if (RES == -1) {                                                           \
            std::stringstream error;                                                  \
            error << #call << " failed. Error code: 0x" << std::hex << (int)errno;    \
            LogErr("os/posix", error.str());                                          \
        }                                                                             \
    } while (0)

Process os::create_proc(ProcessStartInfo& info) {
    posix_spawn_file_actions_t spfact{};
    if ((info.redirectStdin.id != -1) || (info.redirectStdout.id != -1)) {

        if (info.redirectStdin.id != -1) {
            posix_spawn_file_actions_adddup2(&spfact, info.redirectStdin.id, STDIN_FILENO);
        }
        if (info.redirectStdout.id != -1) {
            posix_spawn_file_actions_adddup2(&spfact, info.redirectStdout.id, STDOUT_FILENO);
        }
    }
    char* argv[info.args.size()+2];
    argv[0] = info.cmd.data();
    for (std::size_t i = 1; i < info.args.size()+1; ++i) {
        argv[i] = info.args[i-1].data();
    }
    argv[info.args.size()+1] = (char*) 0;
    pid_t process_id{};

    Process proc{0,nullptr};
    int status = posix_spawn(&process_id, const_cast<char*>(info.cmd.data()), &spfact, NULL, argv, environ);
    if (status != 0) {
        std::stringstream error;
        error << "Create process failed, error: 0x" << std::hex << status;
        LogErr("os/posix", error.str());
        proc.pid = -1;
        return proc;
    }

    proc.pid = process_id;
    
    return proc;
}

void os::close_proc(Process process) {
    kill(process.pid, SIGTERM);
}

void os::kill_proc(Process process, uint32_t ret) {
    kill(process.pid, SIGKILL);
}

bool os::create_pipe(File& outReadPipe, File& outWritePipe) {
    int pipefd[2]{};
    SYS_CALL_AND_LOG(pipe(pipefd));
    outReadPipe.id = pipefd[0];
    outWritePipe.id = pipefd[1];
    return (outReadPipe.id + outWritePipe.id) > 0;
}

int os::pipe_write(File pipe, const void* buf, int count) {
    const int fd = pipe.id;
    int res = write(fd, buf, (size_t)count);
    if (res == -1) {
        std::stringstream error;
        error << "Pipe write failed, error: 0x" << std::hex << errno;
        LogErr("os/posix", error.str());
    }
    return res;
}

int os::pipe_read(File pipe, void* buf, int count) {
    const int fd = pipe.id;
    int res = read(fd, buf, (size_t)count);
    if (res == -1) {
        std::stringstream error;
        error << "Pipe read failed, error: 0x" << std::hex << errno;
        LogErr("os/posix", error.str());
    }
    return res;
}

int os::get_current_pid() {
    return getpid();
}

File os::get_stdin() {
    return File{STDIN_FILENO,nullptr};
}

File os::get_stdout() {
    return File{STDOUT_FILENO,nullptr};
}

void os::close_file(File f) {
    close(f.id);
}