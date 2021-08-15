/******************************************************************************
 * Funciones genericas de lectura y escritura de tabla modbus
 * Thorium, (c) 2021 Thorium Corp
 * Author(s): Julio Cesar Mendez <mendezjcx@tutanota.com>, <mendezjcx@gmail.com>
 ******************************************************************************/
#include <stdlib.h>             /* Standard Lib    C89   */
#include <stdio.h>              /* I/O lib         C89   */
#include <string.h>             /* Strings         C89   */
#include <unistd.h>             /* UNIX std stf    POSIX */
#include <errno.h>              /* error stf       POSIX */
#include <fcntl.h>              /* UNIX file ctrl  UNIX  */
#include <ctype.h>              /* Char classes    C89   */
#include <termios.h>
#include <strings.h>
#include <time.h>
#include <spawn.h>
#include <signal.h>
#include <sys/types.h>          /* UNIX types      POSIX */
#include <sys/mman.h>           /* mmap()          POSIX */
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>           /* UNIX stat       POSIX */
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/kernel.h>
#include "ioleeessim.h"
#include "thoriumcorp.h"

struct timespec	ts;							// Variable para el time stamp
char			who[40];					// Variable para indicar el nombre de la fucion
int 			fd,							// Descriptor de plc_mem
				shmFlags=0,					// Flags para apertura de la plc_mem
				ret,						// Variable para el retorno de otras funciones
				mem_err=0,					// Badera global para indicar si hubo error abriendo la memoria compartida
				create=0,
				publicar=0;
PLC_Map 		*plc_memory;				// Apuntador a la estructura de la memroia compartida

//********** Rutina principal escritura en los hregs **********/
int io_eschreg (unsigned int start, unsigned int count, short hregs[BUFFER_SIZE])
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (hregs == NULL)
	{
		// Error en los parametros.
		return -1;
	}
	else
	{
		if (( start < 0)  || (count <= 0) || ((start + count) > HREG_SIZE) || (count > BUFFER_SIZE))
		{
			// Error en indices.
			return -2;
		}
		else
		{
				for ( indice = start ; indice < (start + count) ; ++indice)
				{
					plc_memory->hregs[indice] = (hregs[ indice - start ]);
					ret = clock_gettime (CLOCK_REALTIME, &ts);
					plc_memory->hregs_ts[indice] = ts;
					plc_memory->hup_flag[indice] |= 0x01;
				}
				return 0;
		}
	}
}

// ******** Rutina principal escritura en los iregs ***********
int io_escireg (unsigned int start, unsigned int count, short iregs[BUFFER_SIZE])
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (iregs == NULL)
	{
		// Error en los parametros.
		return -1;
	}
	else
	{
		if (((start < 0 ) || (count <= 0)) || ((start + count) > (IREG_SIZE)) || (count > BUFFER_SIZE))
		{
			// Error en indices.
			return -2;
		}
		else
		{
				for ( indice = start ; indice < (start + count) ; ++indice)
				{
					plc_memory->iregs[indice] = iregs[ indice - start ];
					ret = clock_gettime (CLOCK_REALTIME, &ts);
					plc_memory->iregs_ts[indice] = ts;
					plc_memory->iup_flag[indice] |= 0x01;
				}
				return 0;
		}
	}
}

// ******** Rutina principal escritura en los coils **********
int io_esccoil (unsigned int start, unsigned int count, unsigned short valor)
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (((start < 0 ) || (count <= 0)) || ((start + count) > (COIL_SIZE)) || (count > BUFFER_SIZE))
	{
		// Error en indices.
		return -2;
	}
	else
	{
			for ( indice = start ; indice < (start + count) ; ++indice)
			{
				if (valor)
					SETBIT (plc_memory->coils[indice / 16], indice % 16);
				else
					RESETBIT (plc_memory->coils[indice / 16], indice % 16);
				ret = clock_gettime (CLOCK_REALTIME, &ts);
				plc_memory->coils_ts[indice] = ts;
				plc_memory->cup_flag[indice] |= 0x01;
			}
			return 0;
	}
}

