CXX      = g++
CXXFLAGS = -std=c++11 -Wall
SRC_DIR  = provided_code
SRCS     = $(SRC_DIR)/parser.cc \
           $(SRC_DIR)/lexer.cc \
           $(SRC_DIR)/inputbuf.cc
TARGET   = a.out

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)
	rm -rf output/

test: $(TARGET)
	bash $(SRC_DIR)/test1.sh
