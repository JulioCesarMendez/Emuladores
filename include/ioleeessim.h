/******************************************************************************
 * Generic I/O write/read modbus memory functions library declarations
 * Thorium (c) 2021,  Thorium Corp
 * Author(s): Julio Cesar Mendez 	<mendezjcx@tutanota.com>
 ******************************************************************************/
#ifndef _IOLEEESSIM_H
#define _IOLEEESSIM_H

#include <stdlib.h>             /* Standard Lib    C89   */
#include <time.h>

typedef struct _datoD {
	short valor;
	struct timespec timestamp;
	unsigned char flags;
} DATOD;

// Funciones de escritura
//extern 
int io_eschreg (unsigned int , unsigned int , short *);
//extern 
int io_escireg (unsigned int , unsigned int , short *);
//extern 
int io_esccoil (unsigned int , unsigned int , unsigned short );
//extern 
int io_escstat (unsigned int  , unsigned int , unsigned short );
//extern 
int io_esccoils (unsigned int , unsigned int , unsigned short *);
//extern 
int io_escstats (unsigned int  , unsigned int , unsigned short *);

// Funciones de Lectura del valor simple
//extern 
int io_leeireg (unsigned int , unsigned int , short *);
//extern 
int io_leecoil(unsigned int , unsigned int , unsigned short *);
//extern 
int io_leestat (unsigned int , unsigned int , unsigned short *);
//extern 
int io_leehreg (unsigned int , unsigned int , short *);

// Funciones de Lectura del dato con timestamp y status

// *******  Lectura en los holding con timestamp y status ********
int io_leehregDat (unsigned int , unsigned int , DATOD * );

//********  Lectura Input Registers con timestamp y status ***********/
int io_leeiregDat (unsigned int , unsigned int , DATOD *);

// ******** Lectura de los Coils con timestamp y estado       ********* /
int io_leecoilDat (unsigned int , unsigned int , DATOD *);

// *******  Lectura en los stats con timestamp y status********
int io_leestatDat (unsigned int , unsigned int , DATOD *);

// Funciones de apertura/cierre de memoria compartida
//extern 
int Aper_zm(void );
//extern 
void Desv_zm(void);

#endif  // _IOLEEESSIM_H
