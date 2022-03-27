
  //NOTA: El tamaño del buffer para el MQTT se modifico en la libreria PubSubClient.h en 
  //la definicion #define MQTT_MAX_PACKET_SIZE 200, para recibir el JSON del servidor
  
  /*****************************************************
   * PENDIENTES A SER REALIZADOS
   * - Dar respuesta a las acciones solicitadas
   * - Definir todas las posibles acciones a ser realizadas
   * - Ajustar la recepcion de la hora Unix, para su visualizacion (No es necesario)
   * - Manejo pantalla OLED
   * - El sabado 26 de Marzo arranca a trabajar el Git
   */
    
  #include <Arduino.h>
  //#include <ESP8266WiFi.h>
  //#include <DNSServer.h>
  //#include <ESP8266WebServer.h >
  #include <WiFiManager.h>        // Version utilizada 2.0.10
  #include <PubSubClient.h>       // Version utilizada 2.8.0
  #include "RTOS.h"
  #include "Aplicacion.h"
  #include "DriverMQTT.h"
  //#include "DEC_HCS301.h"
  //#include "Recepcion_RF.h"
  //#include "Calc_Key.h"
  //#include "Driver_Led.h"
  #include "Entradas.h"
  //#include "Sirenas.h"
  #include "Salidas.h"
  #include "Config_Operador.h"
  
const vFuncPtrV TaskList[]=
{
  // Tareas Iniciales
  Init_Bases_Tiempo,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,

  // Tareas Rapidas
  TaskDummy,
  TaskDummy,
  TaskDummy,

  // Tareas Medias
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,

  // Tareas Lentas
  Bases_Tiempo,
  Entradas,
  Driver_MQTT,
  Aplicacion,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  TaskDummy,
  Salidas,
};
  
/***************************
 * DEFINICION DE VARIABLES *
 ***************************/
extern stFlagsRTOS Flags_RTOS;
extern strMQTT Flags_MQTT;

unsigned char Test_Task;

WiFiClient espClient;
PubSubClient client(espClient);
const PROGMEM char* MQTT_CLIENT_ID = "223456789";
const PROGMEM char* MQTT_USER = "brunkeybox";
const PROGMEM char* MQTT_PASSWORD = "****";

/******************************************************************************
 * PROCEDIMIENTO PARA VERIFICAR SI HAY DATOS RECIBIDOS DESDE EL SERVIDOR MQTT *
 ******************************************************************************/
void callback(char* topic, byte* payload, unsigned int length) 
 {
    // Se verifica si los datos recibidos vienen del Topico de Aplicacion
    if(!strcmp(Flags_MQTT.TopicApp, topic))
    {
      // Llego data desde el topico de Aplicacion
      Flags_MQTT.Rx_TopicApp = true;  
      // Se limpia el buffer de recepcion
      memset(Flags_MQTT.Buff_AppTopic_Rx, 0x00, sizeof(Flags_MQTT.Buff_AppTopic_Rx));
      // Son copiados los datos recibidos
      memcpy(Flags_MQTT.Buff_AppTopic_Rx, payload, length);
    }
    // Se carga la cantidad de datos recibidos
    Flags_MQTT.Long_Msg_Rx = length;
    Serial.println(Flags_MQTT.Long_Msg_Rx);
 }

/************************************************************
 * Procedimiento para realizar la conexion al servidor MQTT *
 ************************************************************/
void reconnect() 
{
  // Intenta la conexion con el servidor MQTT por 1 minuto
  unsigned char ucNumIntentos;
  ucNumIntentos = 0;
  
  while (!client.connected()) 
  {
    Serial.print("Conectando Servidor MQTT...");
    // Create a random client ID
    String clientId = Client_ID;
    
    String userName = User_Name;
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(), userName.c_str())) 
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) 
    {
      Serial.println("Conectado al Servidor MQTT");
      // Once connected, publish an announcement...
      //**client.publish("wintrok/talanquera/spot", "Hola Wintrok");
      // ... and resubscribe
      client.subscribe(Topico_Aplicacion);
    } 
    else 
    {
      if(++ucNumIntentos != 12)
      {
        Serial.print("Fallo conexion, rc=");
        Serial.print(client.state());
        Serial.println(" Intentando nuevamente en 5 segundos");
        // Wait 5 seconds before retrying
        delay(5000);
      }
      else
      {
        ESP.restart();
      }
    }
  }
}

/***********************************************************
 * Vector donde recide la interrupcion por tiempo de 100uS *
 ***********************************************************/
void onTimerISR(void)
{
  Flags_RTOS.Flag_Ticker = true;
  Test_Task = true;
  //rtos(TaskList);
  //vDec_HCS301();
}

/**************************************************
 * Configuracion del Timer, condiciones iniciales *
 * Tiempo a ser configurado 100uS                 *
 **************************************************/
