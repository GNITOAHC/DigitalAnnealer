CC = g++
CFLAGS = -Wall -g -std=c++20

TARGET = main

all: $(TARGET)

$(TARGET): main.o Graph.o Annealer.o Args.o
	$(CC) $(CFLAGS) -o main main.o Graph.o Annealer.o Args.o

main.o: main.cc Graph.h
	$(CC) $(CFLAGS) -c main.cc

Graph.o: Graph.cc Spin.h
	$(CC) $(CFLAGS) -c Graph.cc

Annealer.o: Annealer.cc Graph.h
	$(CC) $(CFLAGS) -c Annealer.cc

Args.o: Args.cc
	$(CC) $(CFLAGS) -c Args.cc

clean:
	$(RM) $(TARGET) *.o
