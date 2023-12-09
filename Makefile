all: build run

build: release

debug:
	gcc -g stopwatch.c -o stopwatch -Wall -lncurses

release:
	gcc -O2 stopwatch.c -o stopwatch -Wall -lncurses

run:
	./stopwatch
