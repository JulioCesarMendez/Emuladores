/*
 * thoriumcorp.h para la version de Thorium 1.00 del 29/06/2021
 * Archivo de definiciones macro y prototipos de funciones y procedimientos
 * Throium v 1.00 (c) 2021 Thorium Corp
 * Author(s): Julio Cesar Mendez  <mendezjcm@tutanota.com>, <mendezjcx@gmail.com>
 * Fecha ultima modificacion: 29/06/2021
 */
#ifndef _THORIUM_H
#define _THORIUM_H

char VERSION[] = "Thorium (r), BUILD 20210629 >Viva Hugo Chavez Carajo !!";


#include <time.h>
#include <math.h>
/*
 * Definicion general de macros globales
 */
#define TABLEMOD_SIZE	2048			  // Valor para las aplicaciones de simulacion
#define DIMOF(x)        (sizeof(x)/sizeof(x[0]))  /* Array Dimension        */
#define INC(var,lim)    (++(var),(var)%=(lim))    /* Increment with limit   */
#define MAX(a,b)        ((a)>(b)?(a):(b))         /* Maximum of 2 values    */
#define MIX(a,b)        ((a)<(b)?(a):(b))         /* Minimum of 2 values    */
#define NCERO(a)	((a)== 0 ? 1:(a))	  /* Devuelve a si Diferente a cero, 1 si igual a cero */
#define NNCERO(a)	((a)== 0 ? (a):1)	  /* Devuelve 1 si Diferente a cero, a si igual a cero */
#define POWERb(a,b)	((a)^(b))		  /* Potencia de a a la b	*/
#define MAKEWORD(hi,lo) (((hi)<<8)|(lo))          /* Make a word            */
#define MAKELONG(hi,lo) (((hi)<<16)|(lo))         /* Make a long from 2 word*/
#define LONGTOWORDL(a)  ((a)>>8 )
#define LONGTOWORDH(b)	((b)>>16)
#define LOBYTE(w)       ((w) & 0xFF)              /* Get  low byte of w     */
#define HIBYTE(w)       LOBYTE((unsigned int)(w)>>8)    /* Get high byte of w     */
#define CEIL(a,b)       ((a)/(b)+((a)%(b)!=0))    /* Minimum no < than a/b  */
#define GETBIT(b,i)     ((b)&(1<<(i)))            /* Get i-th bit of (b)    */
#define SETBIT(b,i)     ((b)|=(1<<(i)))           /* Set i-th bit of (b)    */
#define RESETBIT(b,i)   ((b)&=~(1<<(i)))          /* Re-set i-th bit of (b) */
#define INDICECONVER(a)	(((a)-13)>0?((a)-13) : 0 ) /* Indice no menor de cero para arreglo de base de tiempo de espera*/
                                                  /* Get serial port number */
#define GET_PORT_NUMBER(s) (atoi(strstr((const char *)(s),"/ttyS")+5)) /* Para SO Linux */
#define BUFFER_SIZE      1024
#define POLYCRC16        0xA001                   /* CRC-16 Modbus Polyn    */
#define GP   		 0xA001			  //generating polynomial
#define FALSE		 0
#define TRUE             1                        /* True value != 0        */
#define STAT_SIZE        TABLEMOD_SIZE                    /* Status registers       */
#define COIL_SIZE        TABLEMOD_SIZE                    /* Coil registers         */
#define IREG_SIZE        TABLEMOD_SIZE                    /* Input registers        */
#define HREG_SIZE        TABLEMOD_SIZE                    /* 64000 Holding registers including statistics + 100 REGISTROS DE CONTROL PARA THORIUM */
#define COIL_MSIZ        (COIL_SIZE/16)           /* Actual size of Coils  (1024) */
#define STAT_MSIZ        (STAT_SIZE/16)           /* Actual size of Status (1024) */
#define LIMITE_SUP	 HREG_SIZE - 1		  // Limite superior para arreglos de registros
#define OnCoil           0xFF00u
#define OffCoil          0x0000
#define NBYTES           6              	/* Counter position within the query       */
#define NumBytePreamble  5    			// Numero mÃ¡ximo de bytes FF de preambulo en la trama del mensaje por defecto
#define DATA             7              	/* Data position within the query          */
#define FUNCTION         1              	/* Modbus function position within queries */
#define CALIDAD_MALA	 0x00			// Bits 6 y 7 en cero
#define CALIDAD_INCIERTA 0x40			// Bit 7 en cero, bit 6 en uno
#define CALIDAD_BUENA	 0xC0			// Bit 6 y 7 en uno
#define NUMBER_PORTS     42                        /* Total number of ports:36 RS232, 2  USB */
#define MAX_MASTER_TASKS 15                        /* Number of master tasks */
#define MAX_SLAVE_TASKS	 (NUMBER_PORTS-MAX_MASTER_TASKS) /* Number of slave tasks  */
#define REG_CONTROL_THORIUM 100			/* 100 REGISTROS DE CONTROL PARA THORIUM */
#define REG_STDCS	 ( 12 * NUMBER_PORTS )
#define HREG_EXTRA	 ( REG_STDCS + NUMBER_PORTS)	/* Doce registro de estado para cada puerto mas un registro  */
/* 546 */																/* de identificacion de protocolo y estado actual del mismo  */
#define HREG_TOTAL	 ( HREG_SIZE + HREG_EXTRA )					/* Hreg extras para las estadisticas de los puertos */
/*  */
#define IREG_TOTAL	 ( IREG_SIZE + HREG_EXTRA )					/* Iregs extras para los estatus de la THORIUM*/
/*  */

