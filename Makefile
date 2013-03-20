headers=src/glb.h src/shader.h src/texture.h src/buffer.h src/program.h src/sampler.h  src/glb_types.h src/framebuffer.h
files=src/glb.c src/shader.c src/texture.c src/buffer.c src/program.c src/sampler.c src/framebuffer.c src/tga.c

all:
	gcc $(files) -std=c99 -pedantic -o libglb.so -g -lGL -lm -fPIC --shared -Wall -DDEBUG -D GL_GLEXT_PROTOTYPES -Wno-int-to-pointer-cast

install:
	cp libglb.so /usr/local/lib/
	mkdir -p /usr/local/include/glb/
	cp $(headers) /usr/local/include/glb/

#-Wno-implicit-function-declaration -Wno-unused-function
