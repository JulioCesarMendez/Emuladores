/**********************************************************
 * 	GREEN HOUSE T H O R I U M   S I M U L A T O R
 * Generic io write/read modbus memory functions
 * Thorium (c) 2021, Thorium Corp
 * Author(s): Julio Cesar Mendez <mendezjcx@tutanota.com>
 **********************************************************/

#include <stdlib.h>             /* Standard Lib    C89   */
#include <stdio.h>              /* I/O lib         C89   */
#include <string.h>             /* Strings         C89   */
#include <unistd.h>             /* UNIX std stf    POSIX */
#include <errno.h>              /* error stf       POSIX */
#include <fcntl.h>              /* UNIX file ctrl  UNIX  */
#include <ctype.h>              /* Char classes    C89   */
#include <time.h>
#include <sys/types.h>          /* UNIX types      POSIX */
#include <sys/mman.h>           /* mmap()          POSIX */
#include <sys/shm.h>
#include <sys/stat.h>           /* UNIX stat       POSIX */
#include <sys/time.h>
#include <curses.h>
#include <linux/kernel.h>
#include "../include/ioleeessim.h"
#include <termios.h>
#include <term.h>

#define MAXI(a,b)        ((a)>(b)?(a):(b))         /* Maximum of 2 values    */
#define MINI(a,b)        ((a)<(b)?(a):(b))         /* Minimum of 2 values    */
#define NCERO(a)	((a)== 0 ? 1:(a))	  /* Devuelve a si Diferente a cero, 1 si igual a cero */
#define NNCERO(a)	((a)== 0 ? (a):1)	  /* Devuelve 1 si Diferente a cero, a si igual a cero */

// #define SEC3(a)	    ((a)<35 ? (a)++:30)	  /* Devuelve de un numero secuencial de 30 a 34 */

#define BUFFER_SIZE      1024
#define MAX_TANQUES 10		// Maximo pueden ser 85 tanques

// Estructura para la interpretacion de los valores o registros modbus
typedef union modicon_fl
{
    float a;
    unsigned int b;
    unsigned short c[2];
    unsigned char d[4];
}realmap;

//  Estructura con los posibles formatos de valores provenientes de los registros modbus
typedef struct _ThoriumValue
{
	char    pvchar;    /* CHAR: PvCHAR        */
	int 	pvsint;    /* SINT: PvSINT        */
	long  	pvdint;    /* DINT: PvDINT      */
	float   pvreal;    /* REAL: PvREAL     */
}Thorioumpt;


realmap valores;
Thorioumpt data;

typedef struct _Tanque
{
	char nombre[20];
	float volumen; 			// Volumen total del tanque
	float volActual;
	float altura; 			// Altura del tanque
	char isStatAltoAlto;	 	// Sw nivel alto alto ?. Se asume instalado el 95% de la altura
	char isStatAlto;  		// Sw nivel alto ?. Se asume instalado el 90% de la altura
	char isStatBajo; 		// Sw nivel bajo ?. Se asume instalado el 10% de la altura
	char isStatBajoBajo;		// Sw nivel bajo bajo ?. Se asume instalado el 5% de la altura
        char isFT[3];                   // Existen transmisores de flujo ? Debe haber uno por cada bomba (NO VALVULA) de llenado
	char isLT;			// Existe transmisor de nivel ?
	float iniNivel; 		// Volumen inicial
	char isPhC[3];			// Sensores de pH ? 1, 2 ó 3. 	//char isPhC02; 	//char isPhC03;
	char isTE[3];			// Sensores de Temperatura ? 1, 2 ó 3. 	//char isTE02; 	//char isTE03;
	char isECC[3];			// Sensores de Conductividad ? 1, 2 ó 3.	//char isECC02; 	//char isECC03;
	char isPT[3];			// Sensores de Presión ? 1, 2 ó 3. 	//char isPT02; 	//char isPT03;
	char isVT[3];			// Sensores de Vibración ? 1, 2 ó 3. 	//char isVT02; 	//char isVT03;
	char isTUT[3];			// Sensores de Turbidez ? 1, 2 ó 3. 	//char isTUT02;	//char isTUT03;
	char isOT[3];			// Sensores de Oxigeno ? 1, 2 ó 3. 	//char isOT02;	//char isOT03;
        bool isPM_RUN;                  // Contacto NO, del contactor del arraque de la bomba para informar que la bomba esta en marcha.
        bool isPB_PM_START;             // Pulsador verde de 22 mm NO, utilizado para el arranque de la bomba desde el tablero (PM-01).  
        bool isPB_PM_STOP;              // Pulsador rojo  de 22 mm NO, utilizado para la parada de la bomba desde el tablero (PM-01).  
	char DescargaTipo;		// DREN VALV/BOMB: =0  DIGITAL (ON/OFF), =1  ANALOG (% OPENED). =-1 NONE. MY BE A PUMP (ON/OFF) OR ON/OFF VALV.
	float FlujoDescarga;		// DREN VALV/BOMB: =%   L/min
	float desEstad;	                // Estado actual de la valvula/bomba (ON=1/OFF=0, or %Apertura)
	unsigned short spanDesc;	// Direccion de la valvula/bomba (span en registro o IO)
	char isFillValv[3];		// FILL VALV/BOMB: =0  DIGITAL (ON/OFF), =1  ANALOG (% OPENED). =-1 NONE. MY BE A PUMP (ON/OFF) OR ON/OFF VALV.
	float FlujoLlenado[3];		// FILL VALV/BOMB:= %   L/min. Si el llenado es con bomba, esto representa el valor del transmisor de flujo
	float fillEstad[3];	        // Estado actual de la valvula
	char solType[3];		// Tipo de solucion: 0 acida, 1 alcalina, 2 neutra
	float phValue[3];		// Valor de pH de cada sensor
	float phProm;			// pH actual (salida)
	float eccValue[3];		// Valor de ecc de cada sensor
	float eccProm;			// ecc actual (salida)
	float teValue[3];		// Valor de temp de cada sensor
	float teProm;			// temp actual (salida)
	float ptValue[3];		// Valor de presion de cada sensor
	float ptProm;			// presion actual (salida)
	float vtValue[3];		// Valor de vibracion de cada sensor
	float vtProm;			// vibracion actual (salida)
	float tutValue[3];		// Valor de turbidez de cada sensor
	float tutProm;			// turbidez actual (salida)
	float otValue[3];		// Valor de oxigeno de cada sensor
	float otProm;			// concetracion de oxigeno actual (salida)
} Tanque;

int cantidad_tanques=0;
Tanque arreglo_tanques[MAX_TANQUES];
Tanque * tanques = 0, * tanque = 0;

typedef struct _LineaPantalla
{
	char cadena[150];
	int pos_x;
	int pos_y;
	unsigned char letra;			// Color de la letra
	unsigned char fondo;			// Color de fondo
	unsigned char atrib;			// Atributo del texto: normal, negrita, subrayallo, etc...
} LineaPantalla;

// Manejo pantalla
WINDOW *w;
int tecla=-1, i=0, x=0, yup=0, xyz=30;
int max_x, max_y, pos_x, pos_y;
bool fin = false, DEBUG=false;
int blinkking=-5;

LineaPantalla lineasPantalla[34];
LineaPantalla * lineasP=0;

// Manejo de archivos
char tag[15], valor[256], any[5], lineaTemporal[80];

// Manejo de puerto TCP *** no deberia usarse en este programa ***
char this_port[10]= "502";

