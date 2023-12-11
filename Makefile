.PHONY: run clean

build/demo: build/binary.br src/wrapper.sh
	cat src/wrapper.sh build/binary.br > build/demo
	chmod +x build/demo
	stat build/demo | cut -f 8 -d' '

src/shaders.gen.c: src/vert.glsl src/frag.glsl
	tools/minify.py

build/binary.br: src/main.m src/shaders.gen.c
	mkdir -p build
	clang -framework AVFoundation -framework Cocoa -framework OpenGL src/main.c -Oz -o build/binary
	cp build/binary build/unstripped
	strip -SXTNx build/binary
	brotli -fq 11 build/binary

run: build/demo
	build/demo

clean:
	rm -rf build
	rm src/shaders.gen.c
