/*
 * Entradas.h
 *
 *  Created on: 30/01/2021
 *      Author: wintrok
 */

#ifndef ENTRADAS_H_
#define ENTRADAS_H_

#define MASK_POL_PUERTA      0
#define MASK_POL_PUERTA_INV  1



typedef struct
{
  unsigned char Flanco_Cerrada : 1;
  unsigned char Nivel_Cerrada  : 1;
  unsigned char Flanco_Abierta : 1;
  unsigned char Nivel_Abierta  : 1;
}strInputs;

/***************************
 * DEFINICION DE VARIABLES *
 ***************************/



/****************************** 
 * PROTOTIPO DE LAS FUNCIONES *
 ******************************/
void Entradas (void);

#endif /* ENTRADAS_H_ */