// ******* Rutina principal escritura en los stats *******
int io_escstat (unsigned int start, unsigned int count, unsigned short valor)
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (((start < 0 ) || (count <= 0)) || ((start + count) > (STAT_SIZE)) || (count > BUFFER_SIZE))
	{
		// Error en indices.
		return -2;
	}
	else
	{
			for ( indice = start ; indice < (start + count) ; ++indice)
			{
				if (valor)
					SETBIT (plc_memory->stats[indice / 16], indice % 16);
				else
					RESETBIT (plc_memory->stats[indice / 16], indice % 16);
				ret = clock_gettime (CLOCK_REALTIME, &ts);
				plc_memory->stats_ts[indice] = ts;
				plc_memory->sup_flag[indice] |= 0x01;
			}
			return 0;
	}
}

// ******** Rutina principal escritura en los coils **********
int io_esccoils (unsigned int start, unsigned int count, unsigned short valor[BUFFER_SIZE])
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (((start < 0 ) || (count <= 0)) || ((start + count) > (COIL_SIZE)) || (count > BUFFER_SIZE))
	{
		// Error en indices.
		return -2;
	}
	else
	{
			for ( indice = start ; indice < (start + count) ; ++indice)
			{
				if (valor[indice - start])
					SETBIT (plc_memory->coils[indice / 16], indice % 16);
				else
					RESETBIT (plc_memory->coils[indice / 16], indice % 16);
				ret = clock_gettime (CLOCK_REALTIME, &ts);
				plc_memory->coils_ts[indice] = ts;
				plc_memory->cup_flag[indice] |= 0x01;
			}
			return 0;
	}
}

// ******* Rutina principal escritura en los stats *******
int io_escstats (unsigned int start, unsigned int count, unsigned short valor[BUFFER_SIZE])
{
	unsigned int indice;
	// Ciclo de escritura. Iterar hasta terminar escritura.

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (((start < 0 ) || (count <= 0)) || ((start + count) > (STAT_SIZE)) || (count > BUFFER_SIZE))
	{
		// Error en indices.
		return -2;
	}
	else
	{
			for ( indice = start ; indice < (start + count) ; ++indice)
			{
				if (valor[indice - start])
					SETBIT (plc_memory->stats[indice / 16], indice % 16);
				else
					RESETBIT (plc_memory->stats[indice / 16], indice % 16);
				ret = clock_gettime (CLOCK_REALTIME, &ts);
				plc_memory->stats_ts[indice] = ts;
				plc_memory->sup_flag[indice] |= 0x01;
			}
			return 0;
	}
}



// *******  Lectura en los holding  ********
int io_leehreg (unsigned int start, unsigned int count, short hregs_lec[BUFFER_SIZE] )
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > HREG_TOTAL) || (count > BUFFER_SIZE))
	{ // * Error en indices.
		return -2; 
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			hregs_lec[ indice - start] = (plc_memory->hregs[indice]);
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			hregs_lec[ indice ] = 0;
		}
		return 0;
	}
}

//********  Lectura Input Regustrers ***********/
int io_leeireg (unsigned int start, unsigned int count, short iregs_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > IREG_SIZE) || (count > BUFFER_SIZE))
	{ //  Error en indices. 
		return -2; 
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			iregs_lec[ indice - start ] = (plc_memory->iregs[indice]);
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			iregs_lec[ indice ] = 0;
		}
		return 0;
	}
}

// ********res Lectura de los Coils        ********* /
int io_leecoil(unsigned int start, unsigned int count, unsigned short coils_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > COIL_SIZE) || (count > BUFFER_SIZE))
	{// * Error en indices.
		return -2;
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			if (GETBIT(plc_memory->coils[indice/ 16], indice % 16))
				coils_lec[ indice - start  ] = 1;
			else
				coils_lec[ indice - start  ] = 0;
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			coils_lec[ indice ] = 0;
		}
		return 0;
	}
} // Fin de io_leecoil

