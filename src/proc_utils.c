#include "proc_utils.h"

#define SEM_Q 0
#define SEM_S 1
#define N_SEM 2

int mess_q_create (char * path, int id) 
{
    // genero la chiave per la coda
    int msgkey = ftok(path, id);

    // credo la cda in memoria e mi salvo l'ID per il return
    int msgid = msgget (msgkey, 0666 | IPC_CREAT);

    // Controllo cbe non ci siano stati errori
    if (msgid == -1) 
    {
        my_perror("(PROC_UTILS.C) msgget creation failed.");
        exit(1);
    }

    return msgid;
}

void mess_q_receive_message (ret_mess *ret, int msgid) 
{   
    // Prendo la dimensione da leggere meno il campo mtype
    const unsigned size = sizeof (ret_mess) - sizeof (long);

    // Cerco di leggere dalla coda e controllo che non ci siano stati errori
    if (msgrcv(msgid, ret, size, 0, 0) == -1)
    {
        my_perror("(PROC_UTILS.C) Failed to receive message queue");
        exit(1);
    }
}

void mess_q_remove (int msgid) 
{
    // Rimuovo la coda e controllo che non ci siano stati errori
    if (msgctl(msgid, IPC_RMID, NULL) == -1) 
    {
        my_perror("(PROC_UTILS.C) msgctl failed.");
        exit(1);
    }
}

void mess_q_send_message (ret_mess *ret, int msgid, int semid) 
{
    // Calcola la lunghezza del messaggio senza il campo mtype
    const unsigned size = sizeof (ret_mess) - sizeof (long);

    // Controllo che non ci sia nessuno, se qualcuno sta già inviando qualcosa
    // mi blocco e aspetto il mio turno.
    lock_semaphore (semid, SEM_Q, N_SEM);
    // Invio il messaggio e controllo che non ci siano stati errori
    if (msgsnd (msgid, ret, size, 0) == -1)
        my_perror ("(PROC_UTILS.C) Failed to send message queue");
    // Sblocco il semaforo
    unlock_semaphore (semid, SEM_Q, N_SEM);
}

void pipe_close (int n_child, int pipes[n_child][2]) 
{
    int i;
    // Scorro la pipe
    for (i = 0; i < n_child; ++i) 
    {  
        // CHiudo la pipe in scrittura e controllo che non ci siano errori
        if (close (pipes[i][0]) == -1) 
        {
            my_perror("(PROC_UTILS.C) close pipe[0] failed.");
            exit(1);
        }

        // Chiudo la paipe in lettura e controllo che non ci siano errori
        if (close (pipes[i][1]) == -1)
        {
            my_perror("(PROC_UTILS.C) close pipe[1] failed.");
            exit(1);
        }
    }
}

void pipe_create (int n_child, int pipes[n_child][2]) 
{
    int i;
    // Scorro la lista delle pipe
    for (i = 0; i < n_child; ++i) 
    {
        // Creo la lista iesima e controllo che non ci siano errori
        if (pipe (pipes[i]) == -1) 
        {
            my_perror("(PROC_UTILS.c) create pipe failed.");
            exit(1);
        }
    }
}

void pipe_receive_message (int *pipe, f_com *command) 
{
    // Leggo dalla pipe passata come parametro e controllo che non ci siano 
    // stati errori
    if (read(pipe[0], command, sizeof(f_com)) == -1) 
    {
        my_perror ("(PROC_UTILS.C) receive pipe failed.");
    }
}

void pipe_send_message (int *pipe, f_com *command) 
{
    // Leggo dalla pipe passata come parametro e controllo che non ci siano
    // stati errori
    if (write (pipe[1], command, sizeof (f_com)) == -1) 
    {
        my_perror ("(PROC_UTILS.C) send with pipe failed.");
    }
}

/** @name Operazioni dei figli. */ 
/*@{ */
void do_sum (shm_data *sum_element, shm_data *matC, int row, int semid)
{
    int col, sum = 0;

    // Controllo che non ci sia nessuno che sta già eseguendo operazioni chiamando
    // chiamando la lock del semaforo associato alla somma. Nel caso sia già
    // qualcuno presente nella sezione crtitica mi blocco
    lock_semaphore(semid, SEM_S, N_SEM);
    // faccio la somma
    for (col = 0; col < matC->cols; ++col)
    {
        sum += matC->shmaddress[row*matC->rows + col];

    }

    *(sum_element->shmaddress) += sum;
    // Esco dalla sezione critica e faccio l'unlock in modo tale che il prossimo in
    // coda possa entrare
    unlock_semaphore(semid, SEM_S, N_SEM);
}

void do_multiplication (shm_data *matA, shm_data *matB, shm_data *matC, int i, int j)
{
    int k, val = 0;
    // Scorro l'indice
    for (k = 0; k < matC->rows; ++k)
    {
        // matC[i, j] += matA[i, k]*matB[k,j]
        // printf("val += matA[%u*%u+%u] X matB[%u*%u+%u] -> %u += %u X %u\n",
        //     i, matA->rows, k,
        //     k,matB->rows, j,
        //     val,
        //     (matA->shmaddress[i*matA->rows + k]),
        //     (matB->shmaddress[k*matB->rows + j]));
        val +=  (matA->shmaddress[i*matA->rows + k])
                *
                (matB->shmaddress[k*matB->rows + j]);
    }

    // Inserisco il valore calcolato in  matC[i, j]
    (matC->shmaddress[i*matC->rows + j]) = val;

    //     printf("(matC->shmaddress[%u*%u + %u]) = val -> %u = %u \n",
    //         i, matC->rows, j,
    //         (matC->shmaddress[i*matC->rows + j]),
    //         val);
}
/*@} */
