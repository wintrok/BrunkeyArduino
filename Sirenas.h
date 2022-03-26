/***********************************
 * Sirenas.h
 * Created on: 01/02/2021
 * Author: Wintrok Ingenieria
 * 
 ***********************************/

#ifndef Sirenas_H_
#define Sirenas_H_



typedef struct 
{
  unsigned char Sirena_Panico   : 1;
  unsigned char Sirena_Medica   : 1;
  unsigned char Sirena_Incendio : 1;
}strSirenas;



/**************************************
 * Definicion de tiempos del programa *
 *************************************/
#define DELAY_INIT_SIRENAS      30    // Tiempo que espera para el arranque del programa
#define TIME_ACTIVA_SIRENAS     20    //Tiempo que se activa el pulso de la señal sirena (Base 50mS)

#define SIRENA_INCENDIO_ON    digitalWrite(16, HIGH)
#define SIRENA_INCENDIO_OFF   digitalWrite(16, LOW)




void Sirenas(void);

#endif /* Sirenas_H_ */
