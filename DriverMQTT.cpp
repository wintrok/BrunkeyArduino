/***********************************
 * DriverMQTT.cpp
 * Created on: 12/11/2020
 * Author: Wintrok Ingenieria
 * 
 ***********************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>    // Version Instalada 1.2.7
#include "DriverMQTT.h"
#include "RTOS.h"
#include "Aplicacion.h"
#include "Sirenas.h"
#include "Config_Operador.h"
#include "Salidas.h"

/******************************************
 * Definicion de los estados del programa *
 *****************************************/
enum Select_State_Aplicacion
{
   ST_INIT_MQTT = 0,
   ST_VERIF_SERV_MQTT,
   ST_ESPERAR_ACTIVIDAD,
   ST_RECONECTAR_MQTT,
   ST_VERIF_DATA_SERVIDOR,
   ST_GENERA_PULSO_RELEVO,
   ST_RESPONDER_SERVER,
};

unsigned char ucEstado_MQTT = Offset_Estado | ST_INIT_MQTT;

const unsigned char Verif_LlaveInicio[ ] = {"{\"Comando\":\"ConfWifi\"}"};
const unsigned char Verif_Comando[ ] = {"Comando"};
const unsigned char Cmd_Config_Wifi[ ] = {"{\"Comando\":\"ConfWifi\"}"};
const unsigned char Cmd_Servidor[ ] = {"\"sentido\": \"S\""};

/***************************************
 * PARA PRUEBAS SIMULANDO LA DASHBOARD *
 ***************************************/
//const unsigned char Orden_Activar[ ] = {"{\"Comando\":\"Activar\"}"};
//const unsigned char Orden_Activar[ ] = {"{\"Comando\":\"Activar\""};
//const unsigned char Orden_Activar[ ] = {"Activar"};
const unsigned char Orden_Activar[ ] = {"Panico"};
const unsigned char Orden_Abrir_Puerta[ ] = {"\"accion\": 1"};
const unsigned char Num_Transaccion[ ] = {"\"trn\": "};
const unsigned char Num_Locker[ ] = {"\"port\": "};
//const unsigned char Orden_Desactivar[ ] = {"{\"Comando\":\"Desactivar\"}"};
//const unsigned char Orden_Desactivar[ ] = {"{\"Comando\":\"Desactivar\""};
const unsigned char Orden_Desactivar[ ] = {"Desactivar"};
const unsigned char Orden_Incendio[ ] = {"{\"Comando\":\"Incendio\"}"};
const unsigned char Orden_Medico[ ] = {"{\"Comando\":\"Medico\"}"};

//const unsigned char Rta_Valida[ ] = {"{\"Comando\":\"OK\"}"};
//const unsigned char Rta_Valida[ ] = {"{\"Comando\":\"OK\",\"ID\":\"1225\"}"};
const unsigned char Rta_Valida[ ] = {"{\"Comando\":\"OK\",\"ID\":\""};
//const unsigned char Rta_Valida[ ] = {"{\"Comando\":\"OK\",\"ID\":\"1\"}"};
const unsigned char Rta_Fallida[ ] = {"{\"Comando\":\"Fail\"}"};

/*******************************
 * RESPUESTA HACIA EL SERVIDOR *
 *******************************/
const unsigned char Rta_Servidor[ ] = {"{\"com\": \"SERVIDOR\",\"sentido\": \"A\",\"datos\": {\"hub\": \""};
const unsigned char Rta_Locker[ ] = {"\"port\": "};
const unsigned char Rta_Accion[ ] = {"\"accion\": 1,\"trn\": "};
const unsigned char Rta_Msg[ ] = {"\"msg\": \"Abrir Puerta\",\"timestamp\": 637827006099152859}}"};

 
 
  

String ValSSID, ValPsk;
char strValSSID[40];
int ValRSSI, ValChannel;
char NumTrans[10];
char NumLocker[10];
extern Outputs  Salidas_Operador;

/**************************************
 * Definiciones para el servidor MQTT *
 *************************************/

strMQTT Flags_MQTT;
unsigned long ulTime_MQTT;

