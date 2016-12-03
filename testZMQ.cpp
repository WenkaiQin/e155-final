#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;
        socket.recv(&request);
        
        string rq = string(static_cast<char*>(request.data()), request.size());

        cout << rq << endl;
//        sleep(1);

        zmq::message_t reply(3);
        memcpy(reply.data(), "Rec", 3);
        socket.send(reply);
    }
    return 0;
}

