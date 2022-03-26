/*
 * Aplicacion.h
 *
 *  Created on: 12/06/2020
 *      Author: Wintrok Ingenieria
 */

#ifndef APLICACION_H_
#define APLICACION_H_

#define TIEMPO_ARRANQUE_DRV_MQTT    50  // Tiempo de espera al arranque de la Aplicacion
#define TIEMPO_FILTRO_CONTROL       10  // Tiempo de espera antes de recibir un nuevo control

const unsigned char SSID_val[ ] = {"SSID"};
const unsigned char PSW_val[ ] = {"PSW"};
const unsigned char RSSI_val[ ] = {"RSSI"};
const unsigned char CHANNEL_val[ ] = {"CHANNEL"};

const unsigned char AbrirLlave[ ] = {'{','\0'};
const char Comilla[ ] = {'"','\0'};
const unsigned char DosPuntos[ ] = {':','\0'};
const unsigned char CerrarLlave[ ] = {'}','\0'};
const unsigned char Coma[ ] = {',','\0'};
const unsigned char Enter[ ] = {'\r','\0'};


void Aplicacion(void);


#endif /* APLICACION_H_ */
