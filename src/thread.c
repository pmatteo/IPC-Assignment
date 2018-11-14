#include "thread.h"

pthread_mutex_t mutexsum;

matrix_t matA, matB, matC;		// Matrici per i calcoli
int sum;						// Somma el di C
pthread_mutex_t mutexsum;		// Semaforo
pthread_t *threads;				// Lista dei thread

void scheduling_t(char **argv)
{
	// Mi prendo dalla lista dei parametri la dimensione della matrice.
	// e la uso per calcolare il numero di thr da usare (uno per riga per le
	// moltiplicazione, poi li riuso nelle somme)
	int size = atoi(argv[4]);
	int thr_numb = size;
	// Struttura per l'inizializzazione dei thr con attributi.
	pthread_attr_t attr;

	// Alloco spazio per gli argomenti da passare ai thread e per i thread stessi
	data_t *args = calloc(thr_numb, sizeof(data_t)); 
	threads = calloc(thr_numb, sizeof(pthread_t));

	// Alloco spazio per le matrici, le inizializzo e (A e B) le riempio con  
	// quello che ho letto da file
	matA = alloc_matrix(size);						
	load_matrix_from_file_t(matA, argv[1], size);
	matB = alloc_matrix(size);
	load_matrix_from_file_t(matB, argv[2], size);
	matC = alloc_matrix(size);

	// Inizializzo i mutex e gli attributi per le matrici
	pthread_mutex_init(&mutexsum, NULL);
	pthread_attr_init(&attr);
  	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int i;
	// Scorro i thread
	for (i = 0; i < size; ++i)
	{
		// Setto gli args da passare come parametro
		args[i].id = i;
		args[i].size = size;
		args[i].row = i;
		// Creo il thread per la moltiplicazione della riga iesima della matrice C
		if (pthread_create(&threads[i], &attr, do_multiplication_t, (void *) &args[i]) == -1)
			my_perror("(THREAD.C) : Failed to create thread");
	}

	// Aspetto i thread
	for (i = 0; i < size; ++i)
	{
		if (pthread_join(threads[i], NULL) == -1)
			my_perror("(THREAD.C) : Failed to join thread");
	}

	// Setto gli args e creo il thread per la somma della riga iesima di C
	for (i = 0; i < size; ++i)
	{
		args[i].id = i;
		args[i].size = size;
		args[i].row = i;
		if (pthread_create(&threads[i], NULL, do_sum_t, (void *) &args[i]) == -1)
			my_perror("(THREAD.C) : Failed to create thread");
	}
	// Aspetto i thread
	for (i = 0; i < size; ++i)
	{
		if (pthread_join(threads[i], NULL) == -1)
			my_perror("(THREAD.C) : Failed to join thread");
	}

	// Scrivo il risultato sul file
	write_matrix_to_file_t(matC, argv[3], size);

	my_printf("\nsum = %d\n", sum);

	// Libero tutta la memoria allocata e il semaforo
	free_matrix(matA, size);
	free_matrix(matB, size);
	free_matrix(matC, size);
	free (threads);
	free (args);
	pthread_mutex_destroy(&mutexsum);
}

void * do_multiplication_t(void *args)
{
	int col, i;
	data_t *data = (data_t *) args;
	int val = 0, size = data->size;

	// Scorro le righe della matrice
	for (col = 0; col < size; ++col)
	{
		// Scorro la riga di A e la colonna di B
		for (i = 0; i < size; ++i)
		{
			val += (matA[data->row][i] * matB[i][col]);
		}
		my_printf("Thread (%d): do mul -> row=%d, col=%d, val=%d\n", data->id, data->row, col, val);
		matC[data->row][col] = val;
		val = 0;
	}

	pthread_exit((void*) 0);
}

void * do_sum_t(void *args)
{
	int col;
	data_t *data = (data_t *) args;
	int my_sum = 0, size = data->size;
	//int id=data->id;

	//pthread_join(threads[id-size], NULL);
	// Scorro la riga di C e aggiorno la somma della variabile locale
	for (col = 0; col < size; ++col)
	{
		my_sum += matC[data->row][col];
	}

	my_printf("Thread (%d): do sum -> row=%d\n", data->id, data->row, sum, my_sum);
	
	// Cerco di accedere alla zona critica
	pthread_mutex_lock (&mutexsum);
	// Aggiorno la somma condivisa
	sum += my_sum;
	// Esco dalla zona critica
	pthread_mutex_unlock (&mutexsum);

	pthread_exit((void*) 0);
}

void load_matrix_from_file_t(matrix_t mat, char *path, int size)
{
	int row = 0, col = 0;
	int fd, r;
	char *buff = (char *) calloc(4096, sizeof(char));               // buff per lettura
	//char buff[4096];
    char *line, *val, *brkt, *brkb;

	if ((fd = open(path, O_RDONLY)) == -1) 
	{
        my_perror("(THREAD.C) : open matrix.");
        exit(1);
    }
    // leggo la riga
    while ((r = read(fd, buff, 4096)) > 0) 
    {
        // uso stringtokenizer per dividere
     	for (line = strtok_r(buff, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) 
     	{
            for (val = strtok_r(line, ",", &brkb); val; val = strtok_r(NULL, ",", &brkb)) 
            {
                // Assegno il valore che atoi converte alla mia matrice
                mat[row][col] = atoi (val);
                if (++col == size)
            		break;
            }
            // controllo di essere arrivato alla fine, necessario perche a volte da dei problemi con il newline a volte li da senze. bah
            if (++row == size)
            	break;
            col = 0;
        }
    }
    if (r == -1) 
    {
        my_perror ("THREAD.C) : read error.");
        exit (1);
    }
    free(buff);
    close (fd);
}

void write_matrix_to_file_t (matrix_t mat, char *path, int size) {
    int i,j, fd;
    char buf [4096];               // buff per scrittura
    char val [11];

    if ((fd = open(path, O_WRONLY | O_TRUNC)) == -1) {
        my_perror("(THREAD.C) : open matrix C");
        exit(1);
    }

    // Scorro le rige
    for(i=0; i<size; i++)
    {
    	// Inizializzo il buffer con la stringa vuota
        strcpy(buf, "");
        // Scorro le colonne
        for(j=0; j<size; j++)
        {
        	// Metto in una var tmp il valore letto
        	if (j == size-1)
        		sprintf(val, "%d", mat[i][j]);
            else
            	sprintf(val, "%d,", mat[i][j]);
            // scrivo il valore di val nel buffer per scrivere
            strcat(buf, val);
        }
        // Vado a capo
        strcat(buf, "\n");
        // Scivo sul file la riga
        if (write(fd, buf, strlen(buf)) == -1)
            my_perror("(THREAD.C) : write matrix file");
    }

    close(fd);
}

matrix_t alloc_matrix(int size)
{
	int i;
	matrix_t mat = calloc(size, sizeof(int *));
	
	for (i = 0; i < size; ++i)
	{
		mat[i] = calloc(size, sizeof(int));
	}

	return mat;
}

void free_matrix(matrix_t mat, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		free(mat[i]);
	}
	free(mat);
}