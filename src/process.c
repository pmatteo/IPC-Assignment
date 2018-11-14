#include "process.h"


#define MAT_A   0
#define MAT_B   1
#define MAT_C   2
#define SUM_ROW 3

#define SEM_S   1
#define N_SEM   2

int n_child, msgid, idSem;
shm_data *data_l;

f_com *command;
ret_mess *ret;

void main_process(shm_data shared_data[4], int degree, int child)
{
	n_child = child;
	int pid, i;
	unsigned short vals[2];
	int pipes[n_child][2];
	data_l = shared_data;
	vals[0] = 1;
	vals[1] = 1;

	// creo e setto i semafori
	idSem = create_semaphores_set(N_SEM, vals);

	// Creo la coda di messaggi
	msgid = mess_q_create ("src/main.c", 1);

	// Creo le pipe
	pipe_create (n_child, pipes);


	for(i = 0; i < n_child; i++)
	{
		pid = fork();

		if(pid < 0)										// ERRORE
		{
			my_perror("fork");
			exit(0);
		}

		else if(pid == 0)								// FIGLIO
		{
			// Catturo i segnali per la terminazione.
			signal(SIGQUIT, hup_handler_c);
			signal(SIGTERM, hup_handler_c);
			signal(SIGINT, hup_handler_c);

			// Faccio lavorare i figli
			child_work(i, pipes[i]);

			exit(0);
		}
	}

														// PADRE

	// Catturo i segnali per la terminazione.
	signal(SIGQUIT, hup_handler_f);
	signal(SIGTERM, hup_handler_f);
	signal(SIGINT, hup_handler_f);

	// Faccio lo scheduling dei figli
	scheduling_adv(degree, pipes);

	// aspetto tutti i figli
	int status;
	//signal(SIGALRM, funzione);
	//alarm(n_sec)
	while(wait(&status) > 0);

	// chiudo la pipe
	pipe_close(n_child, pipes);
	// rimuovo i semafori
	delete_semaphore_set(idSem);
	// rimuovo la coda di messaggi
	mess_q_remove(msgid);
}

void scheduling (int degree, int pipes[n_child][2])
{
	uint16_t cur_row, cur_col;
	uint32_t sum_row;

	int i;
	command = (f_com *) malloc (sizeof(f_com));
	ret = (ret_mess *) malloc(sizeof (ret_mess));

	// Mando prima tutte le moltiplicazioni
	for (cur_row = 0; cur_row < degree; cur_row++)
		for (cur_col = 0; cur_col < degree; cur_col++)
		{
			// Leggo dalla coda, il primo della lista che prendo sarà il
			// prossimo che lavora (evito che i processi siano in busy waiting)
			mess_q_receive_message (ret, msgid);
			i = (int) ret->mtype - 1;

			command->com = MULTIPLY;
			command->com_value.mul_val.row = cur_row;
			command->com_value.mul_val.col = cur_col;
			pipe_send_message (pipes[i], command);
		}

	// Mando tutte le somme
	command->com = SUM;
	for (sum_row = 0; sum_row < degree; sum_row++)
	{
		// Leggo dalla coda, il primo della lista che prendo sarà il
		// prossimo che lavora (evito che i processi siano in busy waiting)
		mess_q_receive_message (ret, msgid);
		i = (int) ret->mtype - 1;

		command->com_value.sum_row = sum_row;
		pipe_send_message (pipes[i], command);
	}

	// Avviso tutti che il lavoro è finito in modo che possano terminare
	command->com = KILL;
	for (i = 0; i < n_child; ++i)
		pipe_send_message (pipes[i], command);

	// libero la memoria
	free (command);
	free (ret);
}

void scheduling_adv (int degree, int pipes[n_child][2])
{
	uint16_t cur_row, cur_col;
	uint32_t sum_row;

	int i;
	int n_op = degree*degree;
	command = (f_com *) malloc(sizeof(f_com));
	ret = (ret_mess *) malloc(sizeof (ret_mess));

	lock_semaphore(idSem, SEM_S, N_SEM);

	// Mando moltiplicazioni, mando prima tutte le moltiplicazioni
	// poi la somma della riga
	for (cur_row = 0; cur_row < degree; cur_row++)
		for (cur_col = 0; cur_col < degree; cur_col++)
		{
			//sleep(1);
			// Leggo dalla coda, il primo della lista che prendo sarà il
			// prossimo che lavora (evito che i processi siano in busy waiting)
			mess_q_receive_message (ret, msgid);
			i = (int) ret->mtype - 1;

			// controllo se sia una somma, se lo è diminnuisco il contatore e se
			// questo è a zero sblocco la somma
			if (ret->com == MULTIPLY)
				if (--n_op == 0)
					unlock_semaphore(idSem, SEM_S, N_SEM);

			command->com = MULTIPLY;
			command->com_value.mul_val.row = cur_row;
			command->com_value.mul_val.col = cur_col;
			pipe_send_message (pipes[i], command);
		}

	for (sum_row = 0; sum_row < degree; sum_row++)
	{
		// Mando la somma relativa alla riga cur_row
		mess_q_receive_message (ret, msgid);
		i = (int) ret->mtype - 1;

		// controllo se sia una somma, se lo è diminuisco il contatore e se
		// questo è a zero sblocco la somma
		if (ret->com == MULTIPLY)
			if (--n_op == 0)
				unlock_semaphore(idSem, SEM_S, N_SEM);

		command->com = SUM;
		command->com_value.sum_row = sum_row;
		pipe_send_message (pipes[i], command);
	}

	// Nel caso non avessi già catturato tutti i return di MULTIPLY lo faccio,
	// altrimenti non riesco a sbloccare la somma.
	while(n_op > 0)
	{
		mess_q_receive_message (ret, msgid);
		i = (int) ret->mtype - 1;

		// controllo se sia una somma, se lo è diminnuisco il contatore e se
		// questo è a zero sblocco la somma
		if (ret->com == MULTIPLY)
			if (--n_op == 0)
				unlock_semaphore(idSem, SEM_S, N_SEM);
	}

	// Avviso tutti che il lavoro è finito in modo che possano terminare
	command->com = KILL;
	for (i = 0; i < n_child; ++i)
	{
		pipe_send_message (pipes[i], command);
	}

	// libero la memoria
	free (command);
	free (ret);
}

