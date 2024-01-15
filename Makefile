CC = g++
CFLAGS = -Wall -O3 -std=c++20

MPICC = mpicxx

TARGET = main
MPI_TARGET = mpi_main

# Define paths
SRC_DIR = src
BUILD_DIR = build

# Automatically find all .cc files in src and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.cc' -not -path src/mpi.cc -not -path "src/mpi/*")
MPI_SRCS = $(shell find $(SRC_DIR) -name '*.cc' -not -path "src/annealer/*" -not -path "src/main.cc")

# Convert .cc files to .o files in build directory
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)
MPI_OBJS = $(MPI_SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

# Automatically find all header directories in src and its subdirectories
HEADER_DIRS = $(shell find $(SRC_DIR) -type d -print)
MPI_HEADERS = $(shell find $(SRC_DIR) -type d -print)

INCLUDES = $(addprefix -I, $(HEADER_DIRS))
MPI_INCLUDES = $(shell find $(SRC_DIR) -type d -not -path "src/annealer" | sed 's/^/-I/')

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Generic rule for .cc to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(MPI_INCLUDES) -c $< -o $@

# Pattern rule for MPI objects
$(BUILD_DIR)/mpi/%.o: $(MPI_DIR)/%.cc
	@mkdir -p $(@D)
	$(MPICC) $(CFLAGS) $(MPI_INCLUDES) -c $< -o $@


# MPI target
mpi: $(MPI_TARGET)

$(MPI_TARGET): $(MPI_OBJS)
	$(MPICC) $(CFLAGS) $(MPI_INCLUDES) -o $(MPI_TARGET) $(MPI_OBJS)

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET) $(MPI_TARGET)

# $(TARGET): main.o Graph.o Annealer.o Args.o run.o
# 	$(CC) $(CFLAGS) -o main main.o Graph.o Annealer.o Args.o run.o
#
# main.o: main.cc Graph.h
# 	$(CC) $(CFLAGS) -c main.cc
#
# Graph.o: Graph.cc Spin.h
# 	$(CC) $(CFLAGS) -c Graph.cc
#
# Annealer.o: Annealer.cc Graph.h
# 	$(CC) $(CFLAGS) -c Annealer.cc
#
# mpi/Annealer.o: mpi/Annealer.cc Graph.h
# 	$(MPICC) $(CFLAGS) -c mpi/Annealer.cc
#
# Args.o: Args.cc
# 	$(CC) $(CFLAGS) -c Args.cc
#
# run.o: run.cc Helper.h
# 	$(CC) $(CFLAGS) -c run.cc
#
# mpi.o: mpi.cc
# 	$(MPICC) $(CFLAGS) -c mpi.cc
#
# clean:
# 	$(RM) $(TARGET) *.o
#
# mpi: mpi.o run.o Graph.o mpi/Annealer.o Args.o
# 	$(MPICC) $(CFLAGS) -o main mpi.o run.o Graph.o Annealer.o Args.o

