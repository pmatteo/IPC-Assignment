
/** @file semaphore.h
 * @author Piran Matteo
 * @date 30 May 2017
 * @brief Il file è una serie di funzioni utili per la gestione/creazione dei semafori
 * e sulle operazioni che si possono eseguire su di essi.
 * 
 */
#include "utility.h"
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef semaphore_h
	#define semaphore_h
	
	/**
	 * @brief Funzione che setta i valori dei semafori della lista identificata
	 * da id con quelli nella lista vals, passata dall'utente.
	 * 
	 * @param id id della lista dei semafori su cui operare.
	 * @param vals lista dei valori con cui settare i sem.
	 */
	void set_all_semaphores(int id, unsigned short* vals);

	/**
	 * @brief Funzione che permatte di creare un set di semafori di dimensione sem_number e setta i valori iniziali con quelli contenuti in vals.
	 * 
	 * @param sem_number dimensione del set da creare.
	 * @param vals lista dei valori con cui settare i sem.
	 * 
	 * @return identificativo della coda di semafori.
	 */
	int create_semaphores_set(int sem_number, unsigned short* vals);

	/**
	 * @brief Elimina un insieme di semafori.
	 * 
	 * @return identificativo della coda di semafori.
	 */
	void delete_semaphore_set(int id);

	/**
	 * @brief Funzione che emula la P()
	 * @details La funzione emula il comportamento della funzione P() vista
	 * nelle lezioni di teoria. In sostanza cerca di togliere 1 dal valore del
	 * semaforo alla posizione i (< sem_number) nella lista identificta da id.
	 * Nel caso l'operaizone riuscisse si può entrare in una sezione critica,
	 * altrimenti il processo chiamante viene messo in attesa finchè
	 * l'operazione non riuscirà a concludersi.
	 * 
	 * @param id id della lista dei semafori.
	 * @param i indice della lista (semaforo iesimo).
	 * @param sem_number numero massimo dei semafori.
	 */
	void lock_semaphore(int id, int i, int sem_number);

	/**
	 * @brief Funzione che emula la V().
	 * @details La funzione emula il comportamento della funzione V() vista
	 * nelle lezioni di teoria. Cerca di agguingere 1 dal valore del semaforo
	 * alla posizione i (< sem_number) nella lista identificta da id. La
	 * funzione dunque va a liberare uno dei processi in attesa di entrare
	 * nella sezione critica, nel coso non ci fossero il primo ad arrivare può
	 * entrare.
	 * 
	 * @param id id della lista dei semafori.
	 * @param i indice della lista (semaforo iesimo).
	 * @param sem_number numero massimo dei semafori.
	 */
	void unlock_semaphore(int id, int i, int sem_number);

	#endif