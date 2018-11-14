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

	/**
	 * @brief Funzione principale che il processo principale esegue. Qua
	 * vengono create tutte le strutture necessarie per l'esecuzione e i
	 * thread con relativo scheduling.
	 * 
	 * @param argv args passati al main.
	 */
	void scheduling_t(char **argv);

	/**
	 * @brief Funzione per caricare una matrice da file di tipo CSV.
	 * 
	 * @param mat matrice in cui caricare il file.
	 * @param path path al file.
	 * @param size dimensione della matrice.
	 */
	void load_matrix_from_file_t(matrix_t mat, char *path, int size);

	/**
	 * @brief Funzione per scrivere una matrice su un file di tipo CSV.
	 * 
	 * @param mat matrice da cui leggere i dati.
	 * @param path path al file. su cui scrivere.
	 * @param size dimensione della matrice.
	 */
	void write_matrix_to_file_t (matrix_t mat, char *path, int size);

	/**
	 * @brief Funzione per allocare spazio in memoria per una matrice.
	 * 
	 * @param size dimensione 
	 * @return ritorna un puntatore alla zona di memoria creata.
	 */
	matrix_t alloc_matrix(int size);

	/**
	 * @brief Funzione per fare la free della zona di memoria di una matrice.
	 * 
	 * @param mat puntatore alla zona di memoria della matrice.
	 * @param size dimensione della matrice.
	 */
	void free_matrix(matrix_t mat, int size);

	/**
	 * @brief [brief description]
	 * @details [long description]
	 * 
	 * @param args [description]
	 * @return [description]
	 */
	void * do_multiplication_t(void *args);

	/**
	 * @brief [brief description]
	 * @details [long description]
	 * 
	 * @param args [description]
	 * @return [description]
	 */
	void * do_sum_t(void *args);

	#endif	