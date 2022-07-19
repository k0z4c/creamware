CXX = g++
CFLAGS = -Wall -Wextra -O2 -I./lib/cryptopp860 -L./lib/cryptopp860 -lcryptopp
TARGET = creamware

creamware: egg.cpp
	$(CXX) egg.cpp $(CFLAGS) -o $@ 

%.o: %.c

clean:
	rm ./creamware 
