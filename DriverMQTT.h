/***********************************
 * DriverMQTT.h
 * Created on: 12/11/2020
 * Author: Wintrok Ingenieria
 * 
 ***********************************/

#ifndef DriverMQTT_H_
#define DriverMQTT_H_



typedef struct 
{
  unsigned char State_ConMQTT : 1;
  unsigned char State_Publish : 1;
  unsigned char Rx_TopicServ  : 1;
  unsigned char Rx_TopicApp   : 1;
  unsigned char Tx_TopicServ  : 1;
  unsigned char Tx_TopicApp   : 1;
  unsigned char Conectar_MQTT : 1;
  unsigned int  Long_Msg_Rx;
  char Buff_AppTopic_Rx[250];
  char Buff_ServTopic_Rx[250];
  unsigned int  Long_Msg_Tx;
  char Buff_AppTopic_Tx[250];
  char Buff_ServTopic_Tx[250];
  
  char TopicApp[40];
  char TopicServ[40];
}strMQTT;


//const char* mqtt_server = "142.93.88.99";     //IP SERVIDOR MQTT POR DEFECTO

/**************************************
 * Definicion de tiempos del programa *
 *************************************/
#define DELAY_INIT_MQTT     30    // Tiempo que espera para el arranque del programa
 

#define ConectadoServMQTT   Flags_MQTT.State_ConMQTT


void Driver_MQTT(void);

#endif /* DriverMQTT_H_ */
