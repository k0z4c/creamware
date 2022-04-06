CXX = g++
LD = ld
CFLAGS = -Wall -I./lib/cryptopp860
TARGET = creamware
creamware: egg.cpp
	$(CXX) $(CFLAGS) egg.cpp -o $@

