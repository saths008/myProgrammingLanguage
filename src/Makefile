CXX = clang++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS =
DEBUG_FLAGS = --debug
SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
TARGET = main
VALGRIND_CMD = valgrind --leak-check=yes --log-file=valgrind.rpt
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean

memLeak: $(TARGET)
	@read -p "Enter file: " input_args; \
	$(VALGRIND_CMD) ./$(TARGET) $$input_args
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)
