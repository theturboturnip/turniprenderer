# This would be called "build" but when it is Makefile won't run it because it "is up to date"
_build:
	@# Update CMakeFiles
	@echo "Generating CMakeSources.txt for libturniprenderer" && \
		bash ./libturniprenderer/generate_sources.sh
	@echo "Generating CMakeSources.txt for demo" && \
		bash ./demo/generate_sources.sh

	@# Go to the build folder, run CMake and finally use the resulting makefile
	@mkdir -p ./build/ && cd ./build/ && cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .. && make --no-print-directory

clean:
	rm -rf ./build/libturniprenderer ./build/demo

rebuild: clean _build

all: _build