// Manejo de memoria modbus
unsigned short iregTemporal[BUFFER_SIZE];	// Maximo 24 ireg por tanque: LT, pH01, pH02, pH03, pH04, TE01, TE02,  TE03, TE04, ECC01, ECC02, ECC03, ECC04, ...
unsigned short hregTemporal[BUFFER_SIZE];	// Maximo 4 hreg por tanque: Valv Descarga, Valv Llenado 01,  Valv Llenado 02,  Valv Llenado 03,  Valv Llenado 04
unsigned short statTemporal[BUFFER_SIZE];	// Maximo 7 stat por tanque: PM_RUN, PB_PM_START, PB_PM_STOP, HHLC, HLC, LLC, LLLC.
unsigned short coilTemporal[BUFFER_SIZE];	// Maximo 4 coil por tanque: Valv Descarga, Valv Llenado 01,  Valv Llenado 02,  Valv Llenado 03,  Valv Llenado 04

int spanStat[MAX_TANQUES+1];
int spanIreg[MAX_TANQUES+1];
int spanCoil[MAX_TANQUES+1];
int spanHreg[MAX_TANQUES+1];

float histTemp[720];		// Arreglo para las temperaturas por hora de un mes.
float volumen_entrante, volumen_saliente, valorPhAcid;

// Manejo de tiempos
struct timespec ts, tsa;
time_t timep;
struct tm *p;

long diffTiempo[MAX_TANQUES];

// DECLARACIONES DE FUNCIONES
// Funciones de escritura
extern int io_eschreg (unsigned int , unsigned int , short *);
extern int io_escireg (unsigned int , unsigned int , short *);
extern int io_esccoil (unsigned int , unsigned int , unsigned short );
extern int io_escstat (unsigned int  , unsigned int , unsigned short );
extern int io_esccoils (unsigned int , unsigned int , unsigned short *);
extern int io_escstats (unsigned int  , unsigned int , unsigned short *);

// Funciones de Lectura
extern int io_leeireg (unsigned int , unsigned int , short *);
extern int io_leehreg (unsigned int , unsigned int , short *);
extern int io_leecoil(unsigned int , unsigned int , unsigned short *);
extern int io_leestat (unsigned int , unsigned int , unsigned short *);

// Funciones de apertura/cierre de memoria compartida
extern int Aper_zm(void );
extern void Desv_zm(void);


// Funcion de inicio de la memoria
void iniMem(void );

// Funciones de manejo de interfaz
int interfaz(char , int);

//Funciones de manejo de archivos
void histTempFile (const char* );
void cargarTanques (const char* );

// Funciones de comunicacion
void servidormdb(void );

// Funciones de emulacion de patio de tanques
void simularTanque (int );

// CODIGO DE FUNCIONES
int SEC3(int a)
{
 if (xyz++>=35)
    xyz=30;
 return xyz;
}

void iniMem(void )
{
	int ii=0;
	for (int ii=0; ii< BUFFER_SIZE ; ii++)
	{
		statTemporal[ii] = coilTemporal[ii] = iregTemporal[ii] = hregTemporal[ii] = 0;
	}
	// ACTUALIZACION DE LAS IO
	if (io_escstats(0, BUFFER_SIZE, statTemporal) || 
	    io_esccoils(0, BUFFER_SIZE, coilTemporal) ||
	    io_escireg (0, BUFFER_SIZE, iregTemporal) ||
	    io_eschreg (0, BUFFER_SIZE, hregTemporal))
	{

        sprintf (lineasP->cadena,"Simulador: Fallo en la Inicializacion de la memoria (1).\n" );
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
   		return;
	}
	ii =  BUFFER_SIZE ;
	if (io_escstats(ii, BUFFER_SIZE, statTemporal) || 
	    io_esccoils(ii, BUFFER_SIZE, coilTemporal) ||
	    io_escireg (ii, BUFFER_SIZE, iregTemporal) ||
	    io_eschreg (ii, BUFFER_SIZE, hregTemporal))
	{
        sprintf (lineasP->cadena,"Simulador: Fallo en la Inicializacion de la memoria (2).\n" );
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
		return;
	}

	sprintf (lineasP->cadena,"Simulador: Actualizacion (Inicializacion) de memoria realizada.\n");
    move(SEC3(xyz), 4);
    addstr(lineasP->cadena);
    refresh();
	return;
}

