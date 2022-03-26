/*
 * Crear_Llaves.c
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */
#include "Crear_Llaves.h"
#include "DEC_HCS301.h"
#include "RTOS.h"
//#include "Aplicacion.h"

unsigned char State_Crear_Llaves = Offset_Estado | ST_ESPERAR_CREAR;
extern unsigned char ucJA;
extern stDataDecrypt strControl_RX;
StrFlags_Llaves ucHab_Crear_Llave;



// Desarrollo de la maquina de estados

void Crear_Llaves(void)
{
   CompararEstado(&State_Crear_Llaves);

   switch(State_Crear_Llaves & ucMaskState)
   {

      //*******************************************************************************
      // Estado encargado de esperar la orden para generar llaves para un nuevo
      // apartamento...
      //*******************************************************************************

      case ST_ESPERAR_CREAR:
        if(ucHab_Crear_Llave.Genera_Llave)
        {
          // Es dada la orden para generar la llave
          ucHab_Crear_Llave.Habilitar_Generacion = 1;
          CambiarEstado(&State_Crear_Llaves, ST_ESPERAR_HOPPING_LLAVE);
        }
      break;

      //*******************************************************************************
      // Estado encargado de esperar la orden para generar llaves para un nuevo
      // apartamento...
      //*******************************************************************************

      case ST_ESPERAR_HOPPING_LLAVE:
        if(!ucHab_Crear_Llave.Habilitar_Generacion)
        {
          //La llave a terminado de ser generada
          CambiarEstado(&State_Crear_Llaves, ST_ALMACENAR_LLAVE);
        }
      break;




   }
}
