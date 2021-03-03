all: full

full : bin/main.o bin/galib.o
	gcc bin/main.o bin/galib.o -O3 -o main

bin/main.o : main.c
	gcc -c main.c -O3 -o bin/main.o -I lib/

bin/galib.o : src/geneticalgorithm.c 
	gcc -c src/geneticalgorithm.c -O3 -o bin/galib.o -I lib/

clear : 
	rm bin/*.o
	rm main
