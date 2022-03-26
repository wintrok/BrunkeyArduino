/***********************************
 * Sirenas.cpp
 * Created on: 01/02/2021
 * Author: Wintrok Ingenieria
 * Aplicacion encargada de generar los puslsos que activan los diferentes sonidos de la sirena
 * 
 * Pendientes:
 *    
 ***********************************/

#include "Sirenas.h"
#include <Arduino.h>
#include "RTOS.h"

enum Select_State_Sirenas
{
   ST_TIEMPO_INICIO_SIRENAS = 0,
   ST_ESPERA_ACTIVAR_SIRENA,
   ST_ACTIVANDO_SIRENA,
};


strSirenas Flags_Sirenas;

unsigned char ucEstado_Sirenas = Offset_Estado | ST_TIEMPO_INICIO_SIRENAS;

unsigned long ulTime_Sirenas;


/**********************************
 * FUNCION PRINCIPAL DEL PROGRAMA *
 **********************************/

 void Sirenas(void)
 {
    
    CompararEstado(&ucEstado_Sirenas);
    switch(ucEstado_Sirenas & ucMaskState)
    {

      /*********************************************
      * Estado de espera para arranque del sistema *
      *********************************************/
      case ST_TIEMPO_INICIO_SIRENAS:
        if(Inicio_Estado(ucEstado_Sirenas))
        {
          Reiniciar_Temporizador(&ulTime_Sirenas);
        }
        else
        {
          if(Retorna_Temporizador(DELAY_INIT_SIRENAS, Time_100mS, &ulTime_Sirenas))
          {
            CambiarEstado(&ucEstado_Sirenas, ST_ESPERA_ACTIVAR_SIRENA);
          }
        }
      break;      

      /*******************************************************************
      * Estado encargado de esperar la orden de activacion de una sirena *
      ********************************************************************/
      case ST_ESPERA_ACTIVAR_SIRENA:
        // Se debe activar la salida de Incendio?
        if(Flags_Sirenas.Sirena_Incendio)
        {
          // Se activa señal de Incendio
          SIRENA_INCENDIO_ON;
          Flags_Sirenas.Sirena_Incendio = false;
          CambiarEstado(&ucEstado_Sirenas, ST_ACTIVANDO_SIRENA);
        }
      break;


      /*******************************************************************************
      * Estado encargado de controlar el tiempo que dura activado el pulso de sirena *
      ********************************************************************************/
      case ST_ACTIVANDO_SIRENA:
        if(Inicio_Estado(ucEstado_Sirenas))
        {
          Reiniciar_Temporizador(&ulTime_Sirenas);
        }
        else
        {
          if(Retorna_Temporizador(TIME_ACTIVA_SIRENAS, Time_100mS, &ulTime_Sirenas))
          {
            SIRENA_INCENDIO_OFF;
            CambiarEstado(&ucEstado_Sirenas, ST_ESPERA_ACTIVAR_SIRENA);
          }
        }
      break;

      
    }
 }
