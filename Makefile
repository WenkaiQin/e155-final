CC = g++
CCC = gcc
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: positionCapture readInputCoordinate testZMQ

test: testZMQ

positionCapture:
	$(CC) $(CFLAGS) $(LIBS) positionCapture.cpp -o positionCapture

readInputCoordinate:
	$(CCC) -Wall -g readInputCoordinate.c -lzmq -o readInputCoordinate

testZMQ:
	$(CC) -Wall -g testZMQ.cpp -lzmq -o testZMQ
#writeToUSB:
#	$(CC) $(CFLAGS) writeToUSB.cpp -o writeToUSB

clean:
	rm positionCapture readInputCoordinate testZMQ
	rm *o


