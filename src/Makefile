CXX = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS =

SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
TARGET = main

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean

