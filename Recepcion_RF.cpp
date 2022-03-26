/*
 * Recepcion_RF.c
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */
#include <Arduino.h>
#include "Recepcion_RF.h"
#include "RTOS.h"
#include "DEC_HCS301.h"
#include "Calc_Key.h"
#include "Crear_Llaves.h"
//#include "I2C_Soft.h"
//#include "Aplicacion.h"
//#include "ApplInicio.h"
#include "string.h"
//#include "Memoria_Sistema.h"

/* Se define que llaves se van a usar si de pruebas o de produccion */

#define _VERSION_PRODUCCION_


#ifdef _VERSION_PRUEBAS_
//const uchar arrLlave_Maestra[8] = {0x2D,0x36,0xF3,0x09,0xC1,0xB5,0x74,0x8A};
#endif

#ifdef _VERSION_PRODUCCION_
//const uchar arrLlave_Maestra[8] = {0x50,0x41,0x4E,0x49,0x43,0x4F,0x53,0x31};
//const uchar arrLlave_Maestra[8] = {0x31,0x25,0x47,0x72,0x88,0x52,0x69,0xAC}; // LLAVE COLSECURITY
//const unsigned char arrLlave_Maestra[8] = {0xAC,0x69,0x52,0x88,0x72,0x47,0x25,0x31}; // LLAVE COLSECURITY INVERTIDA
//const uchar arrLlave_Maestra[8] = {0x68,0xEE,0x7F,0x89,0x09,0x23,0x7F,0x76}; // LLAVE JHON PANICOS
//const unsigned char arrLlave_Maestra[8] = {0x79,0x8F,0x5C,0xAA,0x82,0x05,0x39,0xBD}; // LLAVE INTEGRA
//const unsigned char arrLlave_Maestra[8] = {0xAA,0x35,0x67,0x98,0xAB,0x5C,0xD0,0x8F}; // LLAVE TELESENTINEL
//const unsigned char arrLlave_Maestra[8] = {0x57,0x4B,0x2D,0x33,0x36,0x30,0x4D,0x48}; // LLAVE OPERADOR JHON
//const unsigned char arrLlave_Maestra[8] = {0x48,0x4D,0x30,0x36,0x33,0x2D,0x4B,0x57}; // LLAVE OPERADOR JHON INVERTIDA
//const uchar arrLlave_Maestra[8] = {0x56,0x98,0xAD,0xEF,0x12,0x47,0x01,0xEC}; // LLAVE VISE
#endif


//const uchar arrLlave_Control[8] = {0x7D,0x1C,0xD8,0x33,0x49,0x88,0xBA,0x04};


/* Definicion variables para la Aplicacion*/

extern unsigned char ucHay_Cntr;
extern stDataDecrypt strControl_RX;
extern stRtaDecrypt Rta_Decrypt;
extern StrFlags_Llaves ucHab_Crear_Llave;
//extern strPanico Flags_Panico;

Controls *strControls;

unsigned char * ptrBackupControl;
unsigned char j;
unsigned char uiLlaves_Control[8];
unsigned char ucRta_Hop[4];
unsigned char ucHacer_Hop = 0;
unsigned long ulBackupSerial;
unsigned long ulSerial_Recibido;
unsigned long ulBackupCifrado;
unsigned char ucHay_Control;
unsigned char ucHay_Accion = false;
unsigned char ucFlag_Aprendizaje = false;

unsigned char State_Recepcion_RF = Offset_Estado | ST_ESPERA_CONTROL_RF;

unsigned long ulCod_Control_Mem;

// Desarrollo de la maquina de estados

