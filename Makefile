all: cache

second: cache.c
	gcc -Wall -Werror -fsanitize=address -std=c11 cache.c -o cache -lm

clean:
	rm -rf cache
