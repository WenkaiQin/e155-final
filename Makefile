CC = g++
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: positionCapture writeToUSB

positionCapture:
	$(CC) $(CFLAGS) $(LIBS) positionCapture.cpp -o positionCapture

writeToUSB:
	$(CC) $(CFLAGS) writeToUSB.cpp -o writeToUSB

clean:
	rm positionCapture
	rm writeToUSB