void Recepcion_RF(void)
{
   //unsigned char *ptrAux1;
   //unsigned char *ptrAux2;
   unsigned char j;

   CompararEstado(&State_Recepcion_RF);
   switch(State_Recepcion_RF & ucMaskState)
   {

      //*******************************************************************************
      // Estado encargado de esperar la recepcion de un control via RF
      //
      //*******************************************************************************

      case ST_ESPERA_CONTROL_RF:
    //if(ucHay_Cntr && !ucHay_Accion && !Flags_Panico.En_Panico)
    if(ucHay_Cntr)
    {
      //if(strControl_RX.Serial_RX  < 0x00FFFFFF)
      //{
      // Se realizan dos copias del numero serial
      memcpy(&ucRta_Hop, &strControl_RX.Serial_RX,4);

      memcpy(&ulBackupSerial, &strControl_RX.Serial_RX,4);

      // Se procede a calcular la llave para este control
      Calcular_Llave;
      CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CALCULO_LLAVES);
      /*}
      else
      {
        Sin_Cntr;
        CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
      }*/
    }
    else
    {
      if(ucHab_Crear_Llave.Habilitar_Generacion)
      {
        // Debe ser generadas las llaves para un Apto.
        Calcular_Llave;
        CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CALCULO_LLAVES);
      }
      else
      {
        CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
      }
    }
      break;

      //*******************************************************************************
      // Estado encargado de esperar que termine el proceso de descifrado y asi validar
      // el Hopping con la parte baja del codigo fijo (Apartamento)
      // Se verifica si es un control o un sensor
      //*******************************************************************************

      case ST_ESPERA_DESCIFRAR_RF:
        // Se espera a que se finalice de procesar el Hopping
        if(ucHacer_Hop == Es_Fin_Hopping)
        {
          // Es habilitada nuevamente la recepcion de controles
          Sin_Cntr;
          // Se verifica la validez de los datos descifrados
          if(Rta_Decrypt.Verif_Decrypt == (ulBackupSerial & 0x03FF))
          {
            Serial.print("Codigo Control recibido: ");
            Serial.println(ulBackupSerial);
            //memcpy(&ulBackupCifrado, &strControl_RX.Encrypted_Data,4);
        // Es verificado si el serial ya fue programado
          /*Read_data_mem((unsigned char*)&ulCod_Control_Mem, DIR_FLASH_CONTROL, 1, DIR_I2C_MEM_DISP);
          strControls = (Controls*)DIR_FLASH_CONTROL;
          for(j=0; j < NUM_MAX_CONTROLES; j++)
        {
          if(strControls->Cod_Control[j] == strControl_RX.Serial_RX)
          {
            // El control recibido ya fue programado
            break;
          }
        }
        if(j == NUM_MAX_CONTROLES)
        {
          // El control recibido no se encuentra almacenado
              // Verifica si se encuentra en aprendizaje de controles
                //ucHay_Control = TRUE;
                ulSerial_Recibido = ulBackupSerial;
                if(ucFlag_Aprendizaje)
                {
                  //ucHay_Control = TRUE;
                  ucFlag_Aprendizaje = false;
                  CambiarEstado(&State_Recepcion_RF, ST_ESPERA_PROCESAR_CONTROL);
                }
                else
                {
                  // El control no pertenece a este operador
                  CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
                }
        }
        else
        {
          // El control se encuentra en memoria
          if(ucFlag_Aprendizaje)
          {
            // Esta en aprendizaje, se ignora el control
            CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
          }
          else
          {
            // Se recibio un control autorizado para este operador
            ucHay_Accion = true;
                    ulSerial_Recibido = ulBackupSerial;
                    CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
          }
        }*/
          // Verifica si se encuentra en aprendizaje de controles
            ucHay_Control = true;
            ulSerial_Recibido = ulBackupSerial;
            /*if(ucFlag_Aprendizaje)
            {
              ucFlag_Aprendizaje = FALSE;
              CambiarEstado(&State_Recepcion_RF, ST_ESPERA_PROCESAR_CONTROL);
            }
            else
            {
              CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
            }*/

            CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
          }
          else
          {
            // Al descifrar el hopping los datos obtenidos no son validos
            //asm(nop);
            CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
          }
        }
      break;

      //*******************************************************************************
      // Estado encargado de esperar que sea procesado el control recibido
      //
      //*******************************************************************************

      case ST_ESPERA_PROCESAR_CONTROL:
        if(!ucHay_Cntr)
        {
          //asm (nop);
          // Se obliga a esperar una nueva recepcion

          CambiarEstado(&State_Recepcion_RF, ST_ESPERA_CONTROL_RF);
        }
      break;

      //*******************************************************************************
      // Estado encargado de esperar que se generen las llaves solicitadas
      //
      //*******************************************************************************

      case ST_ESPERA_CALCULO_LLAVES:
        // Se espera a que se finalice de procesar el Hopping
        if(ucHacer_Hop == Es_Fin_Hopping)
        {
          ucHab_Crear_Llave.Habilitar_Generacion = 0;
          // Se habilita la realizacion del desciframiento del hopping
          Descifrar_Hopping;
          CambiarEstado(&State_Recepcion_RF, ST_ESPERA_DESCIFRAR_RF);
        }
      break;
   }
}



//**********************************************************************
//              FUNCIONES UTILIZADAS EN LA APLICACION
//**********************************************************************
