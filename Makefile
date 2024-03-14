.DEFAULT_GOAL := all # Set default target to all

CC = g++
CFLAGS = -Wall -O3 -std=c++20

MPICC = mpicxx

TARGET = main
MPI_TARGET = mpi_main
LIB_TARGET = libmylib.a

# Define paths
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

# Library sources and objects
LIB_SRCS = $(shell find $(LIB_DIR) -name '*.cc')
LIB_OBJS = $(LIB_SRCS:$(LIB_DIR)/%.cc=$(BUILD_DIR)/lib/%.o)

# Program sources and objects
SRCS = $(shell find $(SRC_DIR) -name '*.cc' -not -path "src/annealer/Mpi*") # Automatically find all .cc files in src and its subdirectories
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o) # Convert .cc files to .o files in build directory

# MPI sources and objects
MPI_SRCS = $(shell find $(SRC_DIR) -name '*.cc')
MPI_OBJS = $(MPI_SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

# Automatically find all header directories in src and its subdirectories
HEADER_DIRS = $(shell find $(SRC_DIR) -type d -print)
INCLUDES = $(addprefix -I, $(HEADER_DIRS))

# #defines to the program
DEFS =

# Default target
all: $(LIB_TARGET) $(TARGET)

lib: $(LIB_TARGET)

main: $(TARGET)

mpi: DEFS += -DUSE_MPI
mpi: $(LIB_TARGET) $(MPI_TARGET)

# ===== Library target rules
# Rule to make the library
$(LIB_TARGET): $(LIB_OBJS)
	ar rcs $(BUILD_DIR)/$(LIB_TARGET) $(LIB_OBJS)

# Generic rule for compiling library source files into objects
$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(DEFS)

# ===== Main target rule
$(TARGET): $(OBJS)
	# $(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) -L$(BUILD_DIR) -lmylib

# Generic rule for .cc to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(DEFS)

# ===== MPI target rules
$(BUILD_DIR)/annealer/MpiAnnealer.o: $(SRC_DIR)/annealer/MpiAnnealer.cc
	@mkdir -p $(@D)
	$(MPICC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(DEFS)

$(MPI_TARGET): $(MPI_OBJS)
	$(MPICC) $(CFLAGS) $(INCLUDES) -o $(MPI_TARGET) $(MPI_OBJS)

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET) $(MPI_TARGET)

.PHONY: all lib main mpi clean
