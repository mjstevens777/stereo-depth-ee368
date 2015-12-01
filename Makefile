all: cmake/Makefile
	make -C cmake

test:
	python run-test-matrix.py > tests.cmd
	cat tests.cmd|tr "\n" "\0"|xargs -P $$(( $(shell nproc) - 2 )) -0 -n 1 sh -c
 
cmake/Makefile:
	mkdir -p cmake
	cd cmake && cmake ..
