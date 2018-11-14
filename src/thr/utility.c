/** @file utility.c
 * @brief Implementazine delle funzioni utili a tutti.
 * 
 */
#include "utility.h"

void my_perror (char *s) 
{
    char buf[256];

    strcpy(buf, s);
    strcat(buf, " : ");
    strcat(buf, strerror(errno));
    strcat(buf, "\n");
    
    write(STDERR_FILENO, buf, strlen(buf));
}


void my_printf (const char *format, ...)
{
	// struct per i parametri 
	va_list arg;

	// Prendo i parametri, li inserisco nella stringa formattata e li printo
	va_start(arg, format);
	vfprintf(stdout, format, arg);
	fflush(stdout);
	va_end(arg);
}