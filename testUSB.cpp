#include "writeToUSB.hpp"
#include "EasyPIO.h"
#include <time.h>

int main() {
    int fd = setupUSB();
    int i = 0;
    pioInit();
    while ( i < 8) {
        delayMillis(500);
        writeByte(i, fd);
        ++i;
    }
    tearDownUSB(fd);
    return 0;
}

