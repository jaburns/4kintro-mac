.PHONY: run clean dist

build/demo: build/binary.pack src/wrapper.sh
	cat src/wrapper.sh build/binary.pack > build/demo
	chmod +x build/demo
	stat build/demo | cut -f 8 -d' '

src/shaders.gen.c: src/vert.glsl src/frag.glsl tools/minify.py
	tools/minify.py

build/binary.pack: src/main.m src/shaders.gen.c src/audio.c
	mkdir -p build
	clang -framework AVFoundation -framework Cocoa -framework OpenGL src/main.m -Oz -o build/binary
	cp build/binary build/unstripped
	strip -SXTNx build/binary
	gzip -9fS .pack build/binary

run: build/demo
	build/demo

clean:
	rm -rf build
	rm src/shaders.gen.c

dist: build/demo
	cd build; zip demo.zip demo
