#include "utility.h"
#include "process.h"
#include "thread.h"


#define MAT_A   0
#define MAT_B   1
#define MAT_C   2
#define SUM     3

void print_matrix (shm_data *data);

int main(int argc, char *argv[])
{
	shm_data shared_data[4];

	// Args check
	if (argc == 6) {
		init_all(shared_data, argv);

		printf("%u\n", shared_data[0].shmaddress[0]);
		// inizio il calcolo
		main_process(shared_data, atoi(argv[4]), atoi(argv[5]));

		// print_matrix(&shared_data[2]);
		write_matrix_to_file(&shared_data[2]);

		my_printf("sum = %i\n", *(shared_data[3].shmaddress));
		free_all(shared_data);
	}
	else if (argc == 5) 
	{
		scheduling_t(argv);
	}

	else
	{
		errno = EINVAL;
		my_perror("(MAIN.C) Too much or too few args.");
		my_printf("Usage (with process):\n");
		my_printf("\t ./calcola.x <fileToMatrixA> <fileToMatrixB> <fileToMatrixC> matDegree n_proc\n\n");
		my_printf("Usage (with thread):\n");
		my_printf("\t ./calcola.x <fileToMatrixA> <fileToMatrixB> <fileToMatrixC> matDegree\n\n");
		exit(1);
	}

	return 0;
}

void print_matrix(shm_data *data) 
{
	int i, j;

    for(i=0; i<data->rows; i++){
        for(j=0; j<data->cols; j++)
            my_printf("%2d  ", data->shmaddress[i*data->rows+j]);
        my_printf("\n");
    }
}