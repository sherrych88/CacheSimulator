all: second

second: second.c
	gcc -Wall -Werror -fsanitize=address -std=c11 second.c -o second -lm

clean:
	rm -rf second