#define MUESTRA_STATUS	 HREG_TOTAL +1	/* Si start = Muestra_status se refiere al byte de estado de comunicacion y el ID de la RTU */
/*  */									/* se colocan en la direccion definida en el archivo de configuracion de la Net-DAS  */
											/* por el usuario.                                        */
#define MUESTRA_RTU_F	 HREG_TOTAL +2	/* Si start = Muestra_rtu_f se refiere a los datos de  */
/*  */									/* la RTU, definidos en el archivo de configuracion de */
											/* La Net-DAS                                          */
#define MUESTRA_RTU_CNF	 HREG_TOTAL +3	/* Si start = Muestra_rtu_cnf se refiere a los datos de  */
/*   */									/* configuracion de cada RTU, definidos en el archivo de configuracion de */
											/* La Net-DAS                                          */
#define MUESTRA_RTU_PID	 HREG_TOTAL +4	/* Si start = Muestra_rtu_pid se refiere a los datos de  */
/*   */									/* configuracion de cada pid, definidos en el archivo de configuracion de */
											/* La Net-DAS                                          */
#define MUESTRA_BIT_CONTROL	HREG_TOTAL +5	/* Si start = MUESTRA_BIT_CONTROL se refiere al valor del estado de comunicacion de la RTU  */
/*   */									/* y se coloca en la direccion definida en el archivo de configuracion de */

#define MUESTRA_PORCIENTO_COMM	HREG_TOTAL +6  /* Si start = MUESTRA_PORCIENTO_COMM se requiere montar el porcentaje de comunicacion de la RTU */
/*   */	

#define REBOOT_SISTEMA	 COIL_SIZE -2 		/* Si COIL = COIL_SIZE -2 se requiere reiniciar todo el sistema */
/*   */									/* */

#define LOGIN_REMOTO	 COIL_SIZE -3	/* Si COIL = COIL_SIZE -3 se requiere login remoto (NO IMPLEMENTADO AUN) */
/*   */									/* */

#define RESET_ESTADISTICA	COIL_SIZE -4	/* Si COIL = COIL_SIZE -4 se requiere reiniciar las estadisticas de comunicacion */
/*   */									/* */

#define REINICIA_SERVICIOS	COIL_SIZE -5	/* Si COIL = COIL_SIZE -5 se requiere reiniciar los servicios de Net-DAS (protocolos) */
/*   */									/* */