// *******  Lectura en los stats ********
int io_leestat (unsigned int start, unsigned int count, unsigned short stats_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (( start < 0) || (count <= 0) || ((start + count) > STAT_SIZE) || (count > BUFFER_SIZE))
	{ // *Error en indices
		return -2;
	}
	else
	{
		for ( indice = start ; indice <= (start + count) ; indice++)
		{
			if (GETBIT(plc_memory->stats[indice/ 16], indice % 16))
				stats_lec[ indice - start ] = 1;
			else
				stats_lec[ indice - start ] = 0;
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			stats_lec[ indice ] = 0;
		}
		return 0;
	}
}// * Fin de io_leestat

// *******  Lectura en los holding con timestamp y status ********
int io_leehregDat (unsigned int start, unsigned int count, DATOD hregs_lec[BUFFER_SIZE] )
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > HREG_TOTAL) || (count > BUFFER_SIZE))
	{ // * Error en indices.
		return -2; 
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			hregs_lec[ indice - start].valor = plc_memory->hregs[indice];
			hregs_lec[ indice - start].timestamp = plc_memory->hregs_ts[indice];
			hregs_lec[ indice - start].flags = plc_memory->hup_flag[indice];
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			hregs_lec[ indice ].valor = 0;
			hregs_lec[ indice ].timestamp.tv_sec = 0;
			hregs_lec[ indice ].timestamp.tv_nsec = 0;
			hregs_lec[ indice ].flags = 0;
		}
		return 0;
	}
}

//********  Lectura Input Registers con timestamp y status ***********/
int io_leeiregDat (unsigned int start, unsigned int count, DATOD iregs_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > IREG_SIZE) || (count > BUFFER_SIZE))
	{ //  Error en indices. 
		return -2; 
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			iregs_lec[ indice - start ].valor = (plc_memory->iregs[indice]);
			iregs_lec[ indice - start ].timestamp = plc_memory->iregs_ts[indice];
			iregs_lec[ indice - start ].flags = plc_memory->iup_flag[indice];
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			iregs_lec[ indice ].valor = 0;
			iregs_lec[ indice ].timestamp.tv_sec = 0;
			iregs_lec[ indice ].timestamp.tv_nsec = 0;
			iregs_lec[ indice ].flags = 0;
		}
		return 0;
	}
}

// ******** Lectura de los Coils con timestamp y estado       ********* /
int io_leecoilDat (unsigned int start, unsigned int count, DATOD coils_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if ((start < 0)  || (count <= 0) || ((start + count) > COIL_SIZE) || (count > BUFFER_SIZE))
	{// * Error en indices.
		return -2;
	}
	else
	{
		for ( indice = start ; indice < (start + count) ; indice++)
		{
			if (GETBIT(plc_memory->coils[indice/ 16], indice % 16))
				coils_lec[ indice - start  ].valor = 1;
			else
				coils_lec[ indice - start  ].valor = 0;
			coils_lec[ indice - start  ].timestamp = plc_memory->coils_ts[indice];
			coils_lec[ indice - start  ].flags = plc_memory->cup_flag[indice];
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			coils_lec[ indice ].valor = 0;
			coils_lec[ indice ].timestamp.tv_sec = 0;
			coils_lec[ indice ].timestamp.tv_nsec = 0;
			coils_lec[ indice ].flags = 0;
		}
		return 0;
	}
} // Fin de io_leecoil

