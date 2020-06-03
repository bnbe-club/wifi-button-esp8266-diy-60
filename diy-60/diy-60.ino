/****************************************************************************************************************************************************
 *  TITLE: An Efficient WiFi Switch For Home Automation
 *
 *  By Frenoy Osburn
 *  YouTube Video: https://youtu.be/nUgfFMaaipk
 ****************************************************************************************************************************************************/

 /********************************************************************************************************************
  * Please make sure that you install the board support package for the ESP8266 boards.
  * You will need to add the following URL to your Arduino preferences.
  * Boards Manager URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
  * You will also need to install the PubSubClient library by Nick O'Leary from the Tools->Library Manager
 ********************************************************************************************************************/
 
 /********************************************************************************************************************
 *  Board Settings:
 *  Board: "WeMos D1 R1 or Mini"
 *  Upload Speed: "921600"
 *  CPU Frequency: "80MHz"
 *  Flash Size: "4MB (FS:@MB OTA:~1019KB)"
 *  Debug Port: "Disabled"
 *  Debug Level: "None"
 *  VTables: "Flash"
 *  IwIP Variant: "v2 Lower Memory"
 *  Exception: "Legacy (new can return nullptr)"
 *  Erase Flash: "Only Sketch"
 *  SSL Support: "All SSL ciphers (most compatible)"
 *  COM Port: Depends *On Your System*
 *********************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MOSFET_PIN    D1

const char ssid[] = "Network";
const char password[] = "password";

const char mqttServer[] = "192.168.1.45";
const char mqttUsername[] = "mqtt";
const char mqttPassword[] = "mqtt";

char pubTopic[] = "mqtt/switch";     

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() 
{
  while (!client.connected())                       // loop until we are connected
  {
    String clientId = "ESP8266Client-";           
    clientId += String(random(0xffff), HEX);        // create a random client ID
    if (!client.connect(clientId.c_str(), mqttUsername, mqttPassword))    //attempt to connect to MQTT broker
    {
      delay(1000);        // wait 1 second before retrying to connect
    }
  }
}

void setup() 
{    
  uint8_t bootCounter = 0;  
  
  pinMode(MOSFET_PIN, OUTPUT); 
  digitalWrite(MOSFET_PIN, LOW);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    bootCounter++;
    if(bootCounter == 40)
    {
      ESP.deepSleep(0);
    }
    delay(500);
  }
  randomSeed(micros());       // do this as we will use random client ID
  
  client.setServer(mqttServer, 1883);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
    
  char payLoad[1];
  itoa(1, payLoad, 10);
  client.publish(pubTopic, payLoad);
  client.disconnect();
  while(client.connected());
  WiFi.disconnect();
  while(WiFi.status() == WL_CONNECTED);

  digitalWrite(MOSFET_PIN, HIGH);   // GPIO state is undefined in deep sleep with a low drive. This ensures that the MOSFET is switched OFF. 
  ESP.deepSleep(0);                 // the board should already be powered OFF due to the line above, so we will most likley not execute this statement
}
