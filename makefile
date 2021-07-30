CXX = g++
CXXFLAGS = -g `pkg-config --cflags --libs opencv4` -latomic -Wno-psabi

tracker.o : tracker.cpp tracker.h struct.h
	@echo "Compiling $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)
