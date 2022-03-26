/*
 * Calc_Key.h
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */

#ifndef SOURCES_CALC_KEY_H_
#define SOURCES_CALC_KEY_H_

#include "RTOS.h"

/* Definiciones para la aplicacion*/




//#define _USE_HCS12_
#define _USE_HCS08_


#define Llave_Control TRUE
#define Descifrar     FALSE


/* Macros utilizados en la aplicacion*/


/*Declaracion de los Estados de la Aplicacion*/

enum Select_State_Calc_Key
{
   ST_ESPERA_ACTIVACION_CALC_KEY,
   ST_INICIAR_DESCIFRAR_HOPPING,
   ST_REALIZAR_DECRYPT,
   ST_AJUSTAR_DATOS_DECRYPT,
   ST_INICIAR_GENERACION_LLAVE

};





/* Definicion de estructuras utilizadas en la aplicacion*/

typedef struct
{
   unsigned short int  Cont_Sync;
   unsigned short int  Verif_Decrypt : 10;
   unsigned short int  Bits_OVR : 2;
   unsigned short int  Boton_4 : 1;
   unsigned short int  Boton_2 : 1;
   unsigned short int  Boton_1 : 1;
   unsigned short int  Boton_3 : 1;
}stRtaDecrypt;

typedef struct
{
   unsigned char  ucHOP1;
   unsigned char  ucHOP2;
   unsigned char  ucHOP3;
   unsigned char  ucHOP4;
}stHop;

typedef struct
{
   unsigned char  ucKEY0;
   unsigned char  ucKEY1;
   unsigned char  ucKEY2;
   unsigned char  ucKEY3;
   unsigned char  ucKEY4;
   unsigned char  ucKEY5;
   unsigned char  ucKEY6;
   unsigned char  ucKEY7;
}stKey;

/* Definicion de funciones utilizadas en la aplicacion*/

void Calculo_Hopping(void);

#endif
