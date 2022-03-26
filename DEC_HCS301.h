/*
 * DEC_HCS301.h
 *
 *  Created on: 16/11/2020
 *      Author: wintrok
 */

#ifndef SOURCES_DEC_HCS301_H_
#define SOURCES_DEC_HCS301_H_



#define _USE_FREC_433Mhz_

/* Definiciones de la Aplicacion */

#define   Hay_Cntr  ucHay_Cntr = true
#define   Sin_Cntr  ucHay_Cntr = false


/* Definiciones para la Recepcion */


#define   KHEADER     28
#define   KGUARDA     62
#define   KAJUSTE     00
#define   KCNBITS     65
#define   KDATOS      19


#define   TIME_GUARD        160
#define   TIME_MIN_GUARD    100
#define   TIME_HEADER_MIN   20
#define   TIME_HEADER_MAX   66
#define   VAL_SAMPLE        4
#define   T_MIN_G           1


enum State_MainHCS200
{
   ST_STANDBY = 1,
   ST_PREAMBULO,
   ST_VALIDA_HEADER,
   ST_VALIDA_UNO_HEADER,
   ST_CALCULA_HEADER,
   ST_READ_BIT,
   ST_SINCRONIZAR_UNO,
   ST_SINCRONIZAR_CERO,
   ST_CALIBRACION,
   ST_HEADER,
   ST_ENCABEZADO,
   ST_RECIBIR_BIT,
   ST_CHEQUEAR,
   ST_LEER_BIT,
   ST_SINCRONIZAR,
   ST_GUARDA,
   ST_INICIAR_COMPARE,
   ST_HEADER_NEW,
   //ST_LEER_BIT_NEW,
   ST_RECIBIR_BIT_NEW,
   ST_SINCRONIZAR_NEW,
   ST_ESPERAR_SENAL_ALTO
 };


typedef struct
{
   unsigned char  Sync_Counter_LSB;
   unsigned char  Sync_Counter_MSB;
   unsigned char  Discrimination_LSB;
   unsigned char  Discrimination_MSB;
   unsigned char  Serial_Number_SN0;
   unsigned char  Serial_Number_SN1;
   unsigned char  Serial_Number_SN2;
   unsigned char  Vlow_Fixed;
}stDataControl;

typedef struct
{
   unsigned long  Serial_RX;
   unsigned long  Encrypted_Data;
   unsigned char  Key_control[8];
}stDataDecrypt;



void vDec_HCS301 (void);


#endif /* SOURCES_DEC_HCS301_H_ */