void Init_Timer(void)
{
  timer1_attachInterrupt(onTimerISR);
  //El Timer es programado con auto recarga
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  //Es ajustado el timer para un periodo de 100uS
  timer1_write(506);
}

/****************************************************
 * Verificacion de la conexion con el servidor MQTT *
 * y verifica si se deben publicar mensajes         *
 ****************************************************/
void Test_Conexion_MQTT(void)
{
  if (client.connected())
  {
    Flags_MQTT.State_ConMQTT = true; 
  }
  else
  {
    Flags_MQTT.State_ConMQTT = false;
  }
  // Es verificado si se deben enviar datos al servidor MQTT
  if(Flags_MQTT.Tx_TopicApp)
  {
    Flags_MQTT.Tx_TopicApp = false;
    // Hay datos a transmitir al topico de Aplicacion
    Flags_MQTT.State_Publish = false;
    //if(client.publish("wintrok/talanquera/spot", Flags_MQTT.Buff_AppTopic_Tx))
    //if(client.publish("wintrok/operador/1225", Flags_MQTT.Buff_AppTopic_Tx))
    //if(client.publish("wintrok/operador", Flags_MQTT.Buff_AppTopic_Tx))
    if(client.publish(Topico_Aplicacion, Flags_MQTT.Buff_AppTopic_Tx))
    {
      Flags_MQTT.State_Publish = true;
    }
  }
}

/*****************************************************************
 * PROCEDIMIENTO PARA EJECUTAR CONDICIONES INICIALES DEL SISTEMA *
 *****************************************************************/
void setup() 
{
  // Este codigo se ejecuta una sola vez:
  // Se inicializa el serial para debug
  Serial.begin(115200);

  // Configira el pin de RF
  //pinMode(PIN_RF, INPUT);
  // Configura pin para manejo del Led
  //pinMode(PIN_LED, OUTPUT);
  // Configira el pin de Relevo
  //pinMode(PIN_RELEVO, OUTPUT);
  // Configira el pin del Sensor de Puerta
  //pinMode(PIN_PUERTA, INPUT);
  // Inicializa con el Led encendido
  //digitalWrite(PIN_LED, LOW);
  // Creamos una instancia de la clase WiFiManager
  WiFiManager wifiManager;
 
  // Descomentar para resetear configuración
  //wifiManager.resetSettings();
 
  // Creamos AP y portal cautivo
  wifiManager.autoConnect("BrunKeyBoxWifi");
  Serial.println("Ya estás conectado");

  // Es inicializada el contador de tareas
  Init_Task(TaskList);
  Serial.println("Inicializadas las tareas del RTOS");
  
  // Se inicia la conexion con el servidor MQTT
  /*Serial.println("Se inicia la conexion con el Servidor MQTT");
  client.setServer("142.93.88.99", 1883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  Flags_MQTT.State_ConMQTT = true; */
  // Es inicializado el timer que nos da la base de tiempo para el sistema Operativo
  Init_Timer();
  Serial.println("Es inicializado el timer del RTOS");

  //setBufferSize(400);
  // Es inicializado por defecto el topico de Aplicacion
  /*memset(Flags_MQTT.TopicApp, 0x00, sizeof(Flags_MQTT.TopicApp));
  memcpy(Flags_MQTT.TopicApp, Topico_Aplicacion, strlen(Topico_Aplicacion));*/
}

/************************************************************
 * Procedimiento para realizar la conexion al servidor MQTT *
 * lo intenta hasta que se pueda conectar                   *
 ************************************************************/
void Conectar_Serv_MQTT(void)
{
  // Es cargada la IP y el puerto del servidor MQTT
  Serial.println("Se inicia la conexion con el Servidor MQTT");
  //client.setServer("142.93.88.99", 1883);
  client.setServer(DirServidor_Operador, Puerto_Operador);
  //client.setServer("142.93.88.99", 1885);
  
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  Flags_MQTT.Conectar_MQTT = false;
  Flags_MQTT.State_ConMQTT = true; 
  // Es inicializado por defecto el topico de Aplicacion
  memset(Flags_MQTT.TopicApp, 0x00, sizeof(Flags_MQTT.TopicApp));
  memcpy(Flags_MQTT.TopicApp, Topico_Aplicacion, strlen(Topico_Aplicacion));
}

/*************************************************************
 * PROCEDIMIENTO LOOP QUE SE ESTA EJECUTANDO EN TODO MOMENTO *
 *************************************************************/
void loop() 
{
  client.loop();
  Test_Conexion_MQTT();
  if(Test_Task)
  {
    Test_Task = false;
    rtos(TaskList);
    //vDec_HCS301();
  }
  if(Flags_MQTT.Conectar_MQTT)
  {
    Conectar_Serv_MQTT();
  }
  
}
