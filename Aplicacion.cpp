/***********************************
 * Aplicacion.cpp
 * Created on: 12/06/2020
 * Author: Wintrok Ingenieria
 * 
 * 
 * Pendientes:
 *  - Si se desconecta del Broker no reiniciar la conexion, solo reconectarse al servidor MQTT [ok]
 *  - Verificar la conexion Wifi [ok]
 *  - Generar una funcion para que envie los datos via JSON al Broker MQTT
 *    Se va a enviar la version del software, la SSID, Password, RSSI, Canal de Operacion [ok]
 *    
 ***********************************/

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "RTOS.h"
#include "Aplicacion.h"
#include "DriverMQTT.h"
#include "Recepcion_RF.h"
#include "Calc_Key.h"
#include "Entradas.h"
#include <string.h>

enum Select_State_Aplicacion
{
   ST_ESPERA_ARRANQUE = 0,
   ST_CONEXION_SERVIDOR_MQTT,
   ST_ESPERAR_RTA_DATOS_RED,
   ST_ESPERA_ACCION,
   ST_RETARDO_ENVIO_CONTROL,
   ST_SEND_JSON_CONTROL,
   ST_SEND_JSON_CERRADA,
   ST_SEND_JSON_ABIERTA,
};

const char InicioSoftware1[ ] = {"Wintrok Ingenieria SAS"};
const char InicioSoftware2[ ] = {"Version SW 1.2 Enero 30/2021"};
const unsigned char Serial_Control[ ] = {"CntrSerial"};
const unsigned char Codigo_Boton[ ] = {"CntrBoton"};
const unsigned char Boton_1[ ] = {"1"};
const unsigned char Boton_2[ ] = {"2"};
const unsigned char Boton_3[ ] = {"3"};
const unsigned char Boton_4[ ] = {"4"};

const unsigned char Puerta[ ] = {"ID_Puerta"};
const unsigned char PuertaID[ ] = {"CC CentroMayor"};
const unsigned char Estado[ ] = {"Estado"};
const unsigned char Abierta[ ] = {"Abierta"};
const unsigned char Cerrada[ ] = {"Cerrada"};
  
unsigned char ucEstado_Aplicacion = Offset_Estado | ST_ESPERA_ARRANQUE;

unsigned long ulTime;

extern strMQTT Flags_MQTT;
extern unsigned char ucHay_Control;
extern unsigned long ulSerial_Recibido;
extern stRtaDecrypt Rta_Decrypt;
extern strInputs Entradas_Operador;
 
