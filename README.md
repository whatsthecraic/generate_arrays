---
generate_arrays
---

A program to create multiple arrays of random 8 byte unsigned integers. 
The arrays are loaded and read by the mmap_throughput suite of experiments,
to investigate the difference in throughput between memory mapped files
and an ad-hoc buffer manager.

Sample usage, to create 4 arrays of 64M ints ~ 512MB:

         ./generate_arrays -N 4 -M 64M /tmp/where_to_store_files