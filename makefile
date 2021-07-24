tracker.o : struct.h tracker.h tracker.cpp
	g++ -g -c tracker.cpp `pkg-config --cflags --libs opencv4` -latomic