/**********************************
 * FUNCION PRINCIPAL DEL PROGRAMA *
 **********************************/

 void Aplicacion(void)
 {
    unsigned char *ptrAuxDat;
    char *ptrAuxDat2;
    char AuxBuffApp[20];
    
    CompararEstado(&ucEstado_Aplicacion);
    switch(ucEstado_Aplicacion & ucMaskState)
    {

      /*********************************************
      * Estado de espera para arranque del sistema *
      *********************************************/
      case ST_ESPERA_ARRANQUE:
        if(Inicio_Estado(ucEstado_Aplicacion))
        {
          Serial.println("Espera Tiempo de arranque Aplicacion");
          Reiniciar_Temporizador(&ulTime);
        }
        else
        {
          if(Retorna_Temporizador(TIEMPO_ARRANQUE_DRV_MQTT, Time_100mS, &ulTime))
          {
            //digitalWrite(14, HIGH);
            Serial.println(InicioSoftware1);
            Serial.println(InicioSoftware2);
            CambiarEstado(&ucEstado_Aplicacion, ST_CONEXION_SERVIDOR_MQTT);
          }
        }
      break;      
      
      /*************************************************************
      * Estado encargado de verificar la conexion al servidor MQTT *
      *************************************************************/
      case ST_CONEXION_SERVIDOR_MQTT:
        if(Inicio_Estado(ucEstado_Aplicacion))
        {
          // Es solicitada la conexion al servidor MQTT
          Flags_MQTT.Conectar_MQTT = true;          
        }
        else
        {
          if(ConectadoServMQTT)
          {
            Serial.println("Inicia Aplicacion");
            Serial.print("Version SDK: ");
            Serial.println(ESP.getSdkVersion());
            CambiarEstado(&ucEstado_Aplicacion, ST_ESPERA_ACCION);
          }
        }
      break;

      /********************************************************************
       * Estado encargado de esperar si se recibieron los datos de la red *
       * en la Dashboard                                                  * 
       *******************************************************************/
      case ST_ESPERAR_RTA_DATOS_RED:
        // Se verifica si la publicacion fue correcta
        if(Flags_MQTT.State_Publish)
        {
          Serial.println("Publicacion Exitosa");
        }
        else
        {
          Serial.println("Error en la Publicacion");
          // Es reiniciado el modulo ESP8266-07
          ESP.restart();
        }
        CambiarEstado(&ucEstado_Aplicacion, ST_ESPERA_ACCION);
      break;

      /*****************************************************
      * Estado que espera que operacion debe ser realizada *
      *****************************************************/
      case ST_ESPERA_ACCION:
        if(Inicio_Estado(ucEstado_Aplicacion))
        {
          Reiniciar_Temporizador(&ulTime);
        }
        else
        {
          // Se verifica si fue presionado un control remoto
          /*if(ucHay_Control)
          {
            // Fue recibido un control es procesado
            ucHay_Control = false;
            if(ConectadoServMQTT)
            {
              CambiarEstado(&ucEstado_Aplicacion, ST_SEND_JSON_CONTROL);  
            }
          }*/
          if(Entradas_Operador.Flanco_Cerrada)
          {
            Serial.println("Flanco Cerrada");
            CambiarEstado(&ucEstado_Aplicacion, ST_SEND_JSON_CERRADA);  
          }
          if(Entradas_Operador.Flanco_Abierta)
          {
            Serial.println("Flanco Abierta");
            CambiarEstado(&ucEstado_Aplicacion, ST_SEND_JSON_ABIERTA);  
          }
        }
      break;

      /*****************************************************************
      * Estado que realiza un retardo al momento de recibir un control *
      *****************************************************************/
      case ST_RETARDO_ENVIO_CONTROL:
        if(Inicio_Estado(ucEstado_Aplicacion))
        {
          Serial.println("Inicializando Retardo Control");
          Reiniciar_Temporizador(&ulTime);
          // Se verifica si la publicacion fue correcta
          if(Flags_MQTT.State_Publish)
          {
            Serial.println("Publicacion Exitosa");
          }
          else
          {
            Serial.println("Error en la Publicacion");
            // Es reiniciado el modulo ESP8266-07
            ESP.restart();
          }
        }
        else
        {
          if(Retorna_Temporizador(TIEMPO_FILTRO_CONTROL, Time_100mS, &ulTime))
          {
            Serial.println("Finalizo Retardo Control");
            // Limpia recepcion de controles
            ucHay_Control = false;
            CambiarEstado(&ucEstado_Aplicacion, ST_ESPERA_ACCION);
          }
        }
      break;      

      /*************************************************************************
      * Estado encargado de publicar el codigo del control y el boton recibido *
      *************************************************************************/
      case ST_SEND_JSON_CONTROL:
        // Es inicializado el buffer de transmision hacia el servidor MQTT
        memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
        // Inicio formato JSON
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AbrirLlave);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Es cargado el serial del control presionado
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Serial_Control);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Se convierte a Ascii el codigo del control
        ultoa(ulSerial_Recibido, (char*)AuxBuffApp,10);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AuxBuffApp);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Es cargado el codigo del Boton
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Codigo_Boton);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Es verificado el boton que se activo
        if(Rta_Decrypt.Boton_1)
        {
          strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Boton_1);
        }
        else
        {
          if(Rta_Decrypt.Boton_2)
          {
            strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Boton_2);
          }
          else
          {
            if(Rta_Decrypt.Boton_3)
            {
              strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Boton_3);
            }
            else
            {
              if(Rta_Decrypt.Boton_4)
              {
                strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Boton_4);  
              }
            }
          }
        }
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &CerrarLlave);
        // Solicita la publicacion al servidor MQTT
        Flags_MQTT.Tx_TopicApp = true;
        CambiarEstado(&ucEstado_Aplicacion, ST_RETARDO_ENVIO_CONTROL);
      break;

      /*********************************************************
      * Estado encargado de publicar que la puerta fue cerrada *
      *********************************************************/
      case ST_SEND_JSON_CERRADA:
        // Es inicializado el buffer de transmision hacia el servidor MQTT
        memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
        // Inicio formato JSON
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AbrirLlave);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Se visualiza el ID de la Puerta
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Puerta);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Se convierte a Ascii el codigo del control
        //ultoa(ulSerial_Recibido, (char*)AuxBuffApp,10);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &PuertaID);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        // Es cargado el codigo del Boton
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Estado);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Cerrada);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &CerrarLlave);
        // Solicita la publicacion al servidor MQTT
        Flags_MQTT.Tx_TopicApp = true;
        CambiarEstado(&ucEstado_Aplicacion, ST_RETARDO_ENVIO_CONTROL);
      break;      

      /*********************************************************
      * Estado encargado de publicar que la puerta fue cerrada *
      *********************************************************/
      case ST_SEND_JSON_ABIERTA:
        // Es inicializado el buffer de transmision hacia el servidor MQTT
        memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
        // Inicio formato JSON
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AbrirLlave);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Se visualiza el ID de la Puerta
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Puerta);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        // Se convierte a Ascii el codigo del control
        //ultoa(ulSerial_Recibido, (char*)AuxBuffApp,10);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &PuertaID);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        // Es cargado el codigo del Boton
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Estado);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Abierta);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &CerrarLlave);
        // Solicita la publicacion al servidor MQTT
        Flags_MQTT.Tx_TopicApp = true;
        CambiarEstado(&ucEstado_Aplicacion, ST_RETARDO_ENVIO_CONTROL);
      break;      
      
    } 
 }
