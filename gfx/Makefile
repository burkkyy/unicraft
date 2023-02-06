include .env

src_files = $(wildcard impl/*.cpp)
obj_files := $(patsubst impl/%.cpp,obj/%.o,$(src_files))

lib/libgfxEngine.a: $(obj_files) $(src_files)
	ar rcs $@ $^

$(obj_files): $(src_files)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(patsubst obj/%.o,impl/%.cpp,$@) -o $@

.PHONY: test
test: test/test
test/test: lib/libgfxEngine.a test/test.cpp
		$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lgfxEngine
