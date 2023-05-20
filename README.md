# Parallel processing using the Map-Reduce paradigm

In the main function, the list of input files is read from the main file and added to a queue. Additionally, any synchronization structures to be used are initialized.

The arguments for mappers and reducers are constructed and threads are started.

Perfect powers are stored in an n x m matrix of unordered sets, where n is the number of mappers and m is the number of reducers. The first dimension of the matrix is used to assign each mapper an array of sets, so each mapper has a row of the matrix. In this array, the index represents the power for which perfect powers are being calculated. The set contains the numbers that are perfect powers.

## Mapper arguments

- list - the list of input files
- powerLists - the array of sets of perfect powers for this mapper
- maxPower - the maximum power up to which to calculate perfect powers
- barrier - a barrier
- mutex - a mutex

## Reducer arguments
- id - the power for which this reducer writes
- powerLists - the matrix of sets of perfect powers
- mappers - the number of mappers
- reducers - the number of reducers
- barrier - a barrier

## Mapper

The mapper looks in the file queue and extracts one if there is one, otherwise it exits the while loop. The extraction from the queue is encapsulated in a mutex.

The file is read. If the number is 1, it is added as a perfect power for all powers. If it is different, all powers for which the number is a perfect power are calculated. The number is added to the corresponding set.

For each file, a new set is created, which is added to the set for this mapper at the end. We have observed that this improves performance.

The mapper waits at the final barrier for all other mappers.

### Calculating perfect powers

A for loop goes through all powers from 2 to maxPower.

For each power, a binary search is performed to find a number A such that A<sup>exp</sup> is equal to the number.

### Reducer

The reducers start with a barrier that coincides with that of the mappers. This way they will only start after all the mappers have finished.

Each reducer has an ID with the power it processes. It extracts the numbers that are perfect powers for this ID. The numbers are in the sets on the columns of the set matrix.

These sets are added to a set and the size of the set is written to the output file.
