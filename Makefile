# sudo apt-get install libclang-dev
all:
	rm -f bindgen
# NYI do not depend on clang 11
	clang bindgen.c -o bindgen -I/usr/lib/llvm-11/include/ -L/usr/lib/llvm-11/lib/ -lclang
	./bindgen test.h | sed 's/, )/)/' > test.fz