// *******  Lectura en los stats con timestamp y status********
int io_leestatDat (unsigned int start, unsigned int count, DATOD stats_lec[BUFFER_SIZE])
{
	unsigned int indice;

	if (mem_err !=0)
	{
		// No se puede acceder a la memoria virtual
		return -3;
	}
	if (( start < 0) || (count <= 0) || ((start + count) > STAT_SIZE) || (count > BUFFER_SIZE))
	{ // *Error en indices
		return -2;
	}
	else
	{
		for ( indice = start ; indice <= (start + count) ; indice++)
		{
			if (GETBIT(plc_memory->stats[indice/ 16], indice % 16))
				stats_lec[ indice - start ].valor = 1;
			else
				stats_lec[ indice - start  ].valor = 0;
			stats_lec[ indice - start  ].timestamp = plc_memory->stats_ts[indice];
			stats_lec[ indice - start  ].flags = plc_memory->sup_flag[indice];
		}
		for ( indice = count ; indice < BUFFER_SIZE ; indice++)
		{
			stats_lec[ indice ].valor = 0;
			stats_lec[ indice ].timestamp.tv_sec = 0;
			stats_lec[ indice ].timestamp.tv_nsec = 0;
			stats_lec[ indice ].flags = 0;
		}
		return 0;
	}
}// * Fin de io_leestat

