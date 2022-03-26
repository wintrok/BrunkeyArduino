/*
 * RTOS.h
 *
 *  Created on: 12/06/2020
 *      Author: Wintrok Ingenieria
 */

#ifndef RTOS_H_
#define RTOS_H_

typedef void (*vFuncPtrV)(void);


#define n_INI_TASK      5
#define n_FST_TASK      3 //5
#define n_MID_TASK      9
#define n_SLO_TASK      10

#define  FIRST_INI_TASK      0
#define FIRST_FST_TASK      n_INI_TASK
#define FIRST_MID_TASK      FIRST_FST_TASK + n_FST_TASK
#define FIRST_SLO_TASK      FIRST_MID_TASK + n_MID_TASK
#define n_GLOBAL_TASKS      FIRST_SLO_TASK + n_SLO_TASK

#define b_SetBit(bit_ID, varID)         (varID |=   (unsigned char)(1<<bit_ID))
#define b_SetBit_Int(bit_ID, varID)     (varID |=   (unsigned int)(1<<bit_ID))
#define b_ClearBit(bit_ID, varID)       (varID &=  ~(unsigned char)(1<<bit_ID))
#define b_XorBit(bit_ID, varID)         (varID ^=   (unsigned char)(1<<bit_ID))
#define b_TestBit(bit_ID, varID)        (varID &    (unsigned char)(1<<bit_ID))
#define b_TestBit_Int(bit_ID, varID)    ((unsigned int)varID &    (unsigned int)(1<<bit_ID))

/******** DEFINICIONES PARA LA FUNCION BASES DE TIEMPO ************/

#define Val_50mS            5        // con 10 tareas lentas
#define Val_100mS           2
#define Val_500mS           5
#define Val_Segundo         2

#define Time_50mS           0
#define Time_100mS          1
#define Time_500mS          2
#define Time_Segundo        3

#define Offset_Estado           (unsigned char) 0x40
#define Inicio_Estado(varID)    (b_TestBit(7,varID))
#define ucMaskState             (unsigned char)  0x3F

typedef struct
{
   unsigned int Flag_Ticker   : 1;
}stFlagsRTOS;


void rtos(const vFuncPtrV * ptrTaskList);
void Init_Task(const vFuncPtrV * ptrTaskList);
void TaskDummy(void);
void Clock_ticker(void);
void Bases_Tiempo(void);
void Init_Bases_Tiempo(void);
unsigned char Retorna_Temporizador(unsigned long cantidad_tiempo, unsigned char Base_Tiempo, unsigned long * contador_tiempo);
unsigned char Retorna_Tiempo(unsigned char bit);
void Reiniciar_Temporizador(unsigned long *contador_tiempo);
void CompararEstado(unsigned char * estado);
void CambiarEstado(unsigned char * estado, unsigned char DesireState);


#endif /* RTOS_H_ */