/***********************************************************
 * FUNCION PARA ENVIAR LOS DATOS DE LA RED EN FORMATO JSON *
 ***********************************************************/
 void Enviar_JSON_Coneccion(void)
 {
    unsigned int AuxNum;
    char AuxBuff[20];
    
    ValSSID = WiFi.SSID();
    AuxNum = ValSSID.length();
    ValRSSI = WiFi.RSSI();
    ValChannel = WiFi.channel();
    ValPsk = WiFi.psk();
    //Serial.println(ValPsk); 
    //Serial.println(ValRSSI); 
    //Serial.println(ValChannel); 
    // Es inicializado el buffer de transmision hacia el servidor MQTT
    memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
    // Inicio formato JSON
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AbrirLlave);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    // Es cargado el nombre de la red SSID
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &SSID_val);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    //memset(AuxBuff, 0x00, 20);
    ValSSID.toCharArray((char*)AuxBuff, AuxNum + 1);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AuxBuff);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Enter);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    // Es cargado el password de la red
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &PSW_val);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    //memset(AuxBuff, 0x00, 20);
    AuxNum = ValPsk.length();
    ValPsk.toCharArray((char*)AuxBuff, AuxNum + 1);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AuxBuff);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Enter);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    // Es cargado el valor del RSSI
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &RSSI_val);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    //memset(AuxBuff, 0x00, 20);
    itoa(ValRSSI, (char*)AuxBuff, 10);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AuxBuff);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Enter);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    // Es cargado el canal utilizado
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &CHANNEL_val);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &DosPuntos);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    //memset(AuxBuff, 0x00, 20);
    itoa(ValChannel, (char*)AuxBuff, 10);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &AuxBuff);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
    strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &CerrarLlave);
    // Se imprime por el debug
    Serial.println(Flags_MQTT.Buff_AppTopic_Tx); 
    // Es activada la pubicacion al MQTT
    Flags_MQTT.Tx_TopicApp = true;
 }

/****************************************************************
 * FUNCION PARA ENVIAR RESPUESTA A LOS COMANDO VALIDA O FALLIDA *
 ****************************************************************/
 void Enviar_JSON_Rta_Valida(void)
 {
    unsigned int AuxNum;  
    char *ptrAux;
    // Es inicializado el buffer de transmision hacia el servidor MQTT
    memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
    memcpy(Flags_MQTT.Buff_AppTopic_Tx, Rta_Valida, sizeof(Rta_Valida));
    ptrAux = &Flags_MQTT.Buff_AppTopic_Tx[0];
    AuxNum = strlen(Flags_MQTT.Buff_AppTopic_Tx);
    ptrAux += AuxNum;
    memcpy(ptrAux, (const char*)Client_ID, sizeof(Client_ID));
    AuxNum = strlen(Flags_MQTT.Buff_AppTopic_Tx);
    ptrAux = &Flags_MQTT.Buff_AppTopic_Tx[0];
    AuxNum = strlen(Flags_MQTT.Buff_AppTopic_Tx);
    ptrAux += AuxNum;
    *ptrAux = '"';
    ++ptrAux;
    *ptrAux = '}';
    // Se imprime por el debug
    Serial.println(Flags_MQTT.Buff_AppTopic_Tx); 
    // Es activada la pubicacion al MQTT
    Flags_MQTT.Tx_TopicApp = true;
 }
 
 void Enviar_JSON_Rta_Fallida(void)
 {
    // Es inicializado el buffer de transmision hacia el servidor MQTT
    memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
    memcpy(Flags_MQTT.Buff_AppTopic_Tx, Rta_Fallida, sizeof(Rta_Fallida));
    // Se imprime por el debug
    Serial.println(Flags_MQTT.Buff_AppTopic_Tx); 
    // Es activada la pubicacion al MQTT
    Flags_MQTT.Tx_TopicApp = true;
 }

/***********************************************
 * PROCEDIMIENTO PARA MANEJO DEL SERVIDOR MQTT *
 ***********************************************/
