#include <iostream>
#include <cstring>
#include "os_interaction/basic/os.h"

using namespace os;

int main() {
    File pipe1w, pipe1r;
    create_pipe(pipe1r, pipe1w);
    File pipe2w, pipe2r;
    create_pipe(pipe2r, pipe2w);
    File pipe3w, pipe3r;
    create_pipe(pipe3r, pipe3w);
    ProcessStartInfo proc_info1, proc_info2;
    proc_info1.cmd = "./lab1_exe_child1";
    proc_info2.cmd = "./lab1_exe_child2";
    proc_info1.redirectStdin = pipe1r;
    proc_info1.redirectStdout = pipe2w;
    proc_info2.redirectStdin = pipe2r;
    proc_info2.redirectStdout = pipe3w;
    Process child1 = create_proc(proc_info1);
    Process child2 = create_proc(proc_info2);
    std::string inp;
    bool running = true;
    while (running) {
        inp = "";
        char c{};
        while (c != '\n') {
            c = getchar();
            inp.push_back(c);
            if (inp == "exit\n") {
                running = false;
                break;
            }
        }
        if (!running) {
            inp[0] = 1;
            pipe_write(pipe1w, inp.data(), 1);
            break;
        }
        pipe_write(pipe1w, inp.data(), inp.size());
        char result{};
        while (result != '\n') {
            pipe_read(pipe3r, &result, 1);
            std::cout << result;
        }   
    }
    kill_proc(child1);
    kill_proc(child2);
    close_file(pipe1r); close_file(pipe1w);
    close_file(pipe2r); close_file(pipe2w);
    close_file(pipe3r); close_file(pipe3w);
    return 0;
}