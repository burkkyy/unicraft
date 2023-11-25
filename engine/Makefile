CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -I include/
LDFLAGS = -lglfw3 -lvulkan -ldl -lpthread

source_files := $(wildcard src/*.cpp)
object_files := $(patsubst %.cpp,build/obj/%.o,$(notdir $(source_files)))

default: $(object_files) $(source_files)

$(object_files): $(source_files)
	$(info $@)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(patsubst build/obj/%.o,src/%.cpp,$@) -o $@

.PHONY: release dev test clean
release: CFLAGS += -DNDEBUG
release: $(object_files) $(source_files)

dev: $(object_files) $(source_files)

test: test.bin
test.bin: test/test.cpp $(object_files) $(source_files)
	$(CC) $(CFLAGS) $< $(object_files) $(LDFLAGS) -o $@

clean: 
	-rm -rf build/
	-rm *.bin *.out
