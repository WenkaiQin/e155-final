CC = g++
CCC = gcc
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: positionCapture readInputCoordinate testZMQ

test: testZMQ testUSB

positionCapture:
	$(CC) -O3 -Wall $(CFLAGS) $(LIBS) positionCapture.cpp -lzmq -o positionCapture

readInputCoordinate:
	$(CCC) -Wall -g readInputCoordinate.c -lzmq -o readInputCoordinate

testZMQ:
	$(CC) -Wall -g testZMQ.cpp -lzmq -o testZMQ
testUSB:
	$(CC) -Wall -g testUSB.cpp -o testUSB

clean:
	rm positionCapture readInputCoordinate testZMQ testUSB