#define FUTEX_OWNER_DIED        	0x40000000		/* Para control (tolerancia) de las violaciones de memoria en el momento del Mutex */
#define REINICIA_SCANING			HREG_SIZE -1			/* 1er REGISTRO DE CONTROL DE THORIUM-> REINICIAR EL SCAN DE TODAS LAS REMOTAS QUE ESTAN FUERA DE COMUNICACION EN EL SIGUIENTE SICLO */
#define FORZAR_SCANING_RTU			HREG_SIZE -2			/* 2do REGISTRO DE CONTROL DE THORIUM-> NUMERO O ID DE REMOTA QUE SE REQUIERE REINICIAR LA ENCUESTA TOTAL */
#define RETORNO_FORZAR_RTU			HREG_SIZE -3			/* 3er REGISTRO DE CONTROL DE THORIUM-> RETORNO DEL VALOR DE CONTROL RECIBIDO EN 2do REGISTRO, PARA CONTROL DEL SCADA */
#define CARGAR_CARTAS_DYNA			HREG_SIZE -4			/* 4to REGISTRO DE CONTROL DE THORIUM-> CARGAR LA CARTA DINAGRAFICA CON EL ID COLOCADO EN ESTA POSICION DE LA MEMORIA MODBUS */
#define RETORNO_CARGAR_DYN			HREG_SIZE -5			/* 5to REGISTRO DE CONTROL DE THORIUM-> RETORNO DEL VALOR DE CONTROL RECIBIDO EN 4do REGISTRO, PARA CONTROL DEL SCADA */
#define SACAR_RTU_SCANING			HREG_SIZE -6			/* 6to REGISTRO DE CONTROL DE THORIUM-> SACAR VIRTUALMENTE LA RTU DE SCANING PERMANENTEMENTE. */
#define RETORNO_SACAR_RTU_SCANING	HREG_SIZE -7			/* 7to REGISTRO DE CONTROL DE THORIUM-> RETORNO DEL VALOR DE CONTROL RECIBIDO EN 6to REGISTRO. */
#define METER_RTU_SCANING			HREG_SIZE -8			/* 8to REGISTRO DE CONTROL DE THORIUM-> METER VIRTUALMENTE LA RTU A SCANING PERMANENTEMENTE. */
#define RETORNO_METER_RTU_SCANING	HREG_SIZE -9			/* 9no REGISTRO DE CONTROL DE THORIUM-> RETORNO DEL VALOR DE CONTROL RECIBIDO EN 8vo REGISTRO. */
#define MONTAR_EST_COMM_RTU			HREG_SIZE -10			/* 10mo REGISTRO DE CONTROL DE THORIUM-> MONTAR LA ESTADISTICA DE COMUNICACION DE UNA REMOTA */
#define RETORNO_MONTA_EST_COMM_RTU	HREG_SIZE -11			/* 11vo REGISTRO DE CONTROL DE THORIUM-> RETORNO DEL VALOR DE CONTROL RECIBIDO EN 10mo REGISTRO. */
#define BLOQUEAR_COMANDOS			HREG_SIZE -12			/* 12vo REGISTRO DE CONTROL DE THORIUM-> CAMPO PARA BLOQUEAR EL ENVIO DE COMANDO VIA SCADA POR CUALQUIER ESCLAVO. */
															/* El valor del registro BLOQUEAR_COMANDOS determina el nivel de bloqueo de los comandos: */
															/*  0X00 = ELIMINA TODOS LOS BLOQUEOS */
															/*  0X01 = Bloquear comandos via Modbus RTU */
															/*  0X02 = Bloquear comandos via Modbus TCP */
															/* 	0x03 = Bloqeuar comandos via Modbus TCP/RTU */
															/* 	0x04 = Bloquear comandos via DNP */
															/* 	0x05 = Bloquear comandos via DNP/Modbus RTU */
															/* 	0x06 = Bloquear comandos via DNP/Modbus TCP */
															/* 	0x07 = Bloquear comandos via DNP/Modbus TCP/RTU */
															/* 	0x08 = Bloquear comandos via Gateway */
															/* 	0x09 = Bloquear comandos via Gateway/Modbus RTU */
															/* 	0x0A = Bloquear comandos via Gateway/Modbus TCP */
															/* 	0x0B = Bloquear comandos via Gateway/DNP */
															/* 	0x0C = Bloquear comandos via Gateway/DNP/Modbus RTU */
															/* 	0x0D = Bloquear comandos via Gateway/DNP/Modbus TCP */
															/* 	0x0E = Bloquear comandos via Gateway/DNP/Modbus TCP/RTU */
															/* 	0x0F = Bloquear TODOS LOS COMANDOS */
#define ELIMINA_BLOQUEO_COMANDOS 	0x00					/*  0X00 = ELIMINA TODOS LOS BLOQUEOS */
#define BLOQUEA_COMANDOS_MODBUSRTU 	0x01					/*  0X01 = Bloquear comandos via Modbus RTU */
#define BLOQUEA_COMANDOS_MODBUSTCP 	0x02					/*  0X02 = Bloquear comandos via Modbus TCP */
#define BLOQUEA_COMANDOS_DNP		0X04					/* 	0x04 = Bloquear comandos via DNP */
#define BLOQUEA_COMANDOS_GATEWAY	0X08					/* 	0x08 = Bloquear comandos via Gateway */
#define BLOQUEA_COMANDOS_TODOS		0X0F					/* 	0x0F = Bloquear TODOS LOS COMANDOS */
#define GURDAR_CONFIGURACION_ACTUAL HREG_SIZE -13			/* 13ER REGISTRO DE CONTROL DE THORIUM-> CAMPO PARA GUARDAR LA CONFIGURACION ACTUAL EN MEMORIA HACIA EL ARCHIVO MASTERS.TXT */

