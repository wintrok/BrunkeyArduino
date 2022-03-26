/*
 * Recepcion_RF.h
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */

#ifndef SOURCES_RECEPCION_RF_H_
#define SOURCES_RECEPCION_RF_H_

#include  "RTOS.h"


/* Definiciones para la aplicacion*/


#define NUM_MAX_CONTROLES      200

#define DIR_FLASH_CONTROL     (0x0100)  // Posicion de memoria donde se almacenan los controles

/* Definicion constantes para la aplicacion*/

#define   Es_Hopping            0x01
#define   Es_Calculo_Llave      0x02
#define   Es_Fin_Hopping        0x03

#define   Descifrar_Hopping     ucHacer_Hop = Es_Hopping
#define   Calcular_Llave        ucHacer_Hop = Es_Calculo_Llave
#define   Fin_Proceso_Hopping   ucHacer_Hop = Es_Fin_Hopping

#define   Es_Control            0x00
#define   Es_sensor             0x01

/*Declaracion de las Direcciones de la E2PROM de Configuracion*/


/*Declaracion de los Estados de la Aplicacion*/

enum State_Recepcion_RF
{
   ST_ESPERA_CONTROL_RF,
   ST_VERIFICA_CONTROL_RF,
   ST_VERIFICA_HABILITADOR_APTO,
   ST_DESCIFRAR_APTO_RF,
   ST_ESPERA_DESCIFRAR_RF,
   ST_ESPERA_PROCESAR_CONTROL,
   ST_VERIFICAR_CODIGO_CONTROL,
   ST_VERIFICAR_CODIGO_SENSOR,
   ST_GRABAR_LLAVE_APTO_RF,
   ST_NO_APTO_DISPONIBLE_RF,
   ST_ESPERA_CALCULO_LLAVES
};






/* Definicion de estructuras utilizadas en la aplicacion*/


typedef struct
{
  unsigned long  Cod_Control[NUM_MAX_CONTROLES];
}Controls;








/* Definicion de funciones utilizadas en la aplicacion*/

void Recepcion_RF(void);      //Maquina de estados principal




#endif /* SOURCES_RECEPCION_RF_H_ */
