CC=g++
CLANG=clang++
CFLAGS=-c -g -Wall -std=c++14 -Wno-unused-variable -Ofast
LDFLAGS=-lz -lm -lhdf5 -lhdf5_cpp -lstdc++fs -lpthread
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=TakZero
SRC=$(SOURCES)

test: $(SRC)
	$(CC) $^ -o $@  $(CFLAGS) $(LIBS)

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@-gcc

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clang: 
	$(CLANG) $(SOURCES) $(CFLAGS) $(LIBS)
	$(CLANG) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)-clang