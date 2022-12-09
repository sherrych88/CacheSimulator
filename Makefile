all: cache

cache: cache.c
	gcc -Wall -Werror -std=c11 cache.c -o cache -lm

clean:
	rm -rf cache
