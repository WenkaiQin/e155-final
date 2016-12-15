/*
 * File Name: readInputCoordinate.c
 * Author: Yi Yang and Wenkai Qin
 * Date: Dec. 8. 2016
 * Intro: This file is the CGI program for the Robot WayPoint Planner
 *        project. It contains the the web input cgi C script to receive
 *        and parse the user input, and then send the user input to
 *        the positionCapture.cpp program.
 *        It uses sscanf to parse the inputs, and later used a message queue
 *        to send the data to the main program through a TCP socket.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main() {
    char *lenstr;
    char *input = malloc(30);
    int len, x_pos, y_pos;

    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    printf("%s%c%c\n","Content-Type:text/html;charset=iso-8859-1",13,10);
    lenstr = getenv("CONTENT_LENGTH");
    if (lenstr == NULL || sscanf(lenstr, "%d", &len) != 1) {
        printf("<P>Invalid input!");
    }
    else {
        fgets(input, len+1, stdin);
        sscanf(input, "x-pos=%d&y-pos=%d", &x_pos, &y_pos);
        printf("<p> X: %d, Y: %d\n\n", x_pos, y_pos);
    }
    char buffer[3];
    zmq_send(requester, input, 30, 0);
    zmq_recv(requester, buffer, 3, 0);
    printf("<p> Rec Msg: %s\n\n", buffer);

    zmq_close(requester);
    zmq_ctx_destroy(context);
    printf("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0;url=/robotcontrol.html\">");
    free(input);
    return 0;
}
