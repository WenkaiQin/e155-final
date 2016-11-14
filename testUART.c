#include "EasyPIO.h"

int main() {
    pioInit();

    uartInit(600);
    while (1) {
        
        char c = '6';
        putCharSerial(c);
       // delayMicros(50000);
        break;
    }
    return 0;
}
