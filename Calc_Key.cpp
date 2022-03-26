/*
 * Calc_Key.c
 *
 *  Created on: 17/06/2016
 *      Author: Jorge
 */

#include "Calc_Key.h"
#include "RTOS.h"
#include "Recepcion_RF.h"
#include "DEC_HCS301.h"
#include "string.h"

extern unsigned char ucHacer_Hop;
extern stDataDecrypt strControl_RX;
//extern const unsigned char arrLlave_Maestra[8];
const unsigned char arrLlave_Maestra[8] = {0x48,0x4D,0x30,0x36,0x33,0x2D,0x4B,0x57}; // LLAVE OPERADOR JHON INVERTIDA

unsigned char State_Calc_Key = Offset_Estado | ST_ESPERA_ACTIVACION_CALC_KEY;
stRtaDecrypt Rta_Decrypt;
stHop strHOP;
stKey strKEY;
unsigned char Flag_Hopping;
unsigned char ucCnt0;
unsigned char ucCnt1;
unsigned char ucCnt2;
unsigned char ucMask;
unsigned char ucACC;
unsigned long ulHOP;
unsigned long long udKEY;

unsigned long *ptrlong;
unsigned long long *ptrdouble;

void Calculo_Hopping(void)
{
  unsigned char * ptrAux;
  unsigned char * ptrAuxA;
  unsigned char * ptrAuxB;
  unsigned char BuffHop[4], AuxKeyBuff[8];
  unsigned char ucCnta;
   CompararEstado(&State_Calc_Key);

   switch(State_Calc_Key & ucMaskState)
   {

      //*******************************************************************************
      // Estado encargado de esperar la activacion del proceso
      //
      //*******************************************************************************

      case ST_ESPERA_ACTIVACION_CALC_KEY:
        // Se verifica si se debe realizar el deciframiento del hopping
        if(ucHacer_Hop == Es_Hopping)
          {
            CambiarEstado(&State_Calc_Key, ST_INICIAR_DESCIFRAR_HOPPING);
          }
         else
         {
           // Se verifica si se debe realizar el calculo de la llave para el control
           if(ucHacer_Hop == Es_Calculo_Llave)
           {
             Flag_Hopping = true;
             CambiarEstado(&State_Calc_Key, ST_INICIAR_GENERACION_LLAVE);
           }
           else
           {
             CambiarEstado(&State_Calc_Key, ST_ESPERA_ACTIVACION_CALC_KEY);
           }
         }
      break;

      //*******************************************************************************
      // Estado encargado de dar las condiciones iniciales para descifrar el Hopping
      // Tiempo de ejecucion maximo: 30,8uS
      //*******************************************************************************

      case ST_INICIAR_DESCIFRAR_HOPPING:
        ucCnt1 = 23;
        ucCnt0 = 8;
        ucCnt2 = 3;
        //memcpy(&strHOP, &strControl_RX.Encrypted_Data, 4);
        ptrAuxA = (unsigned char*)&strHOP;
        ptrAuxB = (unsigned char*)&strControl_RX.Encrypted_Data;
        *(ptrAuxA + 3) = *ptrAuxB;
        *(ptrAuxA + 2) = *(ptrAuxB + 1);
        *(ptrAuxA + 1) = *(ptrAuxB + 2);
        *ptrAuxA = *(ptrAuxB + 3);
        memcpy(&strKEY, &strControl_RX.Key_control, 8);
        CambiarEstado(&State_Calc_Key, ST_REALIZAR_DECRYPT);
      break;

      //*******************************************************************************
      // Estado encargado de Inicializar la generacion de la llave para el control
      // recibido...
      // El codigo del control se debe cargar en strControl_RX.Encrypted_Data
      // Las llaves se regresaran en strControl_RX.Key_control
      //*******************************************************************************

      case ST_INICIAR_GENERACION_LLAVE:
        ucCnt1 = 23;
        ucCnt0 = 8;
        ucCnt2 = 3;
        //memcpy(&strHOP.ucHOP1, &strControl_RX.Serial_RX, 4);
        ptrAux = (unsigned char*)&strControl_RX.Serial_RX;
        strHOP.ucHOP4 = *ptrAux;
        strHOP.ucHOP3 = *(ptrAux + 1);
        strHOP.ucHOP2 = *(ptrAux + 2);
        strHOP.ucHOP1 = *(ptrAux + 3);
        strHOP.ucHOP1 |= 0x20;
        if(!Flag_Hopping)
        {
          strHOP.ucHOP1 |= 0x60;
        }
        memcpy(&strKEY.ucKEY0, &arrLlave_Maestra, 8);
        CambiarEstado(&State_Calc_Key, ST_REALIZAR_DECRYPT);
      break;

      //*******************************************************************************
      // Estado encargado de realizar el proceso de Hopping
      //
      //*******************************************************************************

      case ST_REALIZAR_DECRYPT:
        DECRYPT_INNER:
          //PTJD = 0x80;
          if(ucCnt1 == 1)
          {
            goto ROTATE_KEY;
          }
          if(strHOP.ucHOP2 & 0x08)
          {
            ucMask = 0x10;
            goto SALTO_1;
          }
          else
          {
            ucMask = 0x01;
          }
        SALTO_1:
          if(strHOP.ucHOP3 & 0x01)
          {
            ucMask = ucMask << 2;
          }
        SALTO_2:
          if(strHOP.ucHOP4 & 0x01)
          {
            ucMask = ucMask << 1;
          }
        SALTO_3:
          if(strHOP.ucHOP1 & 0x02)
          {
            ucACC = 0x3A;
            goto SALTO_31;
          }
          if(strHOP.ucHOP1 & 0x40)
          {
            ucACC = 0x5C;
            goto SALTO_4;
          }
          ucACC = 0x2E;
          goto SALTO_4;
        SALTO_31:
          if(strHOP.ucHOP1 & 0x40)
          {
            goto SALTO_4;
          }
          ucACC = 0x74;
          goto SALTO_4;
        SALTO_4:
          if(!(ucACC & ucMask))
          {
            ucACC = 0;
            goto SALTO_6;
          }
          goto SALTO_5;
        SALTO_5:
          ucACC = 0x80;
        SALTO_6:
      ucACC ^= strHOP.ucHOP3;
      ucACC ^= strHOP.ucHOP1;
      ucACC ^= strKEY.ucKEY6;
      ucMask = ucACC;

      ptrlong = &ulHOP;
      //ptrlong = (unsigned long*)&strHOP.ucHOP4;
      *ptrlong = *(unsigned long*)&strHOP;
      ptrAux = (unsigned char *)&ulHOP;

      for (ucCnta = 0; ucCnta < 4; ++ucCnta)
      {
        *(ptrAux + ucCnta) = *(unsigned char *)(&strHOP.ucHOP4 - ucCnta);
      }
      ulHOP = ulHOP << 1;
      memcpy(BuffHop, &ulHOP, 4);
      ptrAuxA = &strHOP.ucHOP1;
      ptrAuxB = &BuffHop[3];
      for (ucCnta = 0; ucCnta < 4; ++ucCnta)
      {
        *(ptrAuxA + ucCnta) = *(ptrAuxB - ucCnta);
      }

      //memcpy(&strHOP.ucHOP1, &ulHOP, 4);
      if(ucMask & 0x80)
      {
        strHOP.ucHOP4 |= 0x1;
      }

        ROTATE_KEY:
            ucACC = strKEY.ucKEY0;

            ptrAuxB = &strKEY.ucKEY0;
      ptrAuxA = &AuxKeyBuff[7];
      for (ucCnta = 0; ucCnta < 8; ++ucCnta)
      {
        *(ptrAuxA - ucCnta) = *(ptrAuxB + ucCnta);
      }

          ptrdouble = &udKEY;
      *ptrdouble = *(unsigned long long*)&AuxKeyBuff;
      udKEY = udKEY << 1;
          //memcpy(&strKEY.ucKEY0, &udKEY, 8);
          memcpy(AuxKeyBuff, &udKEY, 8);
          ptrAuxB = &strKEY.ucKEY0;
      ptrAuxA = &AuxKeyBuff[7];
      for (ucCnta = 0; ucCnta < 8; ++ucCnta)
      {
        *(ptrAuxB + ucCnta) = *(ptrAuxA - ucCnta);
      }

      if(ucCnt1 != 1)
          {
            if(ucACC & 0x80)
            {
              strKEY.ucKEY7 |= 0x1;
            }
          }
          if(!--ucCnt2)
          {
            if(!--ucCnt0)
            {
              if(!--ucCnt1)
              {
                //PTJD = 0x00;
                // Se ha terminado de realizar el hopping
                if(ucHacer_Hop == Es_Hopping)
                {
                  Fin_Proceso_Hopping;
                  Rta_Decrypt.Cont_Sync = (unsigned char)strHOP.ucHOP3;
                  Rta_Decrypt.Cont_Sync = (unsigned char)((Rta_Decrypt.Cont_Sync << 8) | strHOP.ucHOP4);
                  ptrAux = (unsigned char *)&Rta_Decrypt.Cont_Sync;
                  *(ptrAux + 3) = (unsigned char)strHOP.ucHOP1;
                  *(ptrAux + 2) = (unsigned char)strHOP.ucHOP2;
                  CambiarEstado(&State_Calc_Key, ST_ESPERA_ACTIVACION_CALC_KEY);
                }
                else
                {
                  if(Flag_Hopping)
                  {
                    // Se genero la primera parte de la llave
                    Flag_Hopping = false;
                    memcpy(&strControl_RX.Key_control[4], &strHOP, 4);
                    CambiarEstado(&State_Calc_Key, ST_INICIAR_GENERACION_LLAVE);
                  }
                  else
                  {
                    // Se termino de generar la llave
                    Fin_Proceso_Hopping;
                    memcpy(&strControl_RX.Key_control, &strHOP, 4);
                    CambiarEstado(&State_Calc_Key, ST_ESPERA_ACTIVACION_CALC_KEY);
                  }
                }
              }
              else
              {
                //PTJD = 0x00;
                ucCnt0 = 8;
                ucCnt2 = 3;
              }
            }
            else
            {
              //PTJD = 0x00;
              ucCnt2 = 3;
              goto DECRYPT_INNER;
            }
          }
          else
          {
            goto DECRYPT_INNER;
          }
      break;



   }
}



//**********************************************************************
//              FUNCIONES UTILIZADAS EN LA APLICACION
//**********************************************************************
