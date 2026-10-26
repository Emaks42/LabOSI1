#include <iostream>
#include <vector>
#include "os.h"

using namespace os;

int main() {
    std::vector<char> prev_c{0}, c{0};
    File input = get_stdin();
    File output = get_stdout();
    while (1) {
        prev_c[0] = c[0];
        os::pipe_read(input, c.data(), 1);
        if (c[0] == 1) {
            break;
        }
        if (!((c[0] == prev_c[0]) && (c[0] == ' '))) {
            os::pipe_write(output, c.data(), 1);
        }
    }
    return 0;
}