# This would be called "build" but when it is Makefile won't run it because it "is up to date"
_build:
	@# Update CMakeFiles
	@echo "Generating CMakeSources.txt for libturniprenderer" && \
		bash ./libturniprenderer/generate_sources.sh
	@echo "Generating CMakeSources.txt for demo" && \
		bash ./demo/generate_sources.sh

	@# Go to the build folder, run CMake and finally use the resulting makefile
	@mkdir -p ./build/ && cd ./build/ && cmake .. && make --no-print-directory

clean:
	rm -rf ./build/

rebuild: clean _build

all: _build
