// This piece of code is inspired by the answer on stack overflow
// www.stackoverflow.com/questions/6947413

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "EasyPIO.h"

int set_interface_attribs(int fd, int speed) {
    struct termios tty;
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, (speed_t) speed);
    cfsetispeed(&tty, (speed_t) speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    tcsetattr(fd, TCSANOW, &tty);
    return 0;
}

int setupUSB() {
    char *portname = "/dev/ttyUSB0";
    int fd;

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    set_interface_attribs(fd, B9600);
    pioInit();
    return fd;
}

void tearDownUSB(int fd) {
    close(fd);
    return;
}

void writeByte(char *b, int fd) {
    char byte[1];
    char startByte[1];
    char stopByte[1];

    write(fd, b, 1);

//    printf("Bytes sending: %d\n", byte); 
    tcdrain(fd);
    return;    
} 

