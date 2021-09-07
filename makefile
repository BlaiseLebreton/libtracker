# Unused

CXX = g++
CXXFLAGS = -g `pkg-config --cflags --libs opencv4` -latomic -Wno-psabi

obj/tracker.o : src/tracker.cpp src/tracker.h src/struct.h
	@echo "Compiling $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)
