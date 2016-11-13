CC = g++
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: positionCapture

positionCapture:
	$(CC) $(CFLAGS) $(LIBS) positionCapture.cpp -o positionCapture

clean:
	rm positionCapture