/*** Apertura Zona de Memoria ***/
int Aper_zm(void )
{
	/* 
	 * Esta funcion debe ser llamada antes de realizar el uso de la memoria compartida,
	 * antes de leer o escribir valores
	 */

	sprintf (who, "Aper_zm");
	/*********************************************************************
	 * Apertura/Creacion de la memoria compartida para la tabla modbus   *
	 *********************************************************************/
	if (create)
	{
		shmFlags = IPC_CREAT | SHM_PERM;
	}
	else
	{
		shmFlags = SHM_PERM;
	}

	if ((fd = shmget(SHM_KEY, sizeof(PLC_Map), 0666 )) >= 0)
	{
		if (publicar)
			fprintf(stdout,"%s: El mapa de memoria existe y será usado por esta aplicacion..\n", who);
	}
	else
	{
		if ((fd = shmget(SHM_KEY, sizeof(PLC_Map), IPC_CREAT | IPC_EXCL | 0666 )) < 0)
		{
			if (publicar)
				fprintf(stdout, "%s: ERROR en la creacion de la memoria compartida. Terminando...\n", who);
			switch(errno)
			{
				case EACCES:	fprintf(stdout, "%s: shm_open failed: The requested permissions were denied or create denied .\n", who);
								exit(EXIT_FAILURE);
								break;
				case EEXIST:	if (publicar)
									fprintf(stdout, "%s: shm_open failed: Object already exists and O_CREAT and O_EXCL were specified.\n", who);
								if (publicar)
									fprintf(stdout, "%s: Segment existed, attempting to open it..\n", who);
								if ((fd = shmget(SHM_KEY, sizeof(PLC_Map), IPC_EXCL | O_RDWR | 0666 )) >= 0)
								if(fd < 0)
								{
									fprintf(stdout, "%s: shm_open failed: Open (without O_CREAT failed too!), errno: %d\n", who, errno);
									exit(EXIT_FAILURE);
								} /* end if */
								break;
				case EINTR:		fprintf(stdout, "%s: shm_open failed: The operation was interrupted by a signal.\n", who);
								exit(EXIT_FAILURE);
								break;
				case EINVAL:	fprintf(stdout, "%s: shm_open failed: The operation is not supported.\n", who);
								exit(EXIT_FAILURE);
								break;
				case EMFILE:	fprintf(stdout, "%s: shm_open failed: Too many file descriptors were already open.\n", who);
								exit(EXIT_FAILURE);
								break;
				case ENAMETOOLONG:	fprintf(stdout, "%s: shm_open failed: Name was too long (longer than SHM_NAME_MAX chars).\n", who);
								exit(EXIT_FAILURE);
								break;
				case ENFILE:	fprintf(stdout, "%s: shm_open failed: The system file table is full.\n", who);
								exit(EXIT_FAILURE);
								break;
				case ENOENT:	fprintf(stdout, "%s: shm_open failed: Object doesn't exist (O_CREAT was not specified).\n", who);
								exit(EXIT_FAILURE);
								break;
				case ENOMEM:	fprintf(stdout, "%s: shm_open failed: A shared memory identifier and associated shared memory segment shall be created, but the amount of available physical memory is not sufficient to fill the request.\n", who);
								exit(EXIT_FAILURE);
								break;
				case ENOSPC:	fprintf(stdout, "%s: shm_open failed: No memory to create object (O_CREAT was specified).\n", who);
								exit(EXIT_FAILURE);
								break;
				default:		fprintf(stdout, "%s: shm_open failed: Duno why...\n", who);
								exit(EXIT_FAILURE);
								break;
			} /* switch */
			mem_err = 1;
		}
		else
		{
			if (publicar)
				fprintf(stdout, "%s:...Memoria compartida creada satisfactoriamente!\n", who);
		}
	}
	/*
	 * Uniendo la region de la memoria del PLC o tabla modbus al proceso.
	 */
	if ((plc_memory = (PLC_Map *) shmat(fd, NULL, 0)) == (void *) - 1)
	{
		if (publicar)
			fprintf(stdout, "%s: ERROR: No se puede unir la region de memoria del PLC a el espacio de memoria del proceso. Terminando...\n", who);
		if(plc_memory == MAP_FAILED)
		{
			switch(errno)
			{
				case EACCES:	fprintf(stdout, "%s: mmap failed: The FD was not open for read, or for write with (PROT_WRITE or MAP_SHARED)\n", who);
								break;
				case EAGAIN:	fprintf(stdout, "%s: mmap failed: The mapping could not be locked in memory\n", who);
								break;
				case EBADF:		fprintf(stdout, "%s: mmap failed: The FD not a valid open file descriptor.\n", who);
								break;
				case EINVAL:	fprintf(stdout, "%s: mmap failed: The value of len is zero, addr is not valid, bad combination of args\n", who);
								break;
				case EMFILE:	fprintf(stdout, "%s: mmap failed: The too many regions mapped already\n", who);
								break;
				case ENODEV:	fprintf(stdout, "%s: mmap failed: The FD file type is not supported by mmap().\n", who);
								break;
				case ENOMEM:	fprintf(stdout, "%s: mmap failed: Not enough memory\n", who);
								break;
				case ENOTSUP:	fprintf(stdout, "%s: mmap failed: Options not supported on this platform\n", who);
								break;
				case ENXIO:		fprintf(stdout, "%s: mmap failed: Range [off,off+len) are invalid for the FD, MAP_FIXED & invalid addresses, or FD not accessible\n", who);
								break;
				case EOVERFLOW:	fprintf(stdout, "%s: mmap failed: File is too big!\n", who);
								break;
				default:		fprintf(stdout, "%s: mmap failed: Duno why! (errno: %d)\n", who, errno);
								break;
			} /* switch */
		}
		mem_err = 1;
		return -1;
		// exit(EXIT_FAILURE);
	}
	else
	{
		if (publicar)
			fprintf(stdout, "%s: ...Region de memoria del PLC unida satisfactoriamente!\n", who);
	}
	return 0;
}

void Desv_zm(void)
{
	sprintf (who, "Desv_zm");
	if (shmdt(plc_memory) == -1)
	{
		fprintf(stdout,"%s:ERROR: No se puede desvincular la region de memoria. Terminando...\n", who);
		switch(errno)
		{
			case EACCES:	fprintf(stdout,"%s: shm_unlink failed: Permission is denied to be remove the object.\n", who);
							break;
			case ENAMETOOLONG:	fprintf(stdout, "%s: shm_unlink failed: name exceeded SHM_NAME_MAX characters.\n", who);
							break;
			case ENOENT:	fprintf(stdout, "%s: shm_unlink failed: The named object does not exist.\n", who);
							break;
			case EINVAL:	fprintf(stdout,"%s: munmap failed: The address range [addr,addr+len) is invalid.\n"
							"               munmap failed: The len argument is 0.\n"
							"               munmap failed: The addr argument is not a multiple of page size.\n", who);
							break;
			default:		fprintf(stdout, "%s: shm_unlink failed: Duno why (%d)...\n", who, errno);
							break;
		}
	}
}
