/** @file proc_utils.h
 * @author Piran Matteo
 * @date 28 May 2017
 * Il file definisce una serie di funzioni utili per la comunicazione
 * interprocesso. 
 * In particolare per creare, distruggere, mandare e ricevere messaggi
 * attraverso le pipe e le code di messaggi. Queste funzioni sono pensate ad
 * uso esclusivi del file process.c.
 * 
 */

#include "semaphore.h"
#include "utility.h"
#include "shm_utils.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/msg.h>


#ifndef work_manegement_h
	#define work_manegement_h
	
	/**
	 * @brief Enum per determinare quale comando un figlio ha ricevuto dal padre.
	 * 
	 */
	typedef enum e_comand {
		/*@{*/
		MULTIPLY = 0, 	///< identifica la moltiplicazione.
		SUM,			///< identifica la somma.
		START,			///< comando che il figlio manda per far sapere che è pronto.
		KILL			///< comando di terminazione.
		/*@}*/
	} e_comand;


	/**
	 * @brief	Union che può contenere o riga e colanna della matrice C se il
	 * comando è una moltiplicazione oppure la riga della matrice C se il
	 * comando è una somma.
	 * 
	 */
	typedef union value {
		/*@{*/
		struct multiply_val {
			uint16_t row;	///< Riga matrice C per moltiplicazione.
			uint16_t col;	///< Colonna matrice C per moltiplicazione.
		} mul_val;			///< Struct per le righe e colonne.
		uint32_t sum_row;	///< Riga matrice C per la somma.
		/*@}*/
	}value;

	/** @name Tipi di messaggi. */ 
	/*@{ */
	/**
	 * @brief 	Struct che definisce il comando che il padre manda al figlio.
	 * @details La struct, inviata tramite pipe dal padre a un figlio, contiene
	 * l'enum e_comand (che identifica il tipo di comando che il figlio deve 
	 * eseguire) e la union value contentente i dati che servono al figlio per
	 * capire dove eseguire le operazioni.
	 * 
	 */
	typedef struct father_comand {
		/*@{*/
		enum e_comand com;		///< Comando che il figlio deve eseguire.
		union value com_value;	///< Valori per l'esecuzione.
		/*@}*/
	} f_com;

	/**
	 * @brief 	struct che definisce il "pacchetto" di informazioni che il
	 * figlio manda al padre dopo aver completato un comando.
	 * @details struct che vinee inviata sulla coda si messaggi contentente
	 * l'esito dell'operazione che il figlio ha compiuto e che permette al
	 * padre di capire se l'operazione è andata a buon fine o deve essere
	 * ripetuta. Il campo mtype serve al padre per capire quale figlio ha
	 * inviato il messaggio, occhio però che se il figlio è l'ennesimo mtype
	 * conterrà n+1 in quanto il valore 0 non è consantito.
	 */
	typedef struct return_message {
		/*@{*/
		long mtype;			///< long necessario per la coda di messaggi.
		bool finish;		///< Valore per l'esito dell'operazione.
		enum e_comand com;	///< Comando che il figlio deve eseguire.

		int row;
		/*@}*/
	} ret_mess;
	/*@} */

	/**
	 * @brief 	Funzione per la creazione di una coda di messaggi.
	 * @details	La funzione permette di creare una coda di messaggi e ne
	 * restituisce l'ID creato. La key utilizzata per la creazione della coda
	 * usa ftok con i parametri passati come input. 
	 * @note
	 * Si noti che la lettura da pipe è bloccante.
	 * 
	 * @param path 	path che server a ftok per la generazione della chiave.
	 * @param id 	id anch'esso usato per la genrazione della chiave.
	 * 
	 * @return 		l'ID della coda di messaggi creata.
	 */
	int mess_q_create(char * path, int id);

	/**
	 * @brief	Funzione che permette di inviare un messaggio su una coda di
	 * messaggi.
	 * @details La funzione in questione permette di ricevere un messaggio da
	 * una coda di messaggi il cui id è passato come parametro alla funzione.
	 * Il messaggio letto verrà inserito nella struttura di tipo ret_mess
	 * passata anche'essa come parametro.
	 * @note
	 * Si noti che la lettura dalla coda è bloccante.
	 * 
	 * @param ret	puntatore alla struttura di tipo ret_mess su cui scrivere
	 * i dati letti dalla coda di messaggi.
	 * @param msgid	id della coda su cui leggere.
	 * 
	 */
	void mess_q_receive_message(ret_mess *ret, int msgid);

	/**
	 * @brief	Funzione per eliminare una coda esistente.
	 * @details Dato un ID come input la funzione permette di eliminare la coda
	 * con quell'identificativo.
	 * 
	 * @param msgid identificativo della coda da eliminare.
	 */
	void mess_q_remove(int msgid);

	/**
	 * @brief Funzione che permette di inviare un messaggio su una coda di
	 * messaggi in maniera concorrente.
	 * @details La funzione prende permette di inviare sulla coda di messaggi
	 * identificata da 'id' una messaggio (una struct di tipo ret_mess e già
	 * opportunamente settata) il quale è passato come parametro. La funzione
	 * inoltre assicura che l'invio dei messaggi venga fatto in maniera
	 * concorrente attraverso le opportune funzioni di lock_semaphore e
	 * unlock_semaphore definite nel file semaphore.h e implementate nel
	 * corrispondente file c. L'id del semaforo passato come parametro è
	 * infatti quello su cui eseguire le operazioni.
	 * @note
	 * Si noti che la funzione conosce già il numero del semaphoro (0) e la
	 * dimensione dell'array dei semafori(3). Questo perchè è pensata per
	 * essere da supporto a process.c, non è una libreria general pourpose.
	 * 
	 * @param ret	puntatore ad una struct di tipo ret_message da inviare come
	 * messaggio
	 * @param msgid	id della coda su cui mandare il messaggio.
	 * @param semid	id del semaforo per l'accesso concorrente.
	 */
	void mess_q_send_message(ret_mess *ret, int msgid, int semid);

	/**
	 * @brief 	Funzione per la chiusura delle pipe.
	 * @details La funzione permette di chiudere tante pipe quanti sono i
	 * processi in esecuzione, numero passato come parametro alla funzione.
	 * 
	 * @param n_child 	numero di pipe da chiudere.
	 * @param pipes 	puntatore alla lista di pipe su cui eseguire le close.
	 */
	void pipe_close(int n_child, int pipes[n_child][2]);

	/**
	 * @brief 	Funzione per l'apertura delle pipe.
	 * @details La funzione permette di aprire tante pipe quanti sono i
	 * processi in esecuzione, numero passato come parametro alla funzione.
	 * 
	 * @param n_child 	numero di pipe da aprire.
	 * @param pipes 	puntatore alla lista di pipe su cui eseguire le open.
	 */
	void pipe_create(int n_child, int pipes[n_child][2]);

	/**
	 * @brief 	Funzione che permette di ricevere un messaggio su una pipe.
	 * @details Data una pipe e un puntatore ad una struttura di tipo f_com, la
	 * funzione permette di ricevere un comando dalla pipe e di inserirlo
	 * direttamente nella struttura passata come parametro.
	 * 
	 * @param pipe 		puntatore alla pipe su cui leggere.
	 * @param command 	puntatore al comando su cui memorizzare i dati letti.
	 */
	void pipe_receive_message(int *pipe, f_com *command);

	/**
	 * @brief 	Funzione che permette di inviare un messaggio su una pipe.
	 * @details Data una pipe e un comando (già opportunamente settato), la
	 * funzione permette di inviarem tale comando (definito dalla struct f_com)
	 * sulla pipe indicata.
	 * 
	 * @param pipe 		puntatore alla pipe su cui inviare il comando.
	 * @param command 	puntatore al comando da mandare.
	 */
	void pipe_send_message(int *pipe, f_com *command);

		/** @name Operazioni dei figli. */ 
	/*@{ */

	/**
	 * @brief Opera la somma degli elementi di una riga sulla memoria condivisa
	 * passata come parametro.
	 * @details La funzione calcola la somma degli elementi alla riga 'row'
	 * passata in input, sulla matrice  gestita dalla struttura 'matC' passata
	 * anch'essa in input. La somma poi viene aggiornata con quella già
	 * parzialmente prensente nell'area di memoria passata in input attraverso
	 * la struttura 'sum_element'. Il tutto usando i semafori di tipo SystemV
	 * identificati da 'semid', quindi nell'operazione è garantito l'accesso
	 * concorrente alla risorsa 'sum_element'.
	 * 
	 * @param sum_element struttura share_memory_data contenente la somma
	 * parziale degli elementi della matrice matC.
	 * @param matC matrice su cui fare la somma della rigsa row.
	 * @param row Riga su cui fare la somma degli elementi.
	 * @param semid ID del semaforo adibito a questa operazione.
	 */
	void do_sum (shm_data *sum_element, shm_data *matC, int row, int semid);

	/**
	 * @brief Funzione che calcola l'elemento i,j della matrice 'matC' passata
	 * come parametro.
	 * @details Nell'operazione di moltiplicazione AxB fra due matrici, la
	 * funzione calcola l'elemento i,j della matrice C risultante. Le matrici
	 * sono contenute nelle strutture matA, matB e matC. Anche i e j sono dati
	 * in input. Quindi la funzione, data le normale struttura dell'operazione
	 * di moltiplicazione fra due matrici calcola: 
	 \f[
	 \sum_{k=0}^{degreeOfC}{A_{i,k}*A_{k,j}}
	 \f]
	 * L'operazione non ha bisogno di essere fatta in maniera concorrente in
	 * quanto i vari processi lavorano su elementi diversi, quindi non possono
	 * sovrepporsi a livello di memoria.
	 * 
	 * @param matA matrice A dell'operazione AxB.
	 * @param matB matrice B dell'operazione AxB.
	 * @param matC matrice su cui insierire l'elemento calcolato alla posizione i,j
	 * @param i riga della matrice	 
	 * @param j colonna della matrice
	 */
	void do_multiplication (shm_data *matA, shm_data *matB, shm_data *matC, int i, int j);


	#endif