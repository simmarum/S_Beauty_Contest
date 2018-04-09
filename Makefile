all: contest

contest: contest.c
	mpicc contest.c -o contest.out
