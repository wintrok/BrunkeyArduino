/*
 * Config_Operador.h
 *
 *  Created on: 10/03/2021
 *      Author: Jorge
 */

#ifndef SOURCES_CONFIG_OPERADOR_H_
#define SOURCES_CONFIG_OPERADOR_H_

#define Wintrok
//#define Abcontrol

/***********************
 * DEFINICION DE PINES *
 ***********************/
#define   PIN_LED       4
#define   PIN_RF        5
#define   PIN_RELEVO    12
#define   PIN_PUERTA    13

/* Definiciones de constantes */

/* DEFINICION DEL TOPICO UTILIZADO PARA LA COMUNICACION DE LOS DISPOSITIVOS */
//const char Topico_Aplicacion[] = "wintrok/operador";
const char Topico_Aplicacion[] = "brunkeybox/operativo";
//const char Topico_Aplicacion[] = "halcom/bodega";
//const char Topico_Aplicacion[] = "devices/10001;
/* DEFINICION DEL ID CON QUE SE VISUALIZA EN LA DASHBOARD */
#ifdef Wintrok
  //const String Client_ID = "OperadorWintrok";
  //const char Client_ID[] = "9999FFFF";
  const char Client_ID[] = "223456789";
  //const char Client_ID[] = "d1399d3d";
  
  const char User_Name[] = "wintrok";
#else 
  #ifdef Abcontrol
    const String Client_ID = "OperadorAbcontrol;
  #endif
#endif

/*Definicion de la Ip y puerto del Servidor a utilizar*/

//const char DirServidor_Operador[] = "142.93.88.99";
const char DirServidor_Operador[] = "165.232.157.235";
#define Puerto_Operador 1883
/* Definicion estructuras para la aplicacion*/







#endif /* SOURCES_CONFIG_OPERADOR_H_ */
