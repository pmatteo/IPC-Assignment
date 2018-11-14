/** @file shm_utils.h
 * @author Piran Matteo
 * @date 8 Jun 2017
 * @brief Il file definisce una serie di funzioni e strutture che permettono di
 * memorizzare informazioni di tipo intero all'interno della memoria condivisa.
 * Il file contiene funzioni e struct abbastanza generiche e riutilizzabili
 * anche se alcune sono molto specifiche per l'applicazione.
 * 
 */
#include "semaphore.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef shm_utils_h
	#define shm_utils_h
	
	/**
	 * @brief strutture che permettere di gestire meglio sia l'allocazione che 
	 * le operazioni sulla memoria e il suo contenuto.
	 * 
	 */
	typedef struct share_memory_data {
		/*@{*/
		char *path_to_file;		///< path al file da cui eventualmente caricare la matrice e che serve anche per creare la chiave con ftok.
		int *shmaddress;		///< Puntatore al primo indirizzo di memoria  condivisa.
		int rows, cols;			///< Riga e colonna della matrice,servono anche  per determinare la dimensione dello spazio da allocare in memoria condivisa.
		size_t size_element;	///< Dimensione dell'elemento, serve nel caso si volesse usare qualsiasi tipo di dato invece che soli interi.
		int shmid;				///< ID fell'area di memoria.*/
		/*@}*/
	} shm_data;

	/** @name Operazioni sulla memoria. */ 
	/*@{ */
	/**
	 * @brief Funzione per calcolare la dimensione da allocare in memoria
	 * condivisa.
	 * 
	 * @param data puntatore all'elemento da cui prendere i parametri per calcolare la dimensione.
	 * @return dimensione di spazio da allocare.
	 */
	unsigned int sizeof_dm (shm_data *data);

	/**
	 * @brief Funzione che alloca spazio in memoria condivisa.
	 * @details La funzione prende in input un puntatore a share_memory_data, e
	 * ne setta numero di righe, colonne e la dimensione dell'elemento. Crea
	 * poi una nuova zona di memoria condivisa, salva l'ID nella struttura e
	 * alloca spazio in memoria (usando la funzione 'sizeof_dm'). In poche
	 * parole e un inizializzatore della struttura.
	 * 
	 * @param data puntatore ad una zona di memoria di tipo share_memory_data
	 * che deve essere iniziallizato
	 * @param rows numero di righe.
	 * @param cols numero di colonne.
	 * @param sizeElement dimensione per l'elemento.
	 */
	void alloc_data (shm_data *data, int rows, int cols, size_t sizeElement);

	/**
	 * @brief Libera lo spazio in memoria condivisa gestito dall struttura di
	 * tipo share_memory_data passata come parametro alla funzione.
	 * 
	 * @param data puntatore ad una zona di memoria di tipo share_memory_data
	 * che deve essere deallocato.
	 */
	void free_data (shm_data *data);

	/**
	 * @brief Calcola la chiave con ftok per la struttura passata come parametro.
	 * 
	 * @param data puntatore ad una zona di memoria di tipo share_memory_data
	 * di cui deve essere calcolata la chiave.
	 * @return la chiave associata a quella struttura.
	 */
	int get_key (shm_data *data);
	/*@} */
	
	/*@} */

	/** @name Operazioni sui file. */ 
	/*@{ */
	/**
	 * @brief Funzione per leggere una matrice da file CSV.
	 * 
	 * @param mat [description]
	 */
	void load_matrix_from_file (shm_data *mat);

	/**
	 * @brief Funzione per scrivere una matrice su file CSV.
	 * 
	 * @param mat [description]
	 */
	void write_matrix_to_file (shm_data *mat);

	/*@} */
	#endif