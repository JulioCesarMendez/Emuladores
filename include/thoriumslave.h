/*
 * slave10.h para la version de Net-DAS 2.10 del 20/01/2016
 * Archivo de definiciones macro y prototipos de funciones y procedimientos 
 * Net-DAS v 2.10 (c) 2016 PDVSA Intevep, S.A.
 * Author(s): Julio Cesar Mendez  <mendezjcm@pdvsa.com>, <kuaimarejcmx@gmail.com>
 */
#ifndef _THORIUMSLAVE_H
#define _THORIUMSLAVE_H
/*
 * Globals defined in slaves.c
 */
extern unsigned short	verbose;
extern unsigned short	debug;
extern struct termios	termiosp;
extern PLC_Map		*plc_memory;
extern int		TRx;
extern int		fd;
extern long		offset;
extern long		SlaveAddress;
extern unsigned short	RTS, DTS, PRE, POST, STOP, BAUD;
extern char		port_name[80], str[64];
extern int		port_number;

#endif					/* _THORIUMSLAVE_H */
