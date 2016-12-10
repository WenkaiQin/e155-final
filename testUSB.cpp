#include "writeToUSB.hpp"
#include "EasyPIO.h"
#include <time.h>

int main() {
    int fd = setupUSB();
    int i = 0;
    pioInit();
    while (true) {
        char c = '<';
       // writeByte(&c, fd);
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

