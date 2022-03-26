/***********************************
 * Driver_Buzzer.h
 * Created on: 25/01/2021
 * Author: Wintrok Ingenieria
 * 
 ***********************************/

#ifndef Driver_Buzzer_H_
#define Driver_Buzzer_H_

#define BUZZER_ON    digitalWrite(14, HIGH)
#define BUZZER_OFF   digitalWrite(14, LOW)

#define DELAY_INIT_DRIVER_BUZZER   10    // Tiempo que se espera para iniciar el funcionamiento del Driver


/* Definicion eventos a ser generados*/

enum Evento_Buzzer
{
  EVENTO_GENERAR_BUZZER_ACTIVADA = 1,
  EVENTO_GENERAR_BUZZER_DESACTIVADA,
  EVENTO_APAGAR_SECUENCIA_BUZZER,
  EVENTO_GENERAR_BUZZER_ACEPTACION,
  EVENTO_GENERAR_BUZZER_TICK
};

/* Definicion constantes para la aplicacion*/

#define TIME_TOGGLE_BUZZER   3
#define TIME_TOGGLE_ACEPTAR  1
#define TIME_TICK            2

#define NUM_PITOS_ACTIVAR           2
#define NUM_PITOS_DESACTIVAR        1
#define NUM_PITOS_ACEPTAR           4

void Driver_Buzzer(void);

#endif /* Driver_Buzzer_H_ */
