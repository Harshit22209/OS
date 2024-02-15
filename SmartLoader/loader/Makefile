#Create lib_simpleloader.so from loader.c
create:
	gcc -c -m32 -fPIC loader.c -o loader.o
	gcc -m32 -shared -o lib_simpleloader.so loader.o
#Provide the command for cleanup
clean:
	rm loader.o