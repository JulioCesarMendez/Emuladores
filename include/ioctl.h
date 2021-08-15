/*
 * (c) 2012 PDVSA Intevep, S.A.
 * Autor: Julio Cesar Mendez <mendezjcx@pdvsa.com> <kuaimarejcm@gmail.com>
 */
#ifndef _IOCTL_H
#define _IOCTL_H
/*
 * Prototipo de funciones en ioctl.c
 */
extern int  rts_on(int);
extern int  rts_off(int);
extern int  dtr_on(int, int);
extern int  dtr_off(int, int);

#endif                                           /* _IOCTL_H                */
