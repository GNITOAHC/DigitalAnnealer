CC = g++
CFLAGS = -Wall -O3 -std=c++20

MPICC = mpicxx

TARGET = main
MPI_TARGET = mpi_main

# Define paths
SRC_DIR = src
BUILD_DIR = build

# Automatically find all .cc files in src and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.cc' -not -path "src/annealer/Mpi*")
MPI_SRCS = $(shell find $(SRC_DIR) -name '*.cc')

# Convert .cc files to .o files in build directory
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)
MPI_OBJS = $(MPI_SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

# Automatically find all header directories in src and its subdirectories
HEADER_DIRS = $(shell find $(SRC_DIR) -type d -print)
INCLUDES = $(addprefix -I, $(HEADER_DIRS))
# MPI_INCLUDES = $(shell find $(SRC_DIR) -type d -not -path "src/annealer" | sed 's/^/-I/')

# #defines to the program
DEFS =

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Generic rule for .cc to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(DEFS)

$(BUILD_DIR)/annealer/MpiAnnealer.o: $(SRC_DIR)/annealer/MpiAnnealer.cc
	@mkdir -p $(@D)
	$(MPICC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(DEFS)


# MPI target
mpi: DEFS += -DUSE_MPI
mpi: $(MPI_TARGET)

$(MPI_TARGET): $(MPI_OBJS)
	$(MPICC) $(CFLAGS) $(INCLUDES) -o $(MPI_TARGET) $(MPI_OBJS)

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET) $(MPI_TARGET)
