/** @file process.h
 * @author Piran Matteo
 * @date 29 May 2017
 * Creazione, gestione di tutto quello che riguarda padre figli e suddivisione
 * dei lavori (scheduling dei figli, la gestione dei comandi, delle operazione,
 * dei segnali etc..)
 * 
 */

#include "shm_utils.h"
#include "proc_utils.h"
#include "semaphore.h"
#include "utility.h"
#include <sys/wait.h>

#ifndef process_h
	#define process_h
    
    /**
     * @brief funzione che si occupa della creazione dei figli e di iniziare a
     * farli lavorare.
     * @details main_process è la funzione che si occupa anche di inizializzare
     * tutte le strutture di supporto alla comunicazione tra padre e figlio
     * (pipe, code semafori..) e poi crea il numero di figli che gli viene
     * passato come parametro e li mette al lavoro tramite la funzione child_work. 
     * 
     * @param shared_data lista di strutture per le matrici o la somma.
     * @param degree dimensione della matrices.
     * @param child numero di figli da creare.
     */
	void main_process (shm_data shared_data[4], int degree, int child);

    /**
     * @brief Scheuduling iniziale che non controlla e gestisce tutto.
     * @details Prima bozzza dello scheduling. E' funzionante (o almeno lo è su
     * MacOS). Non controlla che le moltiplicazioni di una riga siano siano state
     * fatte tutte prima di mandare la somma. Usa un pò il metodo dello struzzo.
     * Da notare che il meccanismo funziona al 99% su MacOS, avendo passato tutti
     * test fatti tranne 1. Su Linux funzione veramente poco. Ho ul sospetto che
     * lo scheduling di Linux (che usa un meccanismo che alza di molto la priorità
     * all'abbassarsi del tempo di esecuzione di un processo) faccia si che le
     * moltiplicazioni vengano rimandate molto di più rispetto a quello che accade
     * su MacOS, e permettendo alle somme di essere eseguite prima che le
     * moltiplicaizoni relative a quella riga finiscano. E' solo un'ipotesi ma è la
     * migliore che ho trovato. 
     * 
     * @param degree dimansione della matrice.
     * @param pipes lista delle pipe.
     */
    void scheduling (int degree, int pipes[][2]);

    /**
     * @brief Funzione che permette al padre di fare lo scheduling dei figli
     * per assegnare somme o moltiplicazioni.
     * @details La funzione implementa uno scheduling die figli da parte di un
     * padre, quindi come viene distribuito il lavoro tra i figli. Per
     * determinare quale sarà il prossimo figlio che dovrà essere eseguito il
     * padre legge dalla coda di messaggi, prende il campo mtype e gli invia
     * il prossimo comando con relativi parametri (i figli mandano quando
     * partono un messaggio per far sapere che son pronti, se così non fosse
     * il meccanismo non funzionerebbe). Quando tutti i comandi sono stati
     * esauriti i padre manda un messaggio a figli per avvisarli che tutto è
     * stato calcolato e possono temrinare. 
     * @note mtype contiene il numero del figlio, se il valore è i allora il
     * padre dovrà contattare l'iesimo figlio attraverso la pipe i-1, in quanto
     * mtype consente valori >= 1 mentra la lista delle pipe parte da 0.
     * 
     * @param degree dimansione della matrice.
     * @param pipes lista delle pipe.
     */
    void scheduling_adv (int degree, int pipes[][2]);

    /**
     * @brief Funzione che il figlio lancia appena avviato e che consiste nel ricevere i messaggi dal padre e eseguire le operaizoni richieste.
     * @details La funzione gestisce tutto quello che riguarda un processo figlio. All'inizio vengono create le strutture di supporto per leggere e scrivere sui meccanismi di IPC, poi si avvisa il padre che si è operativi attraverso l'apposito comando. Da notare che il campo mtype della struct per la risposta attraverso la coda di messaggi, contiene il numero del figlio. Poi parte un loop infinito dal quale si esce o tramite SIGINT o quando il padre avvisa che il processo può temrinare. Nel ciclo se arriva un messaggio di somma/moltiplicaizone la funzione stampa le relative informazioni e chiama la funzione di somma passando i parametri che il padre gli ha dato.
     * 
     * @param id numero del figlio, lui non può sapere per cui deve essergli passato dal padre.
     * @param pipe la propria pipe, quindi quella alla posizione id-1 nella lista che il padre crea.
     */
    void child_work (int id, int *pipe);

    /** @name Gestione segnali. */ 
    /*@{ */
    /**
     * @brief Gestione dei segnali di terminazione per il padre.
     * 
     * @param sig segnale.
     */
	void hup_handler_f (int sig);

    /**
     * @brief Gestione dei segnali di terminazione per i figli.
     * 
     * @param sig segnale
     */
    void hup_handler_c(int sig);
    /*@} */

    /** @name Operazioni sulla memoria. */ 
    /*@{ */
    /**
     * @brief Funzione per iniziallizzare delle strutture contenenti anche le
     * zone di memoria condivisa.
     * 
     * @param data_l lista di strutture per le matrici o la somma.
     * @param argv parametri per l'inizializzazione.
     */
	void init_all (shm_data data_l[4], char **argv);

    /**
     * @brief Funzione per fare la free dei dati che ho in memoria condivisa
     * e delle strutture che la contengono.
     * 
     * @param data_l lista di strutture per le matrici o la somma.
     */
	void free_all (shm_data data_l[4]);
    /*@} */
    
    #endif