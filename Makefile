all: compact

compact: main.c
	gcc -w -g -fopenmp -o compact main.c -lm

clean:
	rm -rf compact