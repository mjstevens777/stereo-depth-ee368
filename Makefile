all: cmake/Makefile
	make -C cmake

cmake/Makefile:
	mkdir -p cmake
	cd cmake && cmake ..
