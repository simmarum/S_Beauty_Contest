#!/bin/bash

# First argument is N
# Second argument is L
# Third argument is S

INFO_ERROR="\nTry run program with 3 argumetns (N-number of proccesses, L-number of doctors, S-capacity of salon, name_of_hostfile (optional))\n#>bash run.sh 2 3 4\n#>bash run.sh 2 3 4 path/to/hostfile"

if [ -z "$1" ]
  then
    echo "Number of proccesses is missing (N)"
    echo "$INFO_ERROR"
    exit 1
fi

if [ -z "$2" ]
  then
    echo "Number of doctors is missing (L)"
    echo "$INFO_ERROR"
    exit 1
fi

if [ -z "$3" ]
  then
    echo "Capacity of salon is missing (S)"
    echo "$INFO_ERROR"
    exit 1
fi

if [ -n "$4" ]
  then
    make && mpirun --hostfile $4 -np $1 ./contest.out $2 $3
else
  make && mpirun -np $1 ./contest.out $2 $3
fi