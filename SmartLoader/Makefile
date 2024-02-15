#invoke make inside following directories and in this order: loader, launch, fib
compile:
	
	make -C ./loader create 
	mv ./loader/lib_simpleloader.so ./bin
	
	make -C ./launcher create 
	mv ./launcher/launch ./bin
	make -C ./test all
	./bin/launch ./test/fib
	
clean:
	make -C ./loader clean 
	
	make -C ./test clean
	rm ./bin/launch
	rm ./bin/lib_simpleloader.so 
#move the lib_simpleloader.so and launch binaries inside bin directory

#Provide the command for cleanup
