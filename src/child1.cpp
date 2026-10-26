#include <iostream>
#include <vector>
#include "os.h"

using namespace os;

int main() {
    std::vector<char> c{0};
    File input = get_stdin();
    File output = get_stdout();
    while (1) {
        os::pipe_read(input, c.data(), 1);
        if (c[0] == 1) {
            os::pipe_write(output, c.data(), 1);
            break;
        }
        if (('A' <= c[0]) && (c[0] <= 'Z')) {
            c[0] += 'a' - 'A';
        }
        os::pipe_write(output, c.data(), 1);
    }    
    return 0;
}