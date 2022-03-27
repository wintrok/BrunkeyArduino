/*
 * Entradas.c
 *
 *  Created on: 30/01/2021
 *      Author: wintrok
 */


#include <Arduino.h>
#include "Entradas.h"
#include "Config_Operador.h"

#define Val_Puerta  digitalRead(PIN_PUERTA)
strInputs Entradas_Operador;

//*******************

void Entradas (void)
{
  /*strInputs Aux_Entradas_operador;

  Aux_Entradas_operador.Nivel_Cerrada = Entradas_Operador.Nivel_Cerrada;
  Entradas_Operador.Nivel_Cerrada = Val_Puerta ^ MASK_POL_PUERTA_INV;
  Entradas_Operador.Flanco_Cerrada = (Entradas_Operador.Nivel_Cerrada ^ Aux_Entradas_operador.Nivel_Cerrada) & Entradas_Operador.Nivel_Cerrada;

  Aux_Entradas_operador.Nivel_Abierta = Entradas_Operador.Nivel_Abierta;
  Entradas_Operador.Nivel_Abierta = Val_Puerta ^ MASK_POL_PUERTA;
  Entradas_Operador.Flanco_Abierta = (Entradas_Operador.Nivel_Abierta ^ Aux_Entradas_operador.Nivel_Abierta) & Entradas_Operador.Nivel_Abierta;
  */
}
