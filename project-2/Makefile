CXX      = g++
CXXFLAGS = -std=c++11 -Wall
DEBUG_FLAGS = -g -O0
SRC_DIR  = src
SRCS     = $(SRC_DIR)/project2.cc \
           $(SRC_DIR)/lexer.cc \
           $(SRC_DIR)/inputbuf.cc
TARGET   = a.out
FORMAT_SRCS = $(SRC_DIR)/project2.cc
CLANG_FORMAT ?= clang-format
TASK ?= 1

.PHONY: all clean test test-all debug format

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
	rm -rf output/

test: $(TARGET)
	bash scripts/test_p2.sh $(TASK)

test-all: $(TARGET)
	@for t in 1 2 3 4 5 6; do \
		echo "===== Task $$t ====="; \
		bash scripts/test_p2.sh $$t || exit 1; \
	done

format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || \
		{ echo "Error: clang-format not found. Install with: brew install clang-format"; exit 1; }
	$(CLANG_FORMAT) -i $(FORMAT_SRCS)
