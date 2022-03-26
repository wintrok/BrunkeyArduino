/*
   RTOS.cpp

    Created on: 12/06/2020
        Author: Wintrok Ingenier�a
*/

#include "RTOS.h"
#include <Arduino.h>


unsigned char   FST_Task_cnt;       // contador de tareas rapidas
unsigned char   MID_Task_cnt;       // contador de tareas medias
unsigned char   SLO_Task_cnt;       // contador de tareas lentas
unsigned char   INI_Task_cnt;       // contador de tareas iniciales
unsigned char   clockreg;
unsigned char   cont1, cont2, cont3, cont4;

stFlagsRTOS Flags_RTOS;

void    rtos(const vFuncPtrV * ptrTaskList) {
  //----------------------------- TAREAS RAPIDAS ----------------------------------
  for (FST_Task_cnt = FIRST_FST_TASK; FST_Task_cnt < FIRST_MID_TASK; FST_Task_cnt++)
    ptrTaskList[FST_Task_cnt]();
  //----------------------------- TAREAS MEDIAS -----------------------------------
  if (MID_Task_cnt < FIRST_SLO_TASK)
  {
    ptrTaskList[MID_Task_cnt++]();
  }
  else
  {
    //----------------------------- TAREAS LENTAS -----------------------------------
    if (SLO_Task_cnt >= n_GLOBAL_TASKS)
    {
      SLO_Task_cnt = FIRST_SLO_TASK;
    }
    ptrTaskList[SLO_Task_cnt++]();
    MID_Task_cnt = FIRST_MID_TASK;
  }
}

/****************************************************
   Muestreador de Tareas - TICKER
   Temporizador que se encarga de cambiar de tareas
 ****************************************************/
void Clock_ticker(void) {
  while (!Flags_RTOS.Flag_Ticker);
  Flags_RTOS.Flag_Ticker = false;
}

/**********************************************
   Tarea falsa para ajustar sistema operativo
 **********************************************/
void TaskDummy(void) {
  __asm("nop");
}

/*********************************************************
   Manejador de bases de tiempo
   Se encarga de generar Flancos de tiempo para que sean
   utilizados por las tareas lentas que lo soliciten.
 *********************************************************/
void Bases_Tiempo(void) {
  clockreg &= 0b11110000;
  //digitalWrite(LED, !(digitalRead(LED)));
  if (!--cont1)
  {
    cont1 = Val_50mS;
    b_SetBit(Time_50mS, clockreg);
    //digitalWrite(LED, !(digitalRead(LED)));
    if (!--cont2)
    {
      cont2 = Val_100mS;
      b_SetBit(Time_100mS, clockreg);
      if (!--cont3)
      {
        cont3 = Val_500mS;
        b_SetBit(Time_500mS, clockreg);
        if (!--cont4)
        {
          cont4 = Val_Segundo;
          b_SetBit(Time_Segundo, clockreg);
        }
      }
    }
  }
}

/*****************************************
   Inicializacion de las bases de tiempo
 *****************************************/
void Init_Bases_Tiempo(void) {
  cont1 = Val_50mS;
  cont2 = Val_100mS;
  cont3 = Val_500mS;
  cont4 = Val_Segundo;
}

/********************************************
   Funcion para verificar los bits de tiempo
 ********************************************/
unsigned char   Retorna_Tiempo(unsigned char bit) {
  return b_TestBit(bit, clockreg);
}

/***************************************************
   Funcion utilizada para llevar periodos de tiempo
 ***************************************************/
unsigned char Retorna_Temporizador(unsigned long cantidad_tiempo, unsigned char Base_Tiempo, unsigned long * contador_tiempo) {
  if (Retorna_Tiempo(Base_Tiempo))
    *contador_tiempo = *contador_tiempo + 1;
  if (*contador_tiempo >= cantidad_tiempo)
  {
    *contador_tiempo = 0;
    return true;
  }
  else
    return false;
}

/*****************************************
   Funcion para inicializar temporizador
 *****************************************/
void Reiniciar_Temporizador(unsigned long *contador_tiempo)
{
  *contador_tiempo = 0;
}

/****************************************************************************
   Revisa estado actual de cualquier aplicacion con solo examinar bit 7 y 6
 * **************************************************************************/
void CompararEstado(unsigned char * estado)
{
  b_ClearBit(7, * estado);

  if (b_TestBit(6, * estado))
  {
    b_SetBit(7, * estado);
    b_ClearBit(6, * estado);
  }
}

/*********************************************************************
   Conmuta al Estado que necesita la aplicacion / maquina de estados
 *********************************************************************/
void CambiarEstado(unsigned char * estado, unsigned char DesireState)
{
  *estado = (unsigned char)(DesireState | Offset_Estado);
}

/***************************************
   Funcion para inicializar las tareas
 ***************************************/
void Init_Task(const vFuncPtrV * ptrTaskList) {
  for (FST_Task_cnt = FIRST_INI_TASK; FST_Task_cnt < FIRST_FST_TASK; FST_Task_cnt++)
  {
    ptrTaskList[FST_Task_cnt]();
  }

  MID_Task_cnt = FIRST_MID_TASK;
  SLO_Task_cnt = FIRST_SLO_TASK;
}
