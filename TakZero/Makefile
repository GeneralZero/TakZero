CC=g++
CLANG=clang++
CFLAGS=-c -g -Wall -std=c++14 -Wno-unused-variable -Ofast -I /usr/include/hdf5/serial -pthread
LDFLAGS=-pthread -lpthread -L /usr/lib/x86_64-linux-gnu/hdf5/serial/ -lz -lm -lhdf5_cpp -lhdf5 -lstdc++fs
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=TakZero
SRC=$(SOURCES)

test: $(SRC)
	$(CC) $^ -o $@  $(CFLAGS) $(LIBS)

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@-gcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clang: 
	$(CLANG) $(SOURCES) $(CFLAGS) $(LIBS)
	$(CLANG) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)-clang
