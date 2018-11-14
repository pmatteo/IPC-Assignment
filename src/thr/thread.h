#include "utility.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef pthread_h
	#define pthread_h
	
	typedef int **matrix_t;

	typedef struct 
	{
		int		id;		// id del processo
		int		size;	// dimensione delle matrici
		int		row;	// riga della matC da calcolare
	} data_t;

		
	void scheduling_t(char **argv);

	void load_matrix_from_file_t(matrix_t mat, char *path, int size);

	void write_matrix_to_file_t (matrix_t mat, char *path, int size);

	matrix_t alloc_matrix(int size);

	void free_matrix(matrix_t mat, int size);

	void * do_multiplication_t(void *args);

	void * do_sum_t(void *args);

	#endif	