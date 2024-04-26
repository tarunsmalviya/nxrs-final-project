
CXX := g++
CFLAGS := -g -pg -O3 -fopenmp -std=c++14

TARGET := mesh-simplification
SRCS := $(shell ls *.cpp)
OBJS := $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ $(CFLAGS) -o $@

all: $(TARGET)

clean:
	rm -rf *.o $(TARGET)

.PHONY: clean