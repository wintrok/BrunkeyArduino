/*
 * Crear_Llaves.h
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */

#ifndef SOURCES_CREAR_LLAVES_H_
#define SOURCES_CREAR_LLAVES_H_

#include  "RTOS.h"




/* Definiciones para la aplicacion*/


/* Definicion constantes para la aplicacion*/



/* Definicion estructuras para la aplicacion*/

 typedef struct
{
   unsigned char Genera_Llave:1;
   unsigned char Habilitar_Generacion:1;
}StrFlags_Llaves;




/*Declaracion de los Estados de la Aplicacion*/

enum Select_State_Crear_Llaves
{
   ST_ESPERAR_CREAR,
   ST_ESPERAR_HOPPING_LLAVE,
   ST_ALMACENAR_LLAVE
};







/* Definicion de funciones utilizadas en la aplicacion*/

void Crear_Llaves(void);     //Maquina de estados principal




#endif /* SOURCES_CREAR_LLAVES_H_ */