#define YesNo(a)					((a)>0?"YES":"NO")

/* Manejo de memoria compartida */
#define SHM_KEY		(key_t) 6091967	/* Llave o nombre de la memoria compartida */
#define SHM_KEY2	(key_t) 7691906	/* Llave o nombre de la memoria compartida */
#define SHM_SIZE	(size_t) sizeof(PLC_Map)	/* tamaÃ±o de la memoria compartida  */
#define SHM_PERM	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) /* Permiso para que todos los protocolos puedan acceder para lectura y escritura de la memoria */

/* Manejo de errores que causan la terminacion de la aplicacion */
#define errExit(msg)	do { perror(msg); exit(EXIT_FAILURE); } while (0)


/* Manejo de señalizacion de timer de ciclo del PLC */
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

/* mutex para sincronizar el acceso al buffer del Gateway Gateway y la memoria modbus */
pthread_mutex_t mutexBufferElam; 

//typedef struct shmid_ds ShMemSeg;

/*
 * PLC Memory Map Table structure (follows a MODBUS definition)
 */
typedef struct _plc_map_table {
   unsigned short coils[COIL_MSIZ];               /* Coils                  */
   unsigned short stats[STAT_MSIZ];               /* Input status           */
   short iregs[IREG_TOTAL];                        /* Input registers        */
   short hregs[HREG_TOTAL];                       /* Holding registers      */
   struct timespec coils_ts[COIL_SIZE];           /* COIL's Time stamp      */
   struct timespec stats_ts[STAT_SIZE];           /* STAT's Time stamp      */
   struct timespec iregs_ts[IREG_TOTAL];           /* IREG's Time stamp      */
   struct timespec hregs_ts[HREG_TOTAL];           /* HREG's Time stamp      */
   unsigned char  cup_flag[COIL_SIZE];              /* COIL's update/control/cuality flags, bits 1= 1 (0x02) Valor modificado; 6y7=0 (0x00) Calidad Mala; 1 (0x40) Calidad Incierta; 2 (0xC0) Calidad Buena; */
   unsigned char  sup_flag[STAT_SIZE];           /* STAT's update/control/cuality flags */
   unsigned char  iup_flag[IREG_TOTAL];             /* IREG's update/control/cuality flags */
   unsigned char  hup_flag[HREG_TOTAL];           /* HREG's update/control/cuality flags */
} PLC_Map;
/*
 * Estructura para guardar el timestamp de la actualizacion de estados de comunicacion con RTUs y estados
 * y banderas devueltas por las RTUs en las tramas y en los campos de las tramas de comunicacion
*/
typedef struct _timestamp_comm {
    /* Dia del a¤o [0 - 365]  || hora del dia [0 - 23] en Hex. Ejemp. 365 y 23=8EAB */
    unsigned short dayofyearhour;
    /* Similar al anterior minuto y segundo del dia en Hex. Ejemp. 59 y 59 = 1747 */
    unsigned short minutesecound;
} TIME_Comm_Stat;
/*
 * States defined to process configuration files (master & slave)
 */
enum {
    BOCD = 1,
    BOPD ,
	INSTANCE,
    CANAL,
    PORTDEF,
    BOSD,
    RTUDEF,
    ELEDEF,
    KOM,
 	EOSD,
	EOPD,
    EOCD};

enum {
    stat_crcerr = 0,
    stat_queries,
    stat_invalid,
    stat_noresp,
    stat_timeout,
    stat_valid,
    stat_retries,
	stat_invalid_d_a,
	stat_porcent_comm };


/*
 * Serial port structure
 */
typedef struct _serial_port {
    char                port[40];                 /* Device port name        */
	unsigned short 		puerto;
    char                protocol[80];             /* Protocol name           */
    unsigned short      address;
    unsigned long       baud;                     /* Baud rate               */
    unsigned short      numbits;                  /* number of bits 5,7,8    */
    unsigned short      stopbits;                 /* Stop bits               */
    char                parity;                   /* Parity: e, o, n         */
    unsigned short      rts;                      /* Request to send         */
    unsigned short      dtr;                      /* Data terminal ready     */
    unsigned short      pre;                      /* Pre transmission time   */
    unsigned short      post;                     /* Post transmission time  */
    unsigned char       funcion;                /* Funcion del puerto: 0 Maestro, 1 Esclavo, 2 escucha, 3: Maestro/Esclavo. */
    struct _rtus        *rtus;                    /* Pointer to RTUS list    */
    struct _serial_port *next;                    /* Pointer to next port    */
} PORTS;

