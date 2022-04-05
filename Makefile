CXX = g++
LD = ld
CFLAGS = -Wall
TARGET = creamware
creamware: egg.cpp
	$(CXX) $(CFLAGS) egg.cpp -o $@

