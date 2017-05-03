system: clean main.o
	gcc main.o -lm -o thread -pthread
	rm -rf *.o
	./thread log
thread.o: main.c
	gcc -c main.c -o thread.o -lm
clean:
	rm -rf *.o
