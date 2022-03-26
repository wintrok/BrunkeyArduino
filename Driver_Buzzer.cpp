/*
 * Driver_Buzzer.c
 *
 *  Created on: 25/01/2021
 *      Author: wintrok
 *      
 *  Estados del Buzzer    
 *  
 *  
 */

#include "Driver_Buzzer.h"
#include <Arduino.h>
#include "RTOS.h"

/******************************************
 * Definicion de los estados del programa *
 *****************************************/
enum Select_State_Driver_Buzzer
{
   ST_INIT_DRIVER_BUZZER = 0,
   ST_ESPERA_SECUENCIA_BUZZER,
   ST_HACER_SEC_ACTIVACION,
};





unsigned char ucEstado_Driver_Buzzer = Offset_Estado | ST_INIT_DRIVER_BUZZER;


unsigned long ulTime_Buzzer;
unsigned char ucSec_Buzzer;

unsigned char ucAuxa_Buzzer;
unsigned long ulCnt_Time_Buzzer;

/****************************************
 * PROCEDIMIENTO PARA MANEJO DEL BUZZER *
 ****************************************/
void Driver_Buzzer(void)
{
    
    CompararEstado(&ucEstado_Driver_Buzzer);
    switch(ucEstado_Driver_Buzzer & ucMaskState)
    {

      /****************************
      * Estado inicial del driver *
      ****************************/
      case ST_INIT_DRIVER_BUZZER:
        if(Inicio_Estado(ucEstado_Driver_Buzzer))
        {
          Reiniciar_Temporizador(&ulTime_Buzzer);
        }
        else
        {
          if(Retorna_Temporizador(DELAY_INIT_DRIVER_BUZZER, Time_100mS, &ulTime_Buzzer))
          {
            CambiarEstado(&ucEstado_Driver_Buzzer, ST_ESPERA_SECUENCIA_BUZZER);
          }
        }
      break;     

      /*************************************************************************
      * Estado encargado de verificar la funcion que esta ejecutando el Buzzer *
      **************************************************************************/
      case ST_ESPERA_SECUENCIA_BUZZER:
        switch(ucSec_Buzzer)
        {
          // Comprueba si se debe generar Buzzer para activacion de alarma
          case EVENTO_GENERAR_BUZZER_ACTIVADA:
            CambiarEstado(&ucEstado_Driver_Buzzer, ST_HACER_SEC_ACTIVACION);
          break;
          
          // Comprueba si se debe generar Buzzer para desactivacion de alarma
          /*case EVENTO_GENERAR_BUZZER_DESACTIVADA:
            CambiarEstado(&State_Buzzer, ST_HACER_SEC_DESACTIVACION);
          break;   
          
          // Comprueba si se debe generar Buzzer para Aceptacion
          case EVENTO_GENERAR_BUZZER_ACEPTACION:
            CambiarEstado(&State_Buzzer, ST_HACER_SEC_ACEPTACION);
          break;   
          
          // Comprueba si se debe generar Buzzer para Tick
          case EVENTO_GENERAR_BUZZER_TICK:
            CambiarEstado(&State_Buzzer, ST_HACER_SEC_TICK);
          break;        */       
          
          // Si el estado no se encuentra definido carga el estado por defecto
          default:
            //BUZZER_OFF;
            CambiarEstado(&ucEstado_Driver_Buzzer, ST_ESPERA_SECUENCIA_BUZZER);
          break;     
          
        }




      break;

      //*******************************************************************************
      // Estado encargado de realizar la secuencia de Activacion de alarma
      //
      //*******************************************************************************

      case ST_HACER_SEC_ACTIVACION:
        if(Inicio_Estado(ucEstado_Driver_Buzzer)) 
        {
          BUZZER_ON;
          ucAuxa_Buzzer = 0;
          Reiniciar_Temporizador(&ulCnt_Time_Buzzer);
        }
        else
        {
          if((Retorna_Temporizador(TIME_TOGGLE_BUZZER,Time_50mS, &ulCnt_Time_Buzzer)))
          {
            ucAuxa_Buzzer++;
            //BUZZER_TOGGLE_ON;
            digitalWrite(14, !digitalRead(14));
            if(ucAuxa_Buzzer == (NUM_PITOS_ACTIVAR * 2))
            {
              BUZZER_OFF;
              ucSec_Buzzer = EVENTO_APAGAR_SECUENCIA_BUZZER;
              CambiarEstado(&ucEstado_Driver_Buzzer, ST_ESPERA_SECUENCIA_BUZZER);
            }
          }
        }
      break;




      
    }
}
