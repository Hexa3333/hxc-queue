CC=gcc
SRC=hxc_queue.c

all:
	$(CC) -c $(SRC) -o hxc_queue.o -g
	ar rcs libhxc_queue.a hxc_queue.o
	$(CC) demo/demo.c -I. -c -o demo/demo.o -g
	$(CC) demo/demo.o -L. -lhxc_queue -o demo/demo

clean:
	rm -rf hxc_queue.o libhxc_queue.a demo/demo demo/demo.o