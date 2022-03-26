/*
 * Driver_Led.c
 *
 *  Created on: 24/01/2021
 *      Author: wintrok
 *      
 *  Estados del Led    
 *  
 *  Encendido Permanentemente --> No esta conectado a una red Wifi
 *  Toggle Rapido --> Esta conectado a una red Wifi Enciende cada 0.5 Seg.
 *  Toggle Lento  --> Esta conectado al Servidor MQTT Enciende cada 3 Seg.
 *  
 */

#include "Driver_Led.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "DriverMQTT.h"
#include "RTOS.h"
#include "Config_Operador.h"

#define LED_ON      digitalWrite(PIN_LED, LOW)
#define LED_OFF     digitalWrite(PIN_LED, HIGH)
#define LED_TOGGLE  digitalWrite(PIN_LED, !digitalRead(PIN_LED))

/******************************************
 * Definicion de los estados del programa *
 *****************************************/
enum Select_State_Driver_Led
{
   ST_INIT_DRIVER_LED = 0,
   ST_VERIFICA_RED_WIFI,
   ST_SECUENCIA_WIFI,
   ST_VERIFICA_SERVIDOR_MQTT,
};

unsigned char ucEstado_Driver_Led = Offset_Estado | ST_INIT_DRIVER_LED;


unsigned long ulTime_Led;

extern strMQTT Flags_MQTT;


/*************************************
 * PROCEDIMIENTO PARA MANEJO DEL LED *
 *************************************/
void Driver_Led(void)
{
    
    CompararEstado(&ucEstado_Driver_Led);
    switch(ucEstado_Driver_Led & ucMaskState)
    {

      /****************************
      * Estado inicial del driver *
      ****************************/
      case ST_INIT_DRIVER_LED:
        if(Inicio_Estado(ucEstado_Driver_Led))
        {
          Reiniciar_Temporizador(&ulTime_Led);
        }
        else
        {
          if(Retorna_Temporizador(DELAY_INIT_DRIVER_LED, Time_100mS, &ulTime_Led))
          {
            CambiarEstado(&ucEstado_Driver_Led, ST_VERIFICA_RED_WIFI);
          }
        }
      break;     

      /*************************************************************
      * Estado encargado de verificar la conexion con una red Wifi *
      **************************************************************/
      case ST_VERIFICA_RED_WIFI:
        if(WiFi.status() == WL_CONNECTED)
        {
          if(ConectadoServMQTT)
          {
            LED_OFF;
            CambiarEstado(&ucEstado_Driver_Led, ST_SECUENCIA_WIFI);
          }
          else
          {
            LED_ON;  
          }
        }
        else
        {
          LED_ON;  
        }
      break;

      /***********************************************************************
      * Estado donde se realiza la secuencia del Led conectado a la red Wifi *
      ************************************************************************/
      case ST_SECUENCIA_WIFI:
        if(Inicio_Estado(ucEstado_Driver_Led))
        {
          Reiniciar_Temporizador(&ulTime_Led);
          LED_OFF;
        }
        else
        {
          if(Retorna_Temporizador(TIME_TOGGLE_LENTO, Time_100mS, &ulTime_Led))
          {
            LED_TOGGLE;
            CambiarEstado(&ucEstado_Driver_Led, ST_VERIFICA_RED_WIFI);
          }
        }
      break;     



      /*****************************************************************
      * Estado encargado de verificar la conexion con el servidor MQTT *
      ******************************************************************/
      case ST_VERIFICA_SERVIDOR_MQTT:
        if(ConectadoServMQTT)
        {
          CambiarEstado(&ucEstado_Driver_Led, ST_VERIFICA_SERVIDOR_MQTT);
        }
        else
        {
          LED_ON;  
        }
      break;



    }
}
