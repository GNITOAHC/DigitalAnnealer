CC = g++
CFLAGS = -Wall -g -std=c++20

MPICC = mpicxx

TARGET = main

all: $(TARGET)

$(TARGET): main.o Graph.o Annealer.o Args.o run.o
	$(CC) $(CFLAGS) -o main main.o Graph.o Annealer.o Args.o run.o

main.o: main.cc Graph.h
	$(CC) $(CFLAGS) -c main.cc

Graph.o: Graph.cc Spin.h
	$(CC) $(CFLAGS) -c Graph.cc

Annealer.o: Annealer.cc Graph.h
	$(CC) $(CFLAGS) -c Annealer.cc

Args.o: Args.cc
	$(CC) $(CFLAGS) -c Args.cc

run.o: run.cc
	$(CC) $(CFLAGS) -c run.cc

mpi.o: mpi.cc
	$(MPICC) $(CFLAGS) -c mpi.cc

clean:
	$(RM) $(TARGET) *.o

mpi: mpi.o run.o Graph.o Annealer.o Args.o
	$(MPICC) $(CFLAGS) -o main mpi.o run.o Graph.o Annealer.o Args.o
