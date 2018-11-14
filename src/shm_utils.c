/** @file shm_utils.c
 * @author Matteo Piran
 * @date 14 May 2017
 * @brief Libreria di funzioni per la gestione e la creazione di share memory.
 */

#include "shm_utils.h"
#include "utility.h"

unsigned int sizeof_dm (shm_data *data)
{
    // Calcolo la dimensione della matrice che è numero di righe * numero di
    // colonne * dimensione del tipo di dati
    return data->rows * data->cols * data->size_element;
}

void alloc_data (shm_data *data, int rows, int cols, size_t sizeElement)
{
    // setto la dimenisone di righe, colonne e dei dati che devo contenere
    data->rows = rows;
    data->cols = cols;
    data->size_element = sizeElement;
	size_t sizeMatrix = sizeof_dm (data);

    // Credo l'area di memoria e memorizzo nella struttura il suo ID. COntrollo
    // anche che non ci siano stati problemi
	if ((data->shmid = shmget(get_key (data), sizeMatrix, IPC_CREAT|0777)) == -1)
    {
		my_perror ("(SHARE_MEM_UTILS.c) matrix shmget failed.");
        exit (1);
    }

    // Mi faccio resturire l'indirizzo dell'area di memoria creata e lo salvo
    // nella struttura. Controllo anche che non ci siano stati errori
	if ((data->shmaddress = (int *) shmat(data->shmid, NULL, 0)) == (void *)-1)
    {
        my_perror ("(SHARE_MEM_UTILS.c) matrix shmat failed.");
        exit (1);
    }
}

int get_key (shm_data *data)
{
    // calcolo con ftok la chiave dato il path, mentre il numero lo decido io
    return ftok(data->path_to_file, 1);
}

void free_data (shm_data *data)
{
    // Faccio l'unmap del segmento di area di memoria il cui indirizzo di partenza
    // è contenuto nella struttura data e controllo se è andato a buon fine.
    if (shmdt (data->shmaddress) == -1)
    {
        my_perror ("(SHARE_MEM_UTILS.c) shmdt failed.");
        exit (1);
    }

    // Elimino l'area di memoria condivisa indentificata dal valore contenuto nella
    // struttura data. Controllo anche qua che non ci siano stati errori
    if (shmctl (data->shmid, IPC_RMID, 0) == -1)
    {
        my_perror ("(SHARE_MEM_UTILS.c) shmctl IPC_RMID failed.");
        exit (1);
    }
}

void load_matrix_from_file (shm_data *data)
{
    int rw, i=0, fd;
    char *buff = calloc (4096, sizeof(char));               // buff per lettura
    char *line, *val, *brkt, *brkb; // temp string per strtok

    if ((fd = open(data->path_to_file, O_RDONLY)) == -1) 
    {
        my_perror("(UTILS.C) : open matrix B");
        exit(1);
    }

    // leggo a blocchi
    while ((rw = read(fd, buff, sizeof(buff))) > 0) 
    {
        // uso stringtokenizer per dividere
         for (line = strtok_r(buff, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) 
         {
            for (val = strtok_r(line, ",", &brkb); val; val = strtok_r(NULL, ",", &brkb)) 
            {
                // Assegno il valore che atoi converte alla mia matrice
                data->shmaddress[i] = atoi (val);
                i++;
            }
        }
    }

    //printf("%s: %u\n", data->path_to_file, data->shmaddress[0]);

    if (rw == -1)
    {
        my_perror ("SHM_UTILS.C) : read error");
        exit (1);
    }

    close (fd);
    free(buff);
}


void write_matrix_to_file (shm_data *mat) {
    int i,j, fd;
    char buf [4096];               // buff per scrittura
    char val [11];

    if ((fd = open(mat->path_to_file, O_WRONLY | O_TRUNC)) == -1)
    {
        my_perror("(SHM_UTILS.C) : open matrix C");
        exit(1);
    }

    for(i=0; i<mat->rows; i++)
    {
        strcpy(buf, "");
        for(j=0; j<mat->cols; j++)
        {
            if (j == mat->cols-1)
                sprintf(val, "%d", mat->shmaddress[i*mat->rows+j]);
            else
                sprintf(val, "%d,", mat->shmaddress[i*mat->rows+j]);

            strcat(buf, val);
        }
        strcat(buf, "\n");
        if (write(fd, buf, strlen(buf)) == -1)
            my_perror("(SHM_UTILS.C) : write");
    }

    close(fd);
}
