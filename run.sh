#!/usr/bin
# First argument is N
# Second argument is L
# Third argument is S

INFO_ERROR="\nTry run program with 3 argumetns (N-number of proccesses, L-number of doctors, S-capacity of salon)\n#>bash run.sh 2 3 4"

if [ -z "$1" ]
  then
    echo "Number of proccesses is missing (N)"
    echo -e $INFO_ERROR
    exit 1
fi

if [ -z "$2" ]
  then
    echo "Number of doctors is missing (L)"
    echo -e $INFO_ERROR
    exit 1
fi

if [ -z "$3" ]
  then
    echo "Capacity of salon is missing (S)"
    echo -e $INFO_ERROR
    exit 1
fi

echo "Running makefile"
make
echo "Done makefile!"

mpirun -np $1 ./contest.out $2 $3
