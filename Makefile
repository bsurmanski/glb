headers=glb.h shader.h texture.h buffer.h program.h sampler.h
files=glb.c shader.c texture.c buffer.c program.c sampler.c

all:
	gcc $(files) -std=c99 -pedantic -o libglb.so -g -lGL -lm -fPIC --shared -Wall -DDEBUG -D GL_GLEXT_PROTOTYPES -Wno-int-to-pointer-cast

install:
	cp libglb.so /usr/local/lib/
	mkdir -p /usr/local/include/glb/
	cp $(headers) /usr/local/include/glb/

#-Wno-implicit-function-declaration -Wno-unused-function
