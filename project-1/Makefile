CXX      = g++
CXXFLAGS = -std=c++11 -Wall
SRC_DIR  = provided_code
SRCS     = $(SRC_DIR)/parser.cc \
           $(SRC_DIR)/lexer.cc \
           $(SRC_DIR)/inputbuf.cc
TARGET       = a.out
FORMAT_SRCS  = $(SRC_DIR)/parser.cc $(SRC_DIR)/parser.h
CLANG_FORMAT ?= clang-format

.PHONY: all clean test format

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)
	rm -rf output/

test: $(TARGET)
	bash $(SRC_DIR)/test1.sh

format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || \
		{ echo "Error: clang-format not found. Install with: brew install clang-format"; exit 1; }
	$(CLANG_FORMAT) -i $(FORMAT_SRCS)
