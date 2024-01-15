CC = g++
CFLAGS = -Wall -O3 -std=c++20

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

mpi/Annealer.o: mpi/Annealer.cc Graph.h
	$(MPICC) $(CFLAGS) -c mpi/Annealer.cc

Args.o: Args.cc
	$(CC) $(CFLAGS) -c Args.cc

run.o: run.cc Helper.h
	$(CC) $(CFLAGS) -c run.cc

mpi.o: mpi.cc
	$(MPICC) $(CFLAGS) -c mpi.cc

clean:
	$(RM) $(TARGET) *.o

mpi: mpi.o run.o Graph.o mpi/Annealer.o Args.o
	$(MPICC) $(CFLAGS) -o main mpi.o run.o Graph.o Annealer.o Args.o
