# Cache
A cache representation using separate chaining hash tables to simulate the movement of data in a two level cache system.
# How To Run
- clone git https://github.com/sherrych88/CacheSimulator.git
- cd CacheSimulator
- using makefile:
  - mingw32-make
  - testcases:
  - ./cache 512 assoc:1 fifo 16 1024 assoc:1 fifo trace1.txt 
          (output file: one_1.txt)
  - ./cache 512 assoc:1 lru 16 512 assoc:2 lru trace1.txt
          (output file: one_2.txt)
  - ./cache 256 assoc:64 lru 4 512 assoc:4 fifo trace1.txt
          (output file: one_6.txt)
- not using makefile:
  - gcc -Wall -Werror cache.c -o cache
  - testcases:
  - ./cache 512 assoc:1 fifo 16 1024 assoc:1 fifo trace1.txt 
          (output file: one_1.txt)
  - ./cache 512 assoc:1 lru 16 512 assoc:2 lru trace1.txt
          (output file: one_2.txt)
  - ./cache 256 assoc:64 lru 4 512 assoc:4 fifo trace1.txt
          (output file: one_6.txt)
 # How It's Made
 - C
 - I created two tables to represent two levels of exclusive caches.
 - I implemented FIFO (first in first out) and LRU (least recently used) algorithms for evictions and movement of the blocks.
 - Sets are represented by the indices of the hash table and blocks are represented by the elements in the chains of each set.
 - Evictions from L1 are placed into L2
 - The program outputs the number of memory reads, memory writes, cache hits, and cache misses.
 # Notes
 - Creating this program gave me a greater understanding of how cache memory works and how to move data.
