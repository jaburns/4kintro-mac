A: main.m wrapper.sh
	clang -framework AVFoundation -framework Cocoa -framework OpenGL main.m -Oz -o A
	brotli -fq 11 A
	cat wrapper.sh A.br > demo
	chmod +x demo
	rm A.br
