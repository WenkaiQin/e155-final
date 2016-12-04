#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");
    string rq;
    rq = "No Msg";
    while (true) {
        zmq::message_t request;
        //zmqmsg_init(&request);

        try {
            socket.recv(&request, ZMQ_DONTWAIT);
            rq = string(static_cast<char*>(request.data()), request.size());
//            cout << rq << endl;
            zmq::message_t reply(3);
            memcpy(reply.data(), "Rec", 3);
            socket.send(reply);
        }
        catch (zmq::error_t e) {
            cout << "Didn't receive msgs" << endl;
            //continue;
        }
        sleep(1);        
        cout << rq << endl;
    }
    return 0;
}