extern unsigned short	rtuelectrico;	/* Version de NET-DAS para el sector electrico	*/
extern unsigned short	verbose;		/* Bandera de nivel de publicacion				*/
extern unsigned short	debug;			/* Bandera de opcion de depuracion				*/
extern PLC_Map			*plc_memory;	/* Apuntador para el mapa de memoria del PLC	*/
extern PORTS			*puertos;		/* Apuntador a la memoria con los datos de las RTUs de todos los protocolos */

typedef struct _ecuacion_recta {
	float	Y0;
	float	Y1;
	float	C0;
	float	X0;
	float	X1;
	float	m;
} EcRecta;

/*
 * Data element structure
 */
typedef struct _data_ele {
	unsigned long		start;			// Inicio de bloque de registros
	unsigned long		amount;			// Cantidad de registros
	unsigned long		destination;		// Destino en Tabla Modbus
	unsigned long		forzar;			// Control de cambio del registro Modbus
	unsigned long		poll_interval;	// Cantidad de ciclos a esperar por encuesta
	unsigned long		poll_count;		// Para control de ciclos (Interno)
	unsigned short		plural;			// Lo usa el protocolo modbus, para armar consultas de varios registros=1 (func 15), para preguntar por un solo registro=0 (func 5)
	unsigned char		OtrosPropositos;	// Bandera generica para propositos generales
	struct EcRecta		*Recta;			// Ecuacion de la recta para conversión de cuentas a Unidades de Ingenieria 
	struct _data_ele	*next;			// Siguiente grupo de registro
} DATA_ele;

typedef struct _rtus {
    unsigned long     address;
    unsigned int      retry_count;
    unsigned int      timeout;
	char			  rtu_name[80];
    char              IPaddress[80];
    char              online[4];
    char              enlinea;
    int               socket_fd;
	unsigned short	  seqno;			// Secuencia de tramas
    unsigned long     stat_noresp;
    unsigned long     stat_retries;
    unsigned long     stat_queries;
    unsigned long     stat_timeout;
    unsigned long     stat_crcerr;
    unsigned long     stat_invalid;
    unsigned long     stat_valid;
    unsigned long     hart_flags;        // Bit de estado del instrumento, obtenidos con comando 0x00
    unsigned long     hart_status;       // Valor asociado a la comm interna del instrumento
    unsigned long     hart_comm_flags1;  // Primer byte de estado de comm entre Maestro y esclavo
    unsigned long     hart_comm_flags2;  // Segundo byte de estado de comm entre Maestro y esclavo
    struct timespec   stat_noresp_ts;
    struct timespec   stat_retries_ts;
    struct timespec   stat_queries_ts;
    struct timespec   stat_timeout_ts;
    struct timespec   stat_crcerr_ts;
    struct timespec   stat_invalid_ts;
    struct timespec   stat_valid_ts;
    unsigned long     hart_dt;
    unsigned long     hart_di;
    unsigned short    nPreambles;        // Numero de preambulos para la comunicaciÃ³n con la RTU
    TIME_Comm_Stat    hart_f_s_c_ts;     // Timestamp de la ultima actualizacion de los flags y estatus hart 
    DATA_ele          *coils;
    DATA_ele          *stats;
    DATA_ele          *iregs;
    DATA_ele          *hregs;
    struct _rtus      *next;
} RTUS;

enum {
    Coils = 1,
    Stats,
    Hregs,
    Iregs };

enum {
    /* Functions: */
    ReadCoils         = 1,
    ReadInputs        = 2,
    ReadHoldingRegs   = 3,
    ReadInputRegs     = 4,
    ForceCoil         = 5,
    PresetHoldingReg  = 6,
    LoopbackTest      = 8,
    ForceCoils        = 15,
    PresetHoldingRegs = 16,
    ExceptionReport   = 0x80,
    IllegalFunction   = 0x81,
    IllegalDataAddress= 0x82,
    IllegalDataValue  = 0x83,
    Failure			  = 0x84,
    Acknowledge		  = 0x85,
    Busy			  = 0x86,
    NAK				  = 0x87,
    MemoryError		  = 0x88,
	IllegalNumMulRedB = 0x92,
	MultipleReadBlockCRCError = 0x93,
	InconsAddrsMultReadBlock  = 0x94,
	IllegalByteCount		  = 0x96};

