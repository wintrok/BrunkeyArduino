/*
 * DEC_HCS301.c
 *
 *  Created on: 16/11/2020
 *      Author: wintrok
 */


#include "DEC_HCS301.h"
#include "RTOS.h"
#include <Arduino.h>
#include "Config_Operador.h"

//---------------------------------------------------------------------------------------------
//    Definiciones de variables para la maquina de estados
//---------------------------------------------------------------------------------------------
unsigned char ucStat_HCS200   = Offset_Estado | ST_STANDBY;
unsigned char ucTMHOP;
unsigned char ucCNBIT;
unsigned char ucVALTE;
unsigned char ucGlitche;
stDataControl ucTemporal;
stDataDecrypt strControl_RX;
stDataDecrypt strControl_RX_Backup;
unsigned char *ucPrueba;
unsigned char ucCuentaBits;
unsigned char ucHay_Cntr = false;

#define    PIN_RF_CONTROL  digitalRead(PIN_RF)


//---------------------------------------------------------------------------------------------
//
//          Aplicación Principal de Operación
//
//---------------------------------------------------------------------------------------------

void  vDec_HCS301 (void)
{
  //Pin_Led_NegVal();
  CompararEstado(&ucStat_HCS200);
    switch(ucStat_HCS200 & ucMaskState)
    {

   // -------------------- ESTADO  ST_STANDBY  ---------------------------------------
   //
   // --------------------------------------------------------------------------------

      case ST_STANDBY:
        //if (Pin_RF == 1)
      //if(RF_GetVal()) GPIO_HAL_ReadPinInput(PTD_BASE_PTR, 7)
      if(PIN_RF_CONTROL)
        {
          CambiarEstado(&ucStat_HCS200, ST_PREAMBULO);
          ucGlitche = 0;
          ucVALTE = 0;
        }
      break;

  // -------------------- ESTADO  ST_PREAMBULO  -------------------------------------
  //
  // --------------------------------------------------------------------------------

      case ST_PREAMBULO:
        ucTMHOP = TIME_GUARD;
        //if (Pin_RF == 0)
        if(!PIN_RF_CONTROL)
        {
          ucVALTE = 0;
          CambiarEstado(&ucStat_HCS200, ST_VALIDA_HEADER);
        }
      break;

        // -------------------- ESTADO  ST_VALIDA_HEADER  -------------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_VALIDA_HEADER:
        ++ucVALTE;
        //if (Pin_RF == 1)
        if(PIN_RF_CONTROL)
        {
          ucGlitche = 0;
          CambiarEstado(&ucStat_HCS200, ST_VALIDA_UNO_HEADER);
        }
        else
        {
          if((ucVALTE > TIME_MIN_GUARD))
          {
            //asm(nop);
            CambiarEstado(&ucStat_HCS200, ST_STANDBY);
          }
        }
      break;


        // -------------------- ESTADO  ST_VALIDA_UNO_HEADER  -------------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_VALIDA_UNO_HEADER:
        ++ucVALTE;
        //if (Pin_RF == 1)
        if(PIN_RF_CONTROL)
        {
          ++ucGlitche;
          if(ucGlitche >= T_MIN_G)
          {
            CambiarEstado(&ucStat_HCS200, ST_CALCULA_HEADER);
          }
        }
        else
        {
          CambiarEstado(&ucStat_HCS200, ST_VALIDA_HEADER);
        }
      break;

        // -------------------- ESTADO  ST_CALCULA_HEADER  -------------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_CALCULA_HEADER:
        if((ucVALTE > TIME_HEADER_MIN) && (ucVALTE < TIME_HEADER_MAX))
        {
          ucTMHOP = (ucVALTE  / 10);
          //ucVALTE = (ucVALTE  % 10);
          //if(ucVALTE > 4)
          //  {
          //    ++ucTMHOP;
          //  }
          ucVALTE = ucTMHOP;
          ucVALTE = ucVALTE + (ucVALTE / 2);
          ucTMHOP = ucVALTE - 2;
          ucPrueba = &ucTemporal.Sync_Counter_LSB;
          ucCuentaBits = 0;
          ucCNBIT = 0;
          CambiarEstado(&ucStat_HCS200, ST_READ_BIT);
        }
        else
        {
          ucVALTE=4;
          CambiarEstado(&ucStat_HCS200, ST_VALIDA_HEADER);
        }
      break;

        // -------------------- ESTADO  ST_READ_BIT  -----------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_READ_BIT:
        if(--ucTMHOP == 0)
        {
         ucGlitche = 0;
         //if (Pin_RF == 0)
         if(!PIN_RF_CONTROL)
         {
           *ucPrueba = (*ucPrueba >> 1) | 0x80;
           CambiarEstado(&ucStat_HCS200, ST_SINCRONIZAR_CERO);
         }
         else
         {
           *ucPrueba = (*ucPrueba >> 1);
           CambiarEstado(&ucStat_HCS200, ST_SINCRONIZAR_UNO);
         }
         ucCNBIT++;
         ucCuentaBits++;
         if (ucCuentaBits == 8)
         {
           ucPrueba++;
           ucCuentaBits = 0;
         }
         if (ucCNBIT == (KCNBITS - 1))
         {
           CambiarEstado(&ucStat_HCS200, ST_INICIAR_COMPARE);
         }
        }
      break;

         // -------------------- ESTADO  ST_SINCRONIZAR_UNO  -----------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_SINCRONIZAR_UNO:
        ++ucGlitche;
        //if(Pin_RF == 0)
        if(!PIN_RF_CONTROL)
        {
          ucGlitche = 0;
          CambiarEstado(&ucStat_HCS200, ST_SINCRONIZAR_CERO);
        }
        else
        {
          if(ucGlitche > (ucVALTE+2))
          {
            //asm(nop);
            CambiarEstado(&ucStat_HCS200, ST_STANDBY);
          }
        }
      break;

        // -------------------- ESTADO  ST_SINCRONIZAR_CERO  ------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_SINCRONIZAR_CERO:
        ++ucGlitche;
        //if(Pin_RF == 1)
        if(PIN_RF_CONTROL)
        {
          #ifdef _USE_FREC_360Mhz_
          ucTMHOP = ucVALTE -2; // Habilitar para 360Mhz
          #endif

          #ifdef _USE_FREC_433Mhz_
          ucTMHOP = ucVALTE;
          #endif
          CambiarEstado(&ucStat_HCS200, ST_READ_BIT);
        }
        else
        {
          if(ucGlitche > (ucVALTE+2))
          {
            #ifdef _USE_FREC_360Mhz_
            ucVALTE = 4; // Habilitar para 360Mhz
            #endif

            #ifdef _USE_FREC_433Mhz_
            ucVALTE = 3;
            #endif
            CambiarEstado(&ucStat_HCS200, ST_VALIDA_HEADER);
          }
        }
      break;

        // -------------------- ESTADO  ST_HEADER_NEW  ------------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_HEADER_NEW:
          //if (Pin_RF == 0)
        if(!PIN_RF_CONTROL)
          {
            ucTMHOP--;
            //CambiarEstado(&ucStat_HCS200, ST_STANDBY);
            if (ucTMHOP == 0)
            {
              CambiarEstado(&ucStat_HCS200, ST_STANDBY);
            }
          }
          else
          {
            //if ((Pin_RF == 1) && ((TIME_GUARD - ucTMHOP) >= TIME_HEADER_MIN))
          if ((PIN_RF_CONTROL) && ((TIME_GUARD - ucTMHOP) >= TIME_HEADER_MIN))
            {
              ucCNBIT = 0;
              ucCuentaBits = 0;
              ucTMHOP = 0;
              ucPrueba = &ucTemporal.Sync_Counter_LSB;
              CambiarEstado(&ucStat_HCS200, ST_RECIBIR_BIT_NEW);
            }
            else
            {
              CambiarEstado(&ucStat_HCS200, ST_STANDBY);
            }
          }
      break;

        // -------------------- ESTADO  ST_RECIBIR_BIT_NEW  -----------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_RECIBIR_BIT_NEW:
        ucTMHOP++;
        if (ucTMHOP == VAL_SAMPLE)
        {
          ucTMHOP = 0;
          //if (Pin_RF == 0)
          if(!PIN_RF_CONTROL)
          {
            *ucPrueba = (*ucPrueba >> 1) | 0x80;
          }
          else
          {
            *ucPrueba = (*ucPrueba >> 1);
          }

          ucCNBIT++;
          ucCuentaBits++;
          if (ucCuentaBits == 8)
          {
            ucPrueba++;
            ucCuentaBits = 0;
          }
          if (ucCNBIT >= (KCNBITS - 1))
          {
            //CambiarEstado(&ucStat_HCS200, ST_STANDBY);
            CambiarEstado(&ucStat_HCS200, ST_INICIAR_COMPARE);
          }
          else
          {
            ucTMHOP = 0;
            //CambiarEstado(&ucStat_HCS200, ST_RECIBIR_BIT_NEW);
            CambiarEstado(&ucStat_HCS200, ST_SINCRONIZAR_NEW);
          }
        }
      break;

        // -------------------- ESTADO  ST_SINCRONIZAR_NEW  -----------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_SINCRONIZAR_NEW:
        ucTMHOP++;
        //if ((Pin_RF == 0) && (ucTMHOP <= 6) && (ucTMHOP > 3))
        if ((!PIN_RF_CONTROL) && (ucTMHOP <= 6) && (ucTMHOP > 3))
        {
          ucTMHOP = 0;
          //CambiarEstado(&ucStat_HCS200, ST_RECIBIR_BIT_NEW);
          CambiarEstado(&ucStat_HCS200, ST_ESPERAR_SENAL_ALTO);
        }
        else
        {
          //if ((Pin_RF == 1) && (ucTMHOP <= 4) && (ucTMHOP > 3))
          if ((PIN_RF_CONTROL) && (ucTMHOP <= 4) && (ucTMHOP > 3))
          {
            CambiarEstado(&ucStat_HCS200, ST_STANDBY);
          }
        }
      break;

        // -------------------- ESTADO  ST_INICIAR_COMPARE  -------------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_INICIAR_COMPARE:
        //if(Inicio_Estado(ucStat_HCS200))
        //{
          // Se borra la parte alta para dejar solo el numero serial
          ucTemporal.Vlow_Fixed = ucTemporal.Vlow_Fixed & 0x0F;
          //Serial.println("Se reccibio un control por RF");
          ucPrueba = (unsigned char *)&strControl_RX.Serial_RX;
          //ucPrueba = &strControl_RX.Serial_RX;
          for (ucCNBIT = 0; ucCNBIT < 4; ++ucCNBIT)
          {
            *(ucPrueba + ucCNBIT) = *(unsigned char *)(&ucTemporal.Serial_Number_SN0 + ucCNBIT);
          //  *(ucPrueba + ucCNBIT) = *(unsigned char *)(&ucTemporal.Vlow_Fixed - ucCNBIT);
          }
            Hay_Cntr;
            //Serial.print("Control Receptor: ");
            //Serial.println(strControl_RX.Serial_RX);
            memcpy(&strControl_RX.Encrypted_Data, &ucTemporal.Sync_Counter_LSB, 4);
            CambiarEstado(&ucStat_HCS200, ST_STANDBY);
        /*}
        else
        {
          if(strControl_RX_Backup.Serial_RX == strControl_RX.Serial_RX)
          {
            Hay_Cntr;
            Serial.print("Control Receptor: ");
            Serial.println(strControl_RX.Serial_RX);
            memcpy(&strControl_RX.Encrypted_Data, &ucTemporal.Sync_Counter_LSB, 4);
          }
          else
          {
            strControl_RX_Backup.Serial_RX = strControl_RX.Serial_RX;
          }
          //Hay_Cntr;
          //Serial.print("Control Receptor: ");
          //Serial.println(strControl_RX.Serial_RX);
          //memcpy(&strControl_RX.Encrypted_Data, &ucTemporal.Sync_Counter_LSB, 4);
          CambiarEstado(&ucStat_HCS200, ST_STANDBY);
        }*/
      break;

        // -------------------- ESTADO  ST_ESPERAR_SENAL_ALTO  ----------------------------
        //
        // --------------------------------------------------------------------------------

      case ST_ESPERAR_SENAL_ALTO:
        //if (Pin_RF == 1)
      if(PIN_RF_CONTROL)
        {
          ucTMHOP = 0;
          CambiarEstado(&ucStat_HCS200, ST_RECIBIR_BIT_NEW);
        }
      break;

      default:
      CambiarEstado(&ucStat_HCS200, ST_STANDBY);
      break;

      }
}