void child_work (int id, int pipe[2])
{
	// Alloco memoria per leggere e scrivere al padre
	command = (f_com *) malloc(sizeof(f_com));
	ret = (ret_mess *) malloc(sizeof (ret_mess));

	// Setto l'id per far sapere al padre chi sono quando gli rispondo
	// ed invio il primo messaggio a vuoto per fargli sapere che sono
	// in attesa di fare qualcosa
	ret->mtype = (long)(id+1);
	ret->finish = true;
	ret->com = START;
	mess_q_send_message (ret, msgid, idSem);

	while(1)
	{
		// Ricevo il comando
		pipe_receive_message(pipe, command);

		// Se è una moltiplicazione setto le opportune variabili, printo quello
		// che sto per fare e moltiplico
		if(command->com == MULTIPLY)
		{
    		my_printf("Child (%d): %d do -> comand %d row %u col %u\n",
    				id+1, getpid(), command->com,
					command->com_value.mul_val.row,
    				command->com_value.mul_val.col);

    		ret->com = command->com;
			ret->row = (int)command->com_value.mul_val.row;

			do_multiplication(&data_l[MAT_A], &data_l[MAT_B], &data_l[MAT_C], command->com_value.mul_val.row, command->com_value.mul_val.col);
		}

		// Uguale per la somma
		else if(command->com == SUM)
		{
			// Macro perchè Linux e Mac evidentemente trattano i maniera di versa il tipo
			// int32_t
			#ifdef __linux__
				my_printf("Child (%d): %d do -> comand %d sum_row %lu\n",
						id+1, getpid(), command->com, command->com_value.sum_row);
			#else
				my_printf("Child (%d): %d do -> comand %d sum_row %u\n",
						id+1, getpid(), command->com, command->com_value.sum_row);
   			#endif
			do_sum(&data_l[SUM_ROW], &data_l[MAT_C], command->com_value.sum_row, idSem);
		}

		// Se mi avvisa che il lavoro è finito esco
		else if(command->com == KILL)
		{
			my_printf("Process %i are finishing.\n", id+1);
			break;
		}

		// Rispondo
		mess_q_send_message(ret, msgid, idSem);
	}

	free (command);
	free (ret);
}

void hup_handler_f(int sig)
{
	free_all(data_l);
	mess_q_remove(msgid);
	//pipe_close(n_child, pipes);
	delete_semaphore_set(idSem);

	free(command);
	free(ret);
	exit(-1);
}

void hup_handler_c(int sig)
{
	free(command);
	free(ret);
	exit(-1);
}

void free_all(shm_data data_l[4])
{
	size_t i;

	// Libero tutta la memoria condivisa
    for (i = 0; i < 4; i++)
        free_data (&data_l[i]);
}

void init_all (shm_data data_l[4], char **argv)
{
	int i, j;
    int degree = atoi (argv[4]);

	data_l[MAT_A].path_to_file = argv[1];
	data_l[MAT_B].path_to_file = argv[2];
	data_l[MAT_C].path_to_file = argv[3];
    data_l[SUM_ROW].path_to_file = argv[0];

    alloc_data (&data_l[MAT_A], degree, degree, sizeof(int));
    alloc_data (&data_l[MAT_B], degree, degree, sizeof(int));
    alloc_data (&data_l[MAT_C], degree, degree, sizeof(int));
    alloc_data (&data_l[SUM_ROW], 1, 1, sizeof(int));

    // Inizializzo le variabili (caricando da file o ponendo a zero il valore)
    load_matrix_from_file (&data_l[MAT_A]);
    load_matrix_from_file (&data_l[MAT_B]);

    for (i = 0; i < degree; ++i)
        for (j = 0; j < degree; ++j)
            data_l[MAT_C].shmaddress[i*degree+j] = 0;

    *(data_l[SUM_ROW].shmaddress) = 0;
}