CC=gcc

example: lush_array.o example.c
	${CC} -o example -DDEBUG example.c lush_array.o

lush_array.o: lush_array.h lush_array.c
	gcc -c -ggdb -Wall -DDEBUG lush_array.c

clean: 
	rm -f *~ 
	rm -f *.o
	rm -f example
