#include "writeToUSB.hpp"
#include "EasyPIO.h"
#include <time.h>

int main() {
    int fd = setupUSB();
    int i = 0;
    pioInit();
    while (true) {
        delayMillis(500);
        char c = '<';
        writeByte(&c, fd);
        delayMillis(500);
        c = 24;
        writeByte(&c, fd);
        ++i;
        if (i == 16) {
            i = 0;
        }
    }
    tearDownUSB(fd);
    return 0;
}