enum {
    GetSlaveAddr = 1,
    GetFunction,	//2
	Get1stDirDest,	//3
	Get2stDirDest,	//4
	GetNRegisters1,	//5
	GetNRegisters2,	//6
	GetCounts,		//7
    GetBytes,		//8
    GetNbytes,		//9
    Get1stCRC,		//10
    Get2ndCRC };	//11

char 	nombrePuertos[42][16]=
{"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5","/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9",
 "/dev/ttyS10","/dev/ttyS11","/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyS16","/dev/ttyS17","/dev/ttyS18","/dev/ttyS19",
 "/dev/ttyS20","/dev/ttyS21","/dev/ttyS22","/dev/ttyS23","/dev/ttyS24","/dev/ttyS25","/dev/ttyS26","/dev/ttyS27","/dev/ttyS28","/dev/ttyS29",
 "/dev/ttyS30","/dev/ttyS31","/dev/ttyS32","/dev/ttyS33","/dev/ttyS34","/dev/ttyS35","/dev/ttySA0","/dev/ttySA1","/dev/ttySA2","/dev/ttySA3",
 "/dev/usb1","/dev/usb2"};

/*
 * FUnciones de apoyo para Linux RT
 */
void delayth (int tipo, long par1 )
{
	// tipo =potencia a la que se llevan los par1 nanosegundos...; t
	struct timespec ts, tsr;
	
	ts.tv_sec = 0; 
	ts.tv_nsec = par1 * pow(10, MAX(0, MIX(tipo, 13))); // Conversion de multiplo de milisegundo indicado
	// ts.tv_nsec = par1 * POWERb(10, MAX(0, MIX(tipo, 13))); // Conversion de multiplo de milisegundo indicado
	nanosleep (&ts, &tsr);
}

// delays - sleep para un numero especifico de milisegundos
void delaysth(long msec)            // Tiempo en milisegundos
{
	unsigned int seconds      =  (unsigned int) msec / 1000;	// Conversion a segundos
	unsigned int milliseconds = (unsigned int) msec % 1000;	// Conversion a milisegundos
	struct timespec ts;			// Estructura de datos para nanosleeep()
	ts.tv_sec  = seconds;
	ts.tv_nsec = milliseconds * 1000000; // i.e.: tv_nsec=500000000; 500 millones de ns es 1/2 sec
	nanosleep(&ts, NULL);
}

/* **********************************************************
 * Calc_CRC: rutina para el calculo del CRC de una cadena	*
 ************************************************************/
unsigned short  Calc_CRC(char *nData, unsigned int size, unsigned short CRC16M)
{
	unsigned char carry;
	int i,j;
	
	for (j = 0; j < size; j++)
	{
		CRC16M ^= nData[j] & 0xFF;
		for (i=0; i<8; i++)
		{
		  carry = CRC16M & 0x0001;
		  CRC16M >>=1;
		  if (carry) CRC16M ^= GP;
		}
	}
	return CRC16M;
}

int DameNumPuerto(char *msg)
{ int i; for (i=0;i < 42;i++){ if (! strcmp (nombrePuertos[i], msg)) break;}; return MAX(0,i); }


float Anidacion(long RegA, long Oper01,
				long RegB, long Oper02,
				long RegD, long Oper03,
				long RegF )
{
	float valor;
	int i;
	long Reg, Oper;

	valor = RegA;
	for (i=1;i<4;i++)
	{
		if (i=1)
		{
			Reg = RegB;
			Oper = Oper01;
		}
		if (i=2)
		{
			Reg = RegD;
			Oper = Oper02;
		}
		if (i=3)
		{
			Reg = RegF;
			Oper = Oper03;
		}
		switch (Oper)
		{
			case 0: valor = valor + Reg;
					break;
			case 1: valor = valor - Reg;
					break;
			case 2: valor = valor * Reg;
					break;
			case 3: valor = valor / Reg;
					break;
			case 4: valor = pow ((double) valor, (double) Reg);
					break;
			case 5: valor = valor && Reg;
					break;
			case 6: valor = valor || Reg;
					break;
			// case 7: valor = valor ^^ Reg;
			// 		break;
			default:
					break;
		}
	}
	return valor;
}

#endif                          /* _THORIUM_H */
