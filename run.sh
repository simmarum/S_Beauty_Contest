#!/bin/bash

# First argument is N
# Second argument is L
# Third argument is S
# Fourth argument is optional path to hostfile (for mpi)

INFO_ERROR="\nTry run program with 3 argumetns (N-number of proccesses, L-number of doctors, S-capacity of salon, name_of_hostfile (optional))\n#>bash run.sh 2 3 4\n#>bash run.sh 2 3 4 path/to/hostfile"

if [ -z "$1" ]
  then
    echo "Number of proccesses is missing (N)"
    echo -e "$INFO_ERROR"
    exit 1
fi

if [ "$1" -eq "0" ]
  then
    echo "Number of proccesses is ZERO!"
    echo -e "$INFO_ERROR"
    exit 1
fi

if [ -z "$2" ]
  then
    echo "Number of doctors is missing (L)"
    echo -e "$INFO_ERROR"
    exit 1
fi

if [ -z "$3" ]
  then
    echo "Capacity of salon is missing (S)"
    echo -e "$INFO_ERROR"
    exit 1
fi

if [ -n "$4" ]
  then
    mpirun --hostfile $4 -np $1 ./contest $2 $3
else
  mpirun -np $1 ./contest $2 $3
fi
