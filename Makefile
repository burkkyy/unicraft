CC=g++
CFLAGS=-std=c++17 -O2 -Wall -Wextra -I src/
LDFLAGS=-lglfw3 -lvulkan -ldl

.PHONY: clean test
clean:
	-rm -rf build/

test: test.bin
test.bin: test/test.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

