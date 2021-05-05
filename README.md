# Data Compaction
***
This is a C code that uses openmp library threads for compacting data. The code expects data to be stored in a file as space separated positive integers and zeroes. Having zero at a place means there is no data at that spot. Initially the data is scattered. Running the code compacts the data, i.e. puts all the positive integers at one spot.
***
## Instructions for running this code
***
Run the `Makefile` to generated the binary `compact`

```
make
```
and then provide the filename while running the binary

```
./compact > <filename>
```
