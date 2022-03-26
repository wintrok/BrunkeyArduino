/*
 * Salidas.h
 *
 *  Created on: 26/10/2016
 *      Author: Jorge
 */

#ifndef SOURCES_SALIDAS_H_
#define SOURCES_SALIDAS_H_

typedef struct
{
  unsigned char Led     : 1;
  unsigned char Relevo  : 1;
}Outputs;




#define RELEVO_ON       Salidas_Operador.Relevo = true
#define RELEVO_OFF      Salidas_Operador.Relevo = false




void Salidas(void);

#endif /* SOURCES_SALIDAS_H_ */