void Driver_MQTT(void)
{
  char *ptrTrama, *ptrTrama2;
    
    CompararEstado(&ucEstado_MQTT);
    switch(ucEstado_MQTT & ucMaskState)
    {

      /****************************
      * Estado inicial del driver *
      ****************************/
      case ST_INIT_MQTT:
        if(Inicio_Estado(ucEstado_MQTT))
        {
          Reiniciar_Temporizador(&ulTime_MQTT);
        }
        else
        {
          if(Retorna_Temporizador(DELAY_INIT_MQTT, Time_100mS, &ulTime_MQTT))
          {
            CambiarEstado(&ucEstado_MQTT, ST_VERIF_SERV_MQTT);
          }
        }
      break;     

      /******************************************************
      * Estado encargado de esperar la conexion al servidor *
      ******************************************************/
      case ST_VERIF_SERV_MQTT:
        if(ConectadoServMQTT)
        {
          Serial.println("Iniciando Driver MQTT");
          CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
        }
      break;     

      /*******************************************************
      * Estado encargado de esperar actividad en el servidor *
      *******************************************************/
      case ST_ESPERAR_ACTIVIDAD:
        // Se sigue conectado al Servidor MQTT?
        if(!ConectadoServMQTT)
        {
          // Se desconecto del servidor
          Serial.println("Se desconecto del Servidor MQTT");
          // Es reiniciado el modulo ESP8266-07
          if(WiFi.status() == WL_CONNECTED)
          {
            // Seguimos conectados a la red WIfi
            Serial.println("Pero con conexion a la red Wifi");
            // Se procede a solicitar conexion al Servidor MQTT 
            //LED_ON;
            CambiarEstado(&ucEstado_MQTT, ST_RECONECTAR_MQTT);
          }
          else
          {
            if(WiFi.status() == WL_DISCONNECTED)
            {
              // Estamos desconectados de la red WIfi
              Serial.println("Perdimos conexion de la red Wifi");
              ESP.restart();
              CambiarEstado(&ucEstado_MQTT, ST_INIT_MQTT);
            }
          }
        }
        else
        {
          // Se recibieron datos por el topico de aplicacion?
          if(Flags_MQTT.Rx_TopicApp)
          {
            // Hay datos por el topico de aplicacion
            Flags_MQTT.Rx_TopicApp = false;
            Serial.print("Se recibio desde el Topico de Aplicacion: ");
            for (int i = 0; i < Flags_MQTT.Long_Msg_Rx; i++) 
            {
              Serial.print((char)Flags_MQTT.Buff_AppTopic_Rx[i]);
            }
            Serial.println();
            CambiarEstado(&ucEstado_MQTT, ST_VERIF_DATA_SERVIDOR);
          }
        }
      break;     

      /*************************************************************
      * Estado encargado de esperar la reconeccion con el servidor *
      **************************************************************/
      case ST_RECONECTAR_MQTT:
        if(Inicio_Estado(ucEstado_MQTT))
        {
          // Es solicitada la conexion al Servidor MQTT
          Flags_MQTT.Conectar_MQTT = true;
        }
        else
        {
          if(ConectadoServMQTT)
          {
            CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
          }
        }
      break;

      /*******************************************************************
      * Estado encargado de verificar la data recibida desde el servidor *
      ********************************************************************/
      case ST_VERIF_DATA_SERVIDOR:
        // Es verificada la data recibida
        // SE CONFIRMA EL ID DEL DISPOSITIVO SI ES EL CORRECTO
        ptrTrama = strstr((const char*)Flags_MQTT.Buff_AppTopic_Rx, (const char*)Client_ID);
        if(ptrTrama)
        {
          Serial.println("ES PARA MI");
          // Se verifica la procedencia del mensaje 
          ptrTrama2 = strstr((const char*)Flags_MQTT.Buff_AppTopic_Rx, (const char*)Cmd_Servidor);        
          if(ptrTrama2)
          {
            // Se verifica si es una orden para abrir puerta
            Serial.println("ORDEN RECIBIDA DESDE EL SERVIDOR");
            ptrTrama2 = strstr((const char*)Flags_MQTT.Buff_AppTopic_Rx, (const char*)Orden_Abrir_Puerta);        
            if(ptrTrama2)
            {
              Serial.println("SE DEBE ABRIR LA PUERTA DEL LOCKER");
              // Se guarda el numero de transaccion
              ptrTrama2 = strstr((const char*)Flags_MQTT.Buff_AppTopic_Rx, (const char*)Num_Transaccion);
              if(ptrTrama2)
              {
                // Se encontro el numero de transaccion
                memset(NumTrans, 0x00, 10);
                ptrTrama2 += 7;
                //unsigned char n;
                for (char n=0; *ptrTrama2 != ','; n++)
                {
                   NumTrans[n] = *ptrTrama2;
                   ++ptrTrama2;
                }
                Serial.print("Numero Transaccion: ");
                Serial.println(NumTrans);
                //++ptrTrama2;
                //NumTrans[0] = *ptrTrama2; 
                // Se almacena el numero de Locker a ser abierto
                ptrTrama = strstr((const char*)Flags_MQTT.Buff_AppTopic_Rx, (const char*)Num_Locker);
                if(ptrTrama)
                {
                  // Hay numero de Locker
                  memset(NumLocker, 0x00, 10);
                  ptrTrama += 8;
                  //unsigned char n;
                  for (char n=0; *ptrTrama != ','; n++)
                  {
                     NumLocker[n] = *ptrTrama;
                     ++ptrTrama;
                  }
                  Serial.print("Numero Locker: ");
                  Serial.println(NumLocker);
                  CambiarEstado(&ucEstado_MQTT, ST_RESPONDER_SERVER);  
                }
                else
                {
                  // No se encontrol el numero del Locker
                  Serial.println("No se encontro el numero del Locker");
                  CambiarEstado(&ucEstado_MQTT, ST_GENERA_PULSO_RELEVO);  
                }
                
              }
              else
              {
                // No se encontro el numero de transaccion
                Serial.println("NO SE ENCONTRO NUMERO DE TRANSACCION");
                CambiarEstado(&ucEstado_MQTT, ST_GENERA_PULSO_RELEVO);
              }
            }
            else
            {
              // La orden no se reconoce
              Serial.println("ORDEN NO DEFINIDA");
              CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
            }

          }
          else
          {
            // ORDEN DE ORIGEN DESCONOCIDO
            Serial.println("ORDEN RECIBIDA DE ORIGEN DESCONOCIDO");
            CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
          }
        }
        else
        {
          Serial.println("NO...ES PARA MI");
          CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
        }
      break;

      /***************************************************************
      * Estado encargado de activar la salida para mensaje de panico *
      ***************************************************************/
      case ST_GENERA_PULSO_RELEVO:
        if(Inicio_Estado(ucEstado_MQTT))
        {
          Reiniciar_Temporizador(&ulTime_MQTT);
          Serial.println("ABRIENDO PUERTA");
          RELEVO_ON;
        }
        else
        {
          if(Retorna_Temporizador(20, Time_100mS, &ulTime_MQTT))
          {
            RELEVO_OFF;
            CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);
          }
        }
      break;

      /******************************************************
      * Estado encargado de enviar la respuesta al servidor *
      ******************************************************/
      case ST_RESPONDER_SERVER:
        // Es inicializado el buffer de transmision hacia el servidor MQTT
        memset(Flags_MQTT.Buff_AppTopic_Tx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
        // Se carga el inicio de la respuesta
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Rta_Servidor);
        // Se carga el numero del hub
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Client_ID);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Comilla);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Rta_Locker);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &NumLocker);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Rta_Accion);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &NumTrans);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Coma);
        strcat(Flags_MQTT.Buff_AppTopic_Tx, (const char*) &Rta_Msg);
        
        // Se imprime por el debug
        Serial.println(Flags_MQTT.Buff_AppTopic_Tx); 
        // Es activada la pubicacion al MQTT
        Flags_MQTT.Tx_TopicApp = true;
        CambiarEstado(&ucEstado_MQTT, ST_ESPERAR_ACTIVIDAD);


      break;



      
    }   
}
