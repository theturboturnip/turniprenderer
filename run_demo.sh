cd ./build/demo/
if [ $# -ge 1 ]; then
	 if [ $1 == "debug" ]; then
		 gdb ./demo
	 elif [ $1 == "valgrind" ]; then
		 valgrind ./demo
	 elif [ $1 == "callgrind" ]; then
		 valgrind --tool=callgrind ./demo
	 else
		 echo "Invalid argument: must be 'debug', 'valgrind', or 'callgrind'"
		 exit 1
	 fi
else
	./demo
fi
