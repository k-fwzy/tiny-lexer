CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = bin/lexer
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug:
	$(CXX) -std=c++17 -Wall -Wextra -g -O0 -o $(TARGET) $(SRC)

.PHONY: all clean run debug help

help:
	@echo "available targets:"
	@echo "- all - build the lexer (default)"
	@echo "- run - build and run"
	@echo "- debug - build with debug symbols"
	@echo "- clean - remove build files"
	@echo "- help - show this help message"