int interfaz(char initW, int indices)
{
	if (initW)
	{

		endwin();
		w = initscr(); 				// Inicio de la librería
	    curs_set(0);				// 
	    erase();				// Borrado de la pantalla
	    getmaxyx(w, max_y, max_x);		// Tamaño de la pantalla: normal 34, 140

	    keypad(w, TRUE);			// Inicio del teclado
	    noecho();
	    nonl();
	    nodelay(w, TRUE);

        //if (has_colors())
        //{
        //    start_color();
	    //    init_pair(1,COLOR_RED,COLOR_WHITE);
	    //    init_pair(2,COLOR_YELLOW,COLOR_WHITE);
	    //    // init_pair(3,COLOR_RED,COLOR_WHITE);
        //}

		for (int i=0; i<10; i++) 			// Inicio de estructura para el manejo de la interfaz
		{
			lineasP = &lineasPantalla[i];
			memset ((char*)lineasP, 0, sizeof(LineaPantalla)); // inicializar en 0
		}

		move(0, 4);
	    sprintf (lineasP->cadena, "\t\t\t\tSISTEMA DE EMULACION GREEN HOUSE DE THORIUM V1.0 CON %i TANQUES", cantidad_tanques);
		addstr(lineasP->cadena);
		sprintf (lineasP->cadena,"Monitor de eventos:\n    ____________________________________________________________________________________________________");
        move(28, 4);
	    addstr(lineasP->cadena);
	}
	else
	{
        if (blinkking++ > 3) blinkking = -3;
		time(&timep); /* Obtener el tiempo de la estructura time_t, hora UTC */
		p = gmtime(&timep); /* Tiempo UTC convertido a estructura struct tm */
		sprintf (lineasP->cadena, "\t\t\t\t\t %d:%d:%d (UTC) %d/%d/%d (c) 2021 Thorium Corp", p->tm_hour, p->tm_min, p->tm_sec, p->tm_mday, 1 + p->tm_mon, 1900 + p->tm_year);
		move(1, 4);
		addstr(lineasP->cadena);

		sprintf (lineasP->cadena, "Monitor de proceso: Subir:(^),(>). Bajar:(v),(<). Salir: Esc. \n    _____________________________________________________________________________________________________");
		move(3, 4);
		addstr(lineasP->cadena);
		tanques = 0;
		tanques = &arreglo_tanques[yup];
		if (DEBUG)
        {
			sprintf (lineasP->cadena,"Mostrando tanque %s, de %i tanques leidos.", tanques->nombre, cantidad_tanques);
            move(SEC3(xyz), 4);
		    addstr(lineasP->cadena);
        }
		for (int i=0; i<10; i++) 
		{
			lineasP = &lineasPantalla[i];
			if (i==0)
			{
				sprintf (lineasP->cadena, "| TANQUE \t\t | CAPACIDAD(L)\t | ALTURA(m)\t | NIVEL('H20)\t |");
				lineasP->pos_y = 5;
				lineasP->pos_x = 4;
			}else
			if (i==1)
			{
				sprintf (lineasP->cadena, "| %s\t | %6.2f\t | %6.2f\t | %6.2f\t |", tanques->nombre, tanques->volumen, tanques->altura, tanques->iniNivel);
				lineasP->pos_y = 6;
				lineasP->pos_x = 4;
			}else
			if (i==2)
			{
				sprintf (lineasP->cadena, "| LLLC | LLC  | HLC  | HHLC |");
				lineasP->pos_y = 9;
				lineasP->pos_x = 4;
			}else
			if (i==3)
			{
				sprintf (lineasP->cadena, "|");
				if (( tanques->isStatBajoBajo > 0) && (tanques->volActual >= (tanques->volumen * tanques->isStatBajoBajo) ))
				{
					sprintf (lineasP->cadena, "%s  OFF |",lineasP->cadena);
				}else{
					if ( tanques->isStatBajoBajo == 0)
						sprintf (lineasP->cadena, "%s NONE |",lineasP->cadena);
					else
                    {
                        //if (has_colors()) attron(COLOR_PAIR(1) | A_BLINK);
						if (blinkking>0) sprintf (lineasP->cadena, "%s>ON   |",lineasP->cadena);
                        else sprintf (lineasP->cadena, "%s ON   |",lineasP->cadena);
                        //if (has_colors()) attroff(COLOR_PAIR(1));
					}
				}
				if (( tanques->isStatBajo > 0) && (tanques->volActual >= (tanques->volumen * tanques->isStatBajo) ))
				{
					sprintf (lineasP->cadena, "%s  OFF |",lineasP->cadena);
				}else{
					if ( tanques->isStatBajo == 0) 
						sprintf (lineasP->cadena, "%s NONE |",lineasP->cadena);
					else
                    {
                        //if (has_colors()) attron(COLOR_PAIR(1) | A_BLINK);
						if (blinkking>0) sprintf (lineasP->cadena, "%s>ON   |",lineasP->cadena);
                        else sprintf (lineasP->cadena, "%s ON   |",lineasP->cadena);
                        //if (has_colors()) attroff(COLOR_PAIR(1));
					}
                }
		        if (( tanques->isStatAlto > 0) && (tanques->volActual >= (tanques->volumen * tanques->isStatAlto) ))
                {
                    //if (has_colors()) attron(COLOR_PAIR(1) | A_BLINK);
					if (blinkking>0) sprintf (lineasP->cadena, "%s>ON   |",lineasP->cadena);
                    else sprintf (lineasP->cadena, "%s ON   |",lineasP->cadena);
                    //if (has_colors()) attroff(COLOR_PAIR(1));
				}else{
					if ( tanques->isStatAlto == 0) 
						sprintf (lineasP->cadena, "%s NONE |",lineasP->cadena);
					else
						sprintf (lineasP->cadena, "%s OFF  |",lineasP->cadena);
                }
				if (( tanques->isStatAltoAlto > 0) && (tanques->volActual >= (tanques->volumen * tanques->isStatAltoAlto) ))
				{
                    //if (has_colors()) attron(COLOR_PAIR(1) | A_BLINK);
					if (blinkking>0) sprintf (lineasP->cadena, "%s>ON   |",lineasP->cadena);
                    else sprintf (lineasP->cadena, "%s ON   |",lineasP->cadena);
                    //if (has_colors()) attroff(COLOR_PAIR(1));
				}else{
					if ( tanques->isStatAltoAlto == 0) 
						sprintf (lineasP->cadena, "%s NONE |",lineasP->cadena);
					else
                    {
						sprintf (lineasP->cadena, "%s OFF  |",lineasP->cadena);
                    }
                }
                lineasP->pos_y = 10;
                lineasP->pos_x = 4;
			}else
			if (i==4)
			{
				sprintf (lineasP->cadena, "| PhC   | Temp (°C)| ECC (S/m)| Presión (PSI)| Vibración (Hz)|");
				sprintf (lineasP->cadena, "%s Turbulencia (NTU)| Oxigeno (mg/L)|", lineasP->cadena);
				lineasP->pos_y = 13;
				lineasP->pos_x = 4;
			}else
			if (i==5)
			{
				sprintf (lineasP->cadena, "|");
				if ( tanques->phProm >= 0)
				{
					sprintf (lineasP->cadena, "%s %6.2f|",lineasP->cadena, tanques->phProm);
				}else{
					sprintf (lineasP->cadena, "%s NONE  |",lineasP->cadena);
				}
				//if ( tanques->teProm >= 0)
				//{
                    if ((blinkking>0) && ((tanques->teProm < 22.0) || (tanques->teProm > 37.0)))
					    sprintf (lineasP->cadena, "%s>%9.2f|",lineasP->cadena, tanques->teProm);
                    else
					    sprintf (lineasP->cadena, "%s %9.2f|",lineasP->cadena, tanques->teProm);
				//}else{
				//	sprintf (lineasP->cadena, "%s  NONE    |",lineasP->cadena);
				//}
				if ( tanques->eccProm >= 0)
				{
					sprintf (lineasP->cadena, "%s %9.2f|",lineasP->cadena, tanques->eccProm);
				}else{
					sprintf (lineasP->cadena, "%s NONE  |",lineasP->cadena);
				}
				if ( tanques->ptProm >= 0)
				{
                    if ((blinkking>0) && ((tanques->ptProm < 5.0) || (tanques->ptProm > 34.0)))
					    sprintf (lineasP->cadena, "%s>%13.2f|",lineasP->cadena, tanques->ptProm);
                    else
					    sprintf (lineasP->cadena, "%s %13.2f|",lineasP->cadena, tanques->ptProm);
				}else{
					sprintf (lineasP->cadena, "%s    NONE      |",lineasP->cadena);
				}
				if ( tanques->vtProm >= 0)
				{
					sprintf (lineasP->cadena, "%s %14.2f|",lineasP->cadena, tanques->vtProm);
				}else{
					sprintf (lineasP->cadena, "%s     NONE      |",lineasP->cadena);
				}
				if ( tanques->tutProm >= 0)
				{
					sprintf (lineasP->cadena, "%s %17.2f|",lineasP->cadena, tanques->tutProm);
				}else{
					sprintf (lineasP->cadena, "%s       NONE       |",lineasP->cadena);
				}
				if ( tanques->otProm >= 0)
				{
                    if ((blinkking>0) && ((tanques->otProm < 7.0) || (tanques->otProm > 50.0)))
	    				sprintf (lineasP->cadena, "%s>%14.2f|",lineasP->cadena, tanques->otProm);
                    else
		    			sprintf (lineasP->cadena, "%s %14.2f|",lineasP->cadena, tanques->otProm);
				}else{
					sprintf (lineasP->cadena, "%s      NONE     |",lineasP->cadena);
				}
				lineasP->pos_y = 14;
				lineasP->pos_x = 4;
			}else
			if (i==6)
			{
				sprintf (lineasP->cadena, "| Vál/Bomb de llenado 01 | Vál/Bomb de llenado 02 | Vál/Bomb de llenado 03 |");
				lineasP->pos_y = 17;
				lineasP->pos_x = 4;
			}else
			if (i==7)
			{
				sprintf (lineasP->cadena, "|");
				if ( tanques->isFillValv[0] > -1)		// Si esta instalada ?
				{
					if ( tanques->isFillValv[0] == 0) 	// Si es digital (on/off) ?
					{
						if  (tanques->fillEstad[0] != 0)		// Si esta abierta ?
							sprintf (lineasP->cadena, "%s         OPENED         |",lineasP->cadena);
						else
							sprintf (lineasP->cadena, "%s         CLOSED         |",lineasP->cadena);
					} else {				// Es analogica (% apertura)
						sprintf (lineasP->cadena, "%s         %6.2f        |",lineasP->cadena, tanques->fillEstad[0]);
					}
				} else {					// No esta instalada
					sprintf (lineasP->cadena, "%s          NONE          |",lineasP->cadena);
				}
				if ( tanques->isFillValv[1] > -1)
				{
					if ( tanques->isFillValv[1] == 0)
					{
						if  (tanques->fillEstad[1] != 0)
							sprintf (lineasP->cadena, "%s         OPENED         |",lineasP->cadena);
						else
							sprintf (lineasP->cadena, "%s         CLOSED         |",lineasP->cadena);
					} else {
						sprintf (lineasP->cadena, "%s         %6.2f        |",lineasP->cadena, tanques->fillEstad[1]);
					}
				} else {
					sprintf (lineasP->cadena, "%s          NONE          |",lineasP->cadena);
				}
				if ( tanques->isFillValv[2] > -1)
				{
					if ( tanques->isFillValv[2] == 0)
					{
						if  (tanques->fillEstad[2] != 0)
							sprintf (lineasP->cadena, "%s         OPENED         |",lineasP->cadena);
						else
							sprintf (lineasP->cadena, "%s         CLOSED         |",lineasP->cadena);
					} else {
						sprintf (lineasP->cadena, "%s         %6.2f        |",lineasP->cadena, tanques->fillEstad[2]);
					}
				} else {
					sprintf (lineasP->cadena, "%s          NONE          |",lineasP->cadena);
				}
				lineasP->pos_y = 18;
				lineasP->pos_x = 4;
			}else
			if (i==8)
			{
				sprintf (lineasP->cadena, "|  Válvula de descarga  |");
				lineasP->pos_y = 21;
				lineasP->pos_x = 4;
			}else
			if (i==9)
			{
				sprintf (lineasP->cadena, "|");
				if ( tanques->DescargaTipo > -1)
				{
					if ( tanques->DescargaTipo == 0)
					{
						if  (tanques->desEstad != 0)
							sprintf (lineasP->cadena, "%s         OPENED         |",lineasP->cadena);
						else
							sprintf (lineasP->cadena, "%s         CLOSED         |",lineasP->cadena);
					} else {
						sprintf (lineasP->cadena, "%s         %6.2f        |",lineasP->cadena, tanques->desEstad);
					}
				} else {
					sprintf (lineasP->cadena, "%s          NONE          |",lineasP->cadena);
				}
				lineasP->pos_y = 22;
				lineasP->pos_x = 4;
			}
    		move(lineasP->pos_y, lineasP->pos_x);
			addstr(lineasP->cadena);
		}
		refresh();
		tecla = getch();
		switch(tecla)
		{
			case KEY_UP :
            case 262 :
            case 339 :
            case KEY_RIGHT :
            case 62:
				yup++;
				if (yup>=cantidad_tanques) yup=cantidad_tanques-1;
				if (yup>=MAX_TANQUES) yup=MAX_TANQUES-1;
				break;
			case KEY_DOWN :
            case 360 :
            case 338 :
            case KEY_LEFT :
            case 60:
				yup--;
				if (yup<0) yup=0;
				break;
			case 27: // tecla ESC = Salir
				fin = true;
				break;
		}
		//sprintf (lineasP->cadena, "TECLAS=%i, UP=%4i, DW=%4i, LF=%4i, RG=%4i, DO => PRESS=%4i, yup=%i",tecla, KEY_UP, KEY_DOWN, KEY_LEFT,KEY_RIGHT, tecla, yup);
        //move(28, 4);
        //addstr(lineasP->cadena);
        refresh();
	}

	if (fin) 
	{
		endwin();
		exit(EXIT_SUCCESS);
	}
	return true;
}

