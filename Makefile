all: main.c
	gcc -g -Wall -o mycap *.c -lpcap  -std=gnu99 
  
clean:
	rm -rf *.o mycap