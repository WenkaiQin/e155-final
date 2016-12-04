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
//    int request_nbr;

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
//    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
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

