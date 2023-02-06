include ../.env

run: main.cpp
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

shaders/first_shader.frag.spv: shaders/first_shader.frag
	glslc $^ -o $@

shaders/first_shader.vert.spv: shaders/first_shader.vert
	glslc $^ -o $@

.PHONY: all debug clean
all: run debug #shaders/first_shader.frag.spv shaders/first_shader.vert.spv

debug: main.cpp
	$(CC) $(CFLAGS) -DNDEBUG $^ $(LDFLAGS) -o $@

clean:
	@rm run debug 2>/dev/null
