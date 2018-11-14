/** @file utility.h
 * @author Piran Matteo
 * @date 5 Jun 2017
 * @brief Il file definisce una serie di funzioni utili per tutti, come la 
 * definizione di perror e printf che fanno uso di syscall per operare.
 * 
 */

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

#ifndef utility_h
	#define utility_h
	
	/**
	 * @brief Funzione che imita perror.
	 * @details La funzione è una rivisitazione personale della perror che fa 
	 * uso delle syscall direttamente.
	 * 
	 * @param s Stringa che verrà stampata insieme all'errore.
	 */
	void my_perror (char *s);

	/**
	 * @brief Funzione che imita la printf.
	 * @details Funzione che imita la printf attraverso però le syscall, in 
	 * realtà è molto simile all'originale.
	 * 
	 * @param format Stringa formattata per la stampa.
	 */
	void my_printf (const char *format, ...);

#endif
