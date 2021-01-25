# This project calculates all the non-zero values in a matrix using MPI. This works with any number of processes.

# Members: Darryn Wong

To compile:
mpic++ main.cpp
To run with default input file(input.txt) and 1 process:
mpirun -n 1 ./a.out
To run with default input file and 5 process:
mpirun -n 5 ./a.out
To run with any input file (using “input1.txt in this case”) and 2 process:
mpirun -n 5 ./a.out input1.txt