void histTempFile (const char* archivoConf)
{
	int ind=0;
	
	FILE *f = fopen(archivoConf, "r");
	if (f)
	{
		while (fscanf(f, "%[^\n]\n", lineaTemporal) != EOF)
		{
				sscanf (lineaTemporal, "%s", valor);
				histTemp[ind++] = atof(valor); // JCM: Verificar la conversion de flotante
		}
		fclose(f);
	}
	return;
}

void cargarTanques (const char* archivoConf)
{
	int lineap=0, len=0, spnIrg=8;

	cantidad_tanques = 0;
	Tanque * tanque = 0;
	
	FILE *f = fopen(archivoConf, "r");
	if (f)
	{
		do
		{
			if (fscanf(f, "%[^\n]\n", lineaTemporal) != EOF)
			{
				len = strlen(lineaTemporal);
				sscanf (lineaTemporal, "%s %s %s", tag, valor, any);
				if (strcmp(tag, "TANQUE") == 0)
				{
					tanque = &arreglo_tanques[cantidad_tanques];
					//strcpy (tanque->nombre, valor);
					sprintf (tanque->nombre, "%s %s",valor, any);
					if (cantidad_tanques == 0)
					{
						spanCoil[cantidad_tanques] = spanStat[cantidad_tanques] = 0;
						spanIreg[cantidad_tanques] = spanHreg[cantidad_tanques] = 0;
					}else
					{
						spanCoil[cantidad_tanques] = spanCoil[cantidad_tanques-1];
						spanStat[cantidad_tanques] = spanStat[cantidad_tanques-1];
						spanIreg[cantidad_tanques] = spanIreg[cantidad_tanques-1];
						spanHreg[cantidad_tanques] = spanHreg[cantidad_tanques-1];
					}
				}
				else
				if (tanque && strcmp(tag, "VOLUMEN") == 0)
				{
					tanque->volumen = atof(valor); // JCM: Verificar como se convierte el flotante
				}
				else
				if (tanque && strcmp(tag, "ALTURA") == 0)
				{
					tanque->altura = atof(valor);
				}
				else
				if (tanque && strcmp(tag, "PM_RUN") == 0)
				{
					tanque->isPM_RUN = atoi(valor);
					if (tanque->isPM_RUN >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "PB_PM_START") == 0)
				{
					tanque->isPB_PM_START = atoi(valor);
					if (tanque->isPB_PM_START >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "PB_PM_STOP") == 0)
				{
					tanque->isPB_PM_STOP = atoi(valor);
					if (tanque->isPB_PM_STOP >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "LLC") == 0)
				{
					tanque->isStatBajo = atoi(valor);
					if (tanque->isStatBajo >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "HLC") == 0)
				{
					tanque->isStatAlto = atoi(valor);
					if (tanque->isStatAlto >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "LLLC") == 0)
				{
					tanque->isStatBajoBajo = atoi(valor);
					if (tanque->isStatBajoBajo >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "HHLC") == 0)
				{
					tanque->isStatAltoAlto = atoi(valor);
					if (tanque->isStatAltoAlto >= 1) spanStat[cantidad_tanques]++;
				}
				else
				if (tanque && strcmp(tag, "PT01") == 0)
				{
					tanque->isPT[0] = atoi(valor);
					if (tanque->isPT[0] >= 0) 
					{
						spanIreg[cantidad_tanques]++;
						tanque->ptValue[0] = 0.014503774;
					}
				}
				else
				if (tanque && strcmp(tag, "PT02") == 0)
				{
					tanque->isPT[1] = atoi(valor);
					if (tanque->isPT[1] >= 0)
					{
						spanIreg[cantidad_tanques]++;
						tanque->ptValue[1] = 0.014503774;
					}
				}
				else
				if (tanque && strcmp(tag, "PT03") == 0)
				{
					tanque->isPT[2] = atoi(valor);
					if (tanque->isPT[2] >= 0)
					{
						spanIreg[cantidad_tanques]++;
						tanque->ptValue[2] = 0.014503774;
					}
				}
				else
				if (tanque && strcmp(tag, "FT01") == 0)
				{
					tanque->isFT[0] = atoi(valor);
				}
				else
				if (tanque && strcmp(tag, "VLL01") == 0)
				{
					tanque->isFillValv[0] = atoi(valor);
					if (tanque->isFillValv[0] == 0) spanCoil[cantidad_tanques]++; // Valvula on/off
					if ((tanque->isFillValv[0] == 1) && (tanque->isFT[0] == 0)) spanHreg[cantidad_tanques]++; // Valvula analogica
					if ((tanque->isFillValv[0] == 1) && (tanque->isFT[0] == 1)) spanIreg[cantidad_tanques]++; // Transmisor flujo
				}
				else
				if (tanque && strcmp(tag, "VLL%01") == 0)
				{
					tanque->FlujoLlenado[0] = atof(valor);  // JCM: Verificar como se convierte el flotante
				}
				else
				if (tanque && strcmp(tag, "SOL_TYPE") == 0)
				{
					tanque->solType[0] = atof(valor);
				}
				else
				if (tanque && strcmp(tag, "FT02") == 0)
				{
					tanque->isFT[1] = atoi(valor);
				}
				else
				if (tanque && strcmp(tag, "VLL02") == 0)
				{
					tanque->isFillValv[1] = atoi(valor);
					if (tanque->isFillValv[1] == 0) spanCoil[cantidad_tanques]++;	// Valvula on/off
					if ((tanque->isFillValv[1] == 1) && (tanque->isFT[1] == 0)) spanHreg[cantidad_tanques]++; // Valvula analogica
					if ((tanque->isFillValv[1] == 1) && (tanque->isFT[1] == 1)) spanIreg[cantidad_tanques]++; // Transmisor flujo
				}
				else
				if (tanque && strcmp(tag, "VLL%02") == 0)
				{
					tanque->FlujoLlenado[1] = atof(valor);  // JCM: Verificar como se convierte el flotante
				}
				else
				if (tanque && strcmp(tag, "SOL_TYPE") == 0)
				{
					tanque->solType[1] = atof(valor);
				}
				else
				if (tanque && strcmp(tag, "FT03") == 0)
				{
					tanque->isFT[2] = atoi(valor);
				}
				else
				if (tanque && strcmp(tag, "VLL03") == 0)
				{
					tanque->isFillValv[2] = atoi(valor);
					if (tanque->isFillValv[2] == 0) spanCoil[cantidad_tanques]++;	// Valvula on/off
					if ((tanque->isFillValv[2] == 1) && (tanque->isFT[2] == 0)) spanHreg[cantidad_tanques]++; // Valvula analogica
					if ((tanque->isFillValv[2] == 1) && (tanque->isFT[2] == 1)) spanIreg[cantidad_tanques]++; // Transmisor flujo
				}
				else
				if (tanque && strcmp(tag, "VLL%03") == 0)
				{
					tanque->FlujoLlenado[2] = atof(valor);  // JCM: Verificar como se convierte el flotante
				}
				else
				if (tanque && strcmp(tag, "SOL_TYPE") == 0)
				{
					tanque->solType[2] = atof(valor);
				}
				else
				if (tanque && strcmp(tag, "LT") == 0)
				{
					tanque->isLT = atoi(valor);
					if (tanque->isLT >= 0) 
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
					}
				}
				else
				if (tanque && strcmp(tag, "LEVEL_INI") == 0)
				{
					tanque->iniNivel = atof(valor);
				}
				else
				if (tanque && strcmp(tag, "PhC01") == 0)
				{
					tanque->isPhC[0] = atoi(valor);
					if (tanque->isPhC[0] >= 0) 
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->phValue[0] = 7.0;
					}
				}
				else
				if (tanque && strcmp(tag, "PhC02") == 0)
				{
					tanque->isPhC[1] = atoi(valor);
					if (tanque->isPhC[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->phValue[1] = 7.0;
					}
				}
				else
				if (tanque && strcmp(tag, "PhC03") == 0)
				{
					tanque->isPhC[2] = atoi(valor);
					if (tanque->isPhC[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->phValue[2] = 7.0;
					}
				}
				else
				if (tanque && strcmp(tag, "TE01") == 0)
				{
					tanque->isTE[0] = atoi(valor);
					if (tanque->isTE[0] >= 0) 
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
					}
				}
				else
				if (tanque && strcmp(tag, "TE02") == 0)
				{
					tanque->isTE[1] = atoi(valor);
					if (tanque->isTE[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
					}
				}
				else
				if (tanque && strcmp(tag, "TE03") == 0)
				{
					tanque->isTE[2] = atoi(valor);
					if (tanque->isTE[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
					}
				}
				else
				if (tanque && strcmp(tag, "ECC01") == 0)
				{
					tanque->isECC[0] = atoi(valor);
					if (tanque->isECC[0] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->eccValue[0] = 0.05;
					}
				}
				else
				if (tanque && strcmp(tag, "ECC02") == 0)
				{
					tanque->isECC[1] = atoi(valor);
					if (tanque->isECC[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->eccValue[1] = 0.05;
					}
				}
				else
				if (tanque && strcmp(tag, "ECC03") == 0)
				{
					tanque->isECC[2] = atoi(valor);
					if (tanque->isECC[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->eccValue[2] = 0.05;
					}
				}
				else
				if (tanque && strcmp(tag, "VT01") == 0)
				{
					tanque->isVT[0] = atoi(valor);
					if (tanque->isVT[0] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->vtValue[0] = 432.0;
					}
				}
				else
				if (tanque && strcmp(tag, "VT02") == 0)
				{
					tanque->isVT[1] = atoi(valor);
					if (tanque->isVT[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->vtValue[1] = 432.0;
					}
				}
				else
				if (tanque && strcmp(tag, "VT02") == 0)
				{
					tanque->isVT[2] = atoi(valor);
					if (tanque->isVT[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->vtValue[2] = 432.0;
					}
				}
				else
				if (tanque && strcmp(tag, "TUT01") == 0)
				{
					tanque->isTUT[0] = atoi(valor);
					if (tanque->isTUT[0] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->tutValue[0] = 1;
					}
				}
				else
				if (tanque && strcmp(tag, "TUT02") == 0)
				{
					tanque->isTUT[1] = atoi(valor);
					if (tanque->isTUT[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->tutValue[1] = 1;
					}
				}
				else
				if (tanque && strcmp(tag, "TUT03") == 0)
				{
					tanque->isTUT[2] = atoi(valor);
					if (tanque->isTUT[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->tutValue[2] = 1;
					}
				}
				else
				if (tanque && strcmp(tag, "OT01") == 0)
				{
					tanque->isOT[0] = atoi(valor);
					if (tanque->isOT[0] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->otValue[0] = 7;
					}
				}
				else
				if (tanque && strcmp(tag, "OT02") == 0)
				{
					tanque->isOT[1] = atoi(valor);
					if (tanque->isOT[1] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->otValue[1] = 7;
					}
				}
				else
				if (tanque && strcmp(tag, "OT03") == 0)
				{
					tanque->isOT[2] = atoi(valor);
					if (tanque->isOT[2] >= 0)
					{
						spanIreg[cantidad_tanques]=spanIreg[cantidad_tanques]+spnIrg;
						spnIrg++;
						tanque->otValue[2] = 7;
					}
				}
				else
				if (tanque && strcmp(tag, "VD") == 0)
				{
					tanque->DescargaTipo = atoi(valor);
					//if (tanque->DescargaTipo == 0) spanCoil[cantidad_tanques]++;	// Valvula on/off
					//if (tanque->DescargaTipo == 1) spanHreg[cantidad_tanques]++;	// Valvula analogica
				}
				else
				if (tanque && strcmp(tag, "VD%") == 0)
				{
					tanque->FlujoDescarga = atof(valor);  // JCM: Verificar como se convierte el flotante
				}
				else
				if (tanque && strcmp(tag, "SPDESC") == 0)	// Span de la valvula de llenado (dir de IO)
				{
					tanque->spanDesc = atoi(valor);
					tanque->desEstad = 0;
					if (tanque->DescargaTipo == 0) spanCoil[cantidad_tanques]=tanque->spanDesc;	// Valvula on/off
					if (tanque->DescargaTipo == 1) spanHreg[cantidad_tanques]=tanque->spanDesc;	// Valvula analogica
				}
				else
				if (tanque && strcmp(tag, "FIN_TANQUE") == 0)
				{
					cantidad_tanques++;
					if (DEBUG) printf ("Cargado tanque %s, de %i tanques leidos.\n", tanque->nombre, cantidad_tanques);
					if (cantidad_tanques >= MAX_TANQUES) break;
				}
			}else 
			{ 
				if (DEBUG) printf ("NO SE PROCESO LA linea %i...%s\n", lineap, lineaTemporal); 
				break;
			}
		}
		while (1);
		fclose(f);
		if (DEBUG) printf ("Saliendo de procesar arch cfg, con %i tanques..\n", cantidad_tanques);
	}else 
	{ 
		if (DEBUG) printf ("NO SE ABRIO EL ARCHIVO %s\n", archivoConf); 
	}
	return;
}

void simularTanque (int indices)
{
	int spanS=0, spanC=0, spanI=0, spanH=0, j=0, k=0, jj=0;
	// Tanque * tanque = &arreglo_tanques[indices];
	tanque = 0;
	tanque = &arreglo_tanques[indices];
	if (DEBUG)
    {
		sprintf (lineasP->cadena,"Simulando el tanque %i, %s \n", indices+1,tanque->nombre );
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }
	// Lectura inicial en cada ciclo de los estados de las variables de entrada y salida
	if (indices == 0)  // Si es el primet tanque
	{
		spanS = spanC = spanI = spanH = 0;
		if (DEBUG)
        {
			sprintf (lineasP->cadena,"Simulador: Paso por control 0 de span: spanS=%i, spanC=%i, spanI=%i, spanH=%i\n", spanS , spanC , spanI , spanH);
            move(SEC3(xyz), 4);
	        addstr(lineasP->cadena);
            refresh();
        }
	} 
    else 
	{
		spanS = spanStat[indices-1];
		spanC = spanCoil[indices-1];
		spanI = spanIreg[indices-1];
		spanH = spanHreg[indices-1];
		if (DEBUG) 
        {
			sprintf (lineasP->cadena,"Simulador: Paso por control diff 0 de span: spanS=%i, spanC=%i, spanI=%i, spanH=%i\n", spanS , spanC , spanI , spanH);
            move(SEC3(xyz), 4);
	        addstr(lineasP->cadena);
            refresh();
        }
	}
	if (DEBUG)
    { 
		sprintf (lineasP->cadena,"Simulador: 1 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
	    addstr(lineasP->cadena);
        refresh();
    }
	if ((io_leestat(spanS, NCERO(spanStat[indices]), statTemporal)) || 
		(io_leecoil(spanC, NCERO(spanCoil[indices]), coilTemporal)) || 
		(io_leeireg(spanI, NCERO(spanIreg[indices]), iregTemporal)) ||
		(io_leehreg(spanH, NCERO(spanHreg[indices]), hregTemporal)))
	{
		if (DEBUG)
        {
			sprintf (lineasP->cadena,"Simulador: Fallo en la lectura del Estado del tanque %s...\n", tanque->nombre);
            move(SEC3(xyz), 4);
	        addstr(lineasP->cadena);
            refresh();
        }
	}
	if (DEBUG)
    {
		sprintf (lineasP->cadena,"Simulador: 1.4 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }

	// Verificar si las valvulas de descarga esta presente y su estado para actualizar volumen de salida
	volumen_entrante=0;
	volumen_saliente=0;
	if (DEBUG) 
    {
		sprintf (lineasP->cadena,"Simulador: 2 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }
	j = clock_gettime (CLOCK_REALTIME, &ts);
	diffTiempo[indices] = (tsa.tv_sec - ts.tv_sec); // Tiempo en segundo
	tsa = ts;

	// VOLUMEN DE SALIDA
	// DescargaTipo =0 =>Digital: volumen out = apertura valvula/bomba (on=1/off=0)* flujo * tiempo pasado desde la ultima actualizacion (OCA)
	if (tanque->DescargaTipo == 0) // Flujo por minuto que ha salido
		volumen_saliente = coilTemporal[tanque->spanDesc] * tanque->FlujoDescarga * (diffTiempo[indices] / 60); 
	// Analogico: volumen out = %apertura valvula/bomba * flujo * tiempo pasado desde la ultima actualizacion (OCA)
	if (tanque->DescargaTipo == 1)
		volumen_saliente  = ((hregTemporal[tanque->spanDesc] * 32767.00)/ 100.0)* tanque->FlujoDescarga * (diffTiempo[indices] / 60);

	// VOLUMEN DE ENTRADA
	// Verificar si las valvulas/bombas de llenado estan presentes y el estado de cada una para actualizar volumen de entrada
	// Cuando son bombas, cada bomba presente tiene asociado un transmisor de flujo
	for (j=0; j<3; j++)
	{
		tanque->fillEstad[j] = 0;
		// Digital: volumen in = apertura valvula (on=1/off=0)* flujo * tiempo desde la ultima actualizacion
		if (tanque->isFillValv[j] == 0)  
		{	// Flujo por minuto que ha entrado
			volumen_entrante = volumen_entrante + coilTemporal[j] * tanque->FlujoLlenado[j] * (diffTiempo[indices] / 60);
			tanque->fillEstad[j] = (coilTemporal[j] && TRUE); // Estado de la valvula
		}
		// Analogico: volumen in = %apertura valvula * flujo * tiempo desde la ultima actualizacion
		if (tanque->isFillValv[j] == 1)  
		{	// Flujo por minuto que ha entrada
                        if (tanque->isFT[j] >= 1) // Si presente transmisor flujo hay bomba y se toma la medicion de ese transmisor para el calculo
			{
				volumen_entrante = volumen_entrante + iregTemporal[j] * tanque->FlujoLlenado[j] * (diffTiempo[indices] / 60);
				// Valor del flujo
				tanque->fillEstad[j] = iregTemporal[j]; 
			}
			else   // Si no esta el transmisor de flujo hay una valvula y se toma la apertura de la valvula para el calculo
			{
				volumen_entrante = volumen_entrante + hregTemporal[j] * tanque->FlujoLlenado[j] * (diffTiempo[indices] / 60);
				// Estado de la valvula, de cuentas a % apertura. 0=cerrada, 32767=abierta 100%
				tanque->fillEstad[j] = (32767 * hregTemporal[j]) / 100.00; 
			}
		}
	}

	if (DEBUG)
    {
		sprintf (lineasP->cadena,"Simulador: 3 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }

	// VOLUMEN TOTAL ACTUAL DEL TANQUE
	tanque->volActual = tanque->volActual + volumen_entrante - volumen_saliente;
	if (tanque->volActual > tanque->volumen)
		tanque->volActual = tanque->volumen;

	if (tanque->volActual < 0)
		tanque->volActual = 0;

	// Calculo del nivel del tanque pulgadas de agua JCM: FALTA EL CALCULO ADECUADO
	tanque->iniNivel = tanque->volActual * (tanque->volumen / tanque->altura);

	// Control de los limites de volumen del tanque en pulgadas
	if (tanque->iniNivel > tanque->volumen)
		tanque->iniNivel = tanque->volumen;
	if (tanque->iniNivel < 0)
		tanque->iniNivel = 0;

	// CAMBIO A UNIDADES DE INGENIERIA SEGUN CAPACIDAD DEL TANQUE
	tanque->volActual = (tanque->volActual * 32767.0) / NCERO(tanque->volumen);
	tanque->iniNivel = (tanque->iniNivel * 32767.0) / NCERO(tanque->volumen);
	// volumen_saliente = (volumen_saliente * 21767.0) / NCERO(tanque->volumen);  // NO HACE FALTA YA QUE NO USA CUENTAS
	volumen_entrante = (volumen_entrante *32767.0) / NCERO(tanque->volumen);

	if (DEBUG)
    {
		sprintf (lineasP->cadena,"Simulador: 4 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }
	// Particular de pH y ECC
	tanque->phProm = 7;		// pH= 7 en unidades de ingenieria; pH neutro por defecto
	tanque->eccProm = 0.058;	// Ecc 0.058 S/m en unidades de ingenieria; ECC por defecto del agua potable. S/m. Simmens por metro
	tanque->teProm = 26;        // Sensor de rango entre -30 y +70 °C
	tanque->ptProm = 0.014503774;	// Presion atmosferica por defecto (en PSI)
	tanque->vtProm = 432.0;		// La brivacion de la tierra es por defecto desde el 2020 es 432hz
	tanque->tutProm = 1;		// La turbulencia se reporta en unidades nefelométricas (NTU). Lo normal es 
	tanque->otProm = 7.0; 		// Oxigeno disuelto en agua es 7.0 a 8.0 mg/L. Concentracion inversamente a la temperatura
	
	for (k=0; k<3; k++)
	{
		// Esta instalado el sensor de pH (i)
		if (tanque->isPhC[k] >= 0)  
		{
			for (j=0; j<3; j++)
			{
				// Si la entrada fill(j) esta presente se calcula el aporte de acides o alcalinidad
				// Digital: volumen in = estado de la valvula * flujo * tiempo desde la ultima actualizacion en minutos
				if (tanque->isFillValv[j] == 0)
					valorPhAcid = coilTemporal[j] * tanque->FlujoLlenado[j] * (diffTiempo[indices]/60);
				// Analogico: volumen in = %apertura valvula * flujo * tiempo desde la ultima actualizacion en minutos
				if (tanque->isFillValv[j] == 1)  
					valorPhAcid = hregTemporal[j] * tanque->FlujoLlenado[j] * (diffTiempo[indices]/60);
				// SOLUTION TYPE: = 0 ACID BASE (PH REDUCTOR), = 1 NUTRIENT SOLUTION, =2 H20 BASE NEUTRA
				if (tanque->solType[j] == 0)
				{
					//tanque->phValue[k] = tanque->phValue[k] - (14.0 - (valorPhAcid / NCERO(tanque->volumen));
					//tanque->phValue[k] = tanque->phValue[k] - (valorPhAcid / NCERO(tanque->volumen));
					tanque->phValue[k] -= (valorPhAcid / NCERO(tanque->volumen));
				}
				// SOLUTION TYPE: = 0 ACID BASE (PH REDUCTOR), = 1 NUTRIENT SOLUTION, =2 H20 BASE NEUTRA
				if (tanque->solType[j] == 1) 	  
				{
					//tanque->phValue[k] = tanque->phValue[k] - (14.0 + ((valorPhAcid * 14.0) / NCERO(tanque->volumen));
					//tanque->phValue[k] = tanque->phValue[k] - ((valorPhAcid * 14.0) / NCERO(tanque->volumen));
					tanque->phValue[k] -= ((valorPhAcid * 32767.0) / NCERO(tanque->volumen));
				}
				// SOLUTION TYPE: = 0 ACID BASE (PH REDUCTOR), = 1 NUTRIENT SOLUTION, =2 H20 BASE NEUTRA
				if (tanque->solType[j] == 2)
				{
					// tanque->phValue[k] = tanque->phValue[k] - (14.0 - (valorPhAcid / NCERO(tanque->volumen));
					// tanque->phValue[k] = tanque->phValue[k] - ((valorPhAcid * 7.0) / NCERO(tanque->volumen));
					tanque->phValue[k] -= ((valorPhAcid * 16383.5) / NCERO(tanque->volumen));
				}
				if (tanque->phValue[k] > 32767.00) tanque->phValue[k] = 32767.00;
				if (tanque->phValue[k] < 0.00)  tanque->phValue[k] = 0.00;
			}
			// Si hay al menos un sensor definido pasara por aca.
			tanque->phProm = (tanque->phProm + tanque->phValue[k]) / 2.0;
		}

		// Esta instalado el sensor de ECC (i) (OCA)
		if (tanque->isECC[k] >= 0)  
		{
			// Constante asumida para sensor ecc
			tanque->eccValue[k] = tanque->eccValue[k] + (volumen_entrante* 0.058);
			// Si hay al menos un sensor definido pasara por aca.
			tanque->eccProm = (tanque->eccProm + tanque->eccValue[k]) / MINI(2, (k+1));
		}

		if (DEBUG)
        {
			sprintf (lineasP->cadena,"Simulador: 5 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
            move(SEC3(xyz), 4);
	        addstr(lineasP->cadena);
            refresh();
        }
		time(&timep); /* Obtener el tiempo de la estructura time_t, hora UTC */
		p = gmtime(&timep); /* Tiempo UTC convertido a estructura struct tm */
		// printf("%d/%d/%d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
		// printf("%s %d:%d:%d\n", wday[p->tm_wday], p->tm_hour,
		// p->tm_min, p->tm_sec);

		// Calculo Estimado de temperatura
		if (tanque->isTE[k] >= 0)  // Esta instalado el sensor de TE (i) (OCA)
		{
			tanque->teValue[k] = ((histTemp[MINI((p->tm_mday * p->tm_hour),720)] / NCERO(tanque->eccProm))/ 60)* diffTiempo[indices];
            if (tanque->teValue[k] < -30.00)
                tanque->teValue[k] = -30.0;
            if (tanque->teValue[k] > 70.00)
                tanque->teValue[k] = 70.0;
			tanque->teProm = (histTemp[p->tm_hour] + tanque->teValue[k]) / MINI(2, (k+1));
		}
		// Calculo Estimado de Presion
		if (tanque->isPT[k] >= 0)  // Esta instalado el sensor de PT (i) (OCA)
		{	// Volumen en PSI + presion atmosferica
			tanque->ptValue[k] = ((tanque->altura / tanque->volumen) * tanque->volActual); 
			tanque->ptProm = (tanque->ptProm + tanque->ptValue[k]) / MINI(2, (k+1));
		}
		// Calculo de Vibracion
		if (tanque->isVT[k] >= 0)  // Esta instalado el sensor de VT (i) (OCA)
		{
			tanque->vtValue[k] = tanque->isFillValv[0]*432.0 + tanque->isFillValv[1]*432.0 + tanque->isFillValv[2]*432.0;
			tanque->vtValue[k] += tanque->DescargaTipo * 108.0;
			tanque->vtValue[k] = tanque->vtValue[k] - 432.0;
			tanque->vtProm = (tanque->vtProm + tanque->vtValue[k]) / 2.0;
		}
		// Calculo de turbidez (turbulencia)
		if (tanque->isTUT[k] >= 0)  // Esta instalado el sensor de TU (i) (OCA)
		{
			tanque->tutValue[k] = (tanque->eccProm*0.89)+(tanque->isFillValv[0]*0.89);
			tanque->tutValue[k] += (tanque->isFillValv[1]*0.89)+(tanque->isFillValv[2]*0.89);
			tanque->tutProm = (tanque->tutProm + tanque->tutValue[k]) / MINI(2, (k+1));
		}
		// Calculo de Oxigeno
		if (tanque->isOT[k] >= 0)  // Esta instalado el sensor de TE (i) (OCA)
		{
			tanque->otValue[k] = (7.00 / NCERO(tanque->volumen)) * tanque->volActual;
			tanque->otProm = (tanque->otProm + tanque->otValue[k]) / MINI(2, (k+1));
		}
	}
	// PH EN UNIDADES DE INGENIERÍA SEGUN ESCALA 0.0 - 14.0
	// tanque->phProm = (tanque->phProm * 32767.0) / 14.0;

	// AJUSTE DE PRESION SEGUN EL VOLUMEN ACTUAL
	tanque->ptProm =  0.00014503774 * ((tanque->ptProm / tanque->volumen) * tanque->volActual); 

	// AJUSTES DE OT, TUT Y OT DE ACUERDO AL VOLUMEN ACTUAL
	if (tanque->volActual <= 0 )
	{
		tanque->otProm = 0.0;
		tanque->tutProm = 0.0;
	}
	
	if (DEBUG)
    {
        move(SEC3(xyz), 4);
		sprintf (lineasP->cadena,"Simulador: 6 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        addstr(lineasP->cadena);
        refresh();
    }

	// Calculo de los switches de nivel. Estado inicial vacio
        jj=3; // Los sw de nivel comienzan en la 4ta posicion
	// statTemporal[3] = statTemporal[4] = statTemporal[5] = statTemporal[6] = -1;
	if ( tanque->isStatBajo >= 1)
		statTemporal[jj++] = 1;
	if ( tanque->isStatAlto >= 1)
		statTemporal[jj++] = 1;
	if ( tanque->isStatBajoBajo >= 1)
		statTemporal[jj++] = 0; 
	if ( tanque->isStatAltoAlto >= 1)
		statTemporal[jj] = 0;
	jj=3;
	if (( tanque->isStatBajo >= 1) && (tanque->volActual >= (tanque->volumen * (tanque->isStatBajo / 100.0) )))
	{
		statTemporal[jj++] = 0;
	}
	if (( tanque->isStatAlto >= 1) && (tanque->volActual >= (tanque->volumen * (tanque->isStatAlto / 100.0) )))
	{
		statTemporal[jj++] = 1;
	}
	if (( tanque->isStatBajoBajo >= 1) && (tanque->volActual >= (tanque->volumen * (tanque->isStatBajoBajo / 100.0) )))
	{
		statTemporal[jj++] = 0; 
	}
	if (( tanque->isStatAltoAlto >= 1) && (tanque->volActual >= (tanque->volumen * (tanque->isStatAltoAlto / 100.0) )))
	{
		statTemporal[jj] = 1;
	}
	if (DEBUG)
    {
		sprintf (lineasP->cadena,"Simulador: 7 CONTROL: tanque=%s, cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }

	// ACTUALIZACION DE LAS IO
	if (io_escstats(spanS, NCERO(spanStat[indices]), statTemporal) || 
	    io_escireg(spanI, NCERO(spanIreg[indices]), iregTemporal))
	{
		if (DEBUG)
        {
            sprintf (lineasP->cadena,"Simulador: Fallo en la escritura del tanque,  %s... cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
            move(SEC3(xyz), 4);
	        addstr(lineasP->cadena);
            refresh();
        }
		return;
	}
	if (DEBUG)
    {
        sprintf (lineasP->cadena,"Simulador: Escritura del tanque,  %s... cantidad_tanques=%i\n", tanque->nombre, cantidad_tanques);
        move(SEC3(xyz), 4);
        addstr(lineasP->cadena);
        refresh();
    }
	return;
}

int main(int argc, char **argv)
{
	int opt, qwe=0, isArcConf=0; 

	for (int i=0; i<MAX_TANQUES; i++) 			// Inicio de estructura para el manejo de la interfaz
	{
		tanque = 0;
		tanque = &arreglo_tanques[i];
		memset ((char*)tanque, 0, sizeof(Tanque)); // inicializar en 0
	}


	while ((opt = getopt (argc, argv, "dhvcf:u:p:n:i:l:")) != -1)
	{
		switch (opt)
		{
			  /*
			   * Create option
			   */
			case 'c':
			  //++create;
			  break;
			  /*
			   * depurar option
			   */
			case 'd':
				DEBUG = true;
				break;
			  /*
			   * Verbosity option
			   */
			case 'v':
			//	++publicar;
			//	if (publicar > 3)
			//		publicar=3;
				break;
			  /*
			   * Help message option
			   */
			case 'h':
			//	usage (who);
			//  exit (0);
				break;
			  /*
			   * Name of Masters definition text file
			   */
			case 'f':
				cargarTanques (optarg);
				isArcConf = 1;
				break;
			/*
			* Direccion de este esclavo
			*/
			case 'u':
			//	ua = atoi (optarg);
				break;
			  /*
			   * Name of Shared Memory Region (/dev/shmem/<name>)
			   */
			case 'n':
			//	strcpy (plc_name, optarg);
				break;
			  /*
			   * Nombre del puerto serial utilizado por este protocolo
			   */
			case 'p':
			  strcpy (this_port, optarg);
			  break;
			  /*
			   * Nombre del canal para esta conexion
			   */
			case 'l':
			//  strcpy (this_canal, optarg);
			  break;
			  /*
			   * Nombre de la instancia de este protocolo
			   */
			case 'i':
			//  strcpy (this_instance, optarg);
			  break;
			  /*
			   * Unknown option
			   */
			case '?':
				if (DEBUG) printf ("ERROR:: Parámetro inválido.\n");
				exit(-1);
			}
	}
    
	if (isArcConf)
	{

		histTempFile ("../data/histTemp.cfg");

		interfaz(1, 0);	// Se invoca para iniciar la interfaz y el teclado
		// Ejecuta el servidor modbus tcp
		if (DEBUG)
	    {
		sprintf (lineasP->cadena,"Hacia el ciclo principal...\n" );
		move(SEC3(xyz), 4);
		addstr(lineasP->cadena);
		refresh();
	    }

		// Abriendo y vinculando a la memoria compartida
		if (Aper_zm()==0)
		{
			iniMem();
			while(1)
			{
				for (qwe = 0; qwe < cantidad_tanques; qwe++)
				{
					simularTanque(qwe);
					interfaz(0, qwe);
				}
					sleep(1);
			}
		}
		else
		{
			if (DEBUG)
		{
		    sprintf (lineasP->cadena,"Error fatal abriendo la memoria...\n" );
		    move(SEC3(xyz), 4);
		    addstr(lineasP->cadena);
		    refresh();
		}
		sleep(2);
		}

		// Cerrando y/o desvinculando
		Desv_zm();
		sprintf (lineasP->cadena, "pkill -9 -f plc_mem");
		system(lineasP->cadena);
	}
	else
		sprintf (lineasP->cadena,"Error: Falta archivo de configuracion opción -p ruta/nombre \n" );

	return 0;
}
