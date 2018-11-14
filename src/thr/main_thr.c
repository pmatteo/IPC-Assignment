#include "utility.h"
#include "thread.h"

int main(int argc, char *argv[])
{
	if (argc == 5) 
	{
		scheduling_t(argv);
	}

	else
	{
		errno = EINVAL;
		my_perror("(MAIN_THR.C) Too much or too few args.");
		my_printf("Usage \n");
		my_printf("\t ./calcola.x <fileToMatrixA> <fileToMatrixB> <fileToMatrixC> matDegree\n\n");
		exit(1);
	}

	return 0;
}