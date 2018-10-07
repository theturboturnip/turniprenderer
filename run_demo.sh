cd ./build/demo/
if [ $# -ge 1 ]; then
	 if [ $1 == "debug" ]; then
		 gdb ./demo
	 elif [ $1 == "valgrind" ]; then
		 valgrind ./demo
	 else
		 echo "Invalid argument: must be 'debug' or 'valgrind'"
		 exit 1
	 fi
else
	./demo
fi
