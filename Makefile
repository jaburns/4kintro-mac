A: main.m wrapper.sh Makefile
	clang -framework AVFoundation -framework Cocoa -framework OpenGL main.c -Oz -o A
	cp A binary
	strip -SXTNx A
	brotli -fq 11 A
	cat wrapper.sh A.br > demo
	chmod +x demo
	rm A A.br
