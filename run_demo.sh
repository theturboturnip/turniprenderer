cd ./build/demo/
if [ $1 == "debug" ]; then
	gdb ./demo
else
	./demo
fi
