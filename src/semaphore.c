#include "semaphore.h"

void set_all_semaphores(int id, unsigned short* vals)
{
	// Controllo che la lista dei valori da settare non sia null
	if(vals == NULL) 
	{
		my_perror("(SEM_MANAGEMENT.C) error vals must be != NULL");
		return ;
	}
	// Setto i valori della lista di semafori con quelli contenuti in vals
	semctl(id, 0, SETALL, vals);
}

int create_semaphores_set(int n, unsigned short* vals)
{
	int id;

	// Controllo che il numero di semafori da creare non sia minore di 1
	if(n < 1)
	{
		my_printf("(SEM_MANAGEMENT.C) error sem number must be > 0");
		exit(1);
	}

	// Credo la coda di semafori e mi salvo il valore per restituirlo 
	id = semget(IPC_PRIVATE, n, SHM_R | SHM_W);

	// Setto tutti i valori con quelli passati come parametro
	semctl(id, 0, SETALL, vals);

	return id;
}

void delete_semaphore_set(int id)
{
	// Cancello la lista di semafori passata in input e controllo che non ci
	// Siano stati errori
	if(semctl(id, 0, IPC_RMID, NULL) == -1) 
	{
		my_perror("Error releasing semaphore!");
	}
}

void lock_semaphore(int id, int i, int sem_num)
{
	// Creo la struttura per eseguire operazioni sui sem e la inizzializzo
	struct sembuf sb = {i, -1, 0};
	// sb.sem_num = i;
	// sb.sem_op = -1;
	// sb.sem_flg = 0;

	// Tolgo 1 al semaforo e controllo che non ci siano problemi
	// Equivalente di P()
	if(semop(id, &sb, sem_num-1) == -1)
		my_perror("LOCK SEM");
}

void unlock_semaphore(int id, int i, int sem_num)
{
	// Creo la struttura per eseguire operazioni sui sem e la inizzializzo
	struct sembuf sb = {i, 1, 0};
	// sb.sem_num = i;
	// sb.sem_op = 1;
	// sb.sem_flg = 0;

	// Aggiungo 1 al semaforo e controllo che non ci siano problemi
	// Equivalente di V()
	if(semop(id, &sb, sem_num-1) == -1)
		my_perror("UNLOCK SEM");
}