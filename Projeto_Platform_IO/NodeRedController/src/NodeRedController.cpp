#include "NodeRedController.h"

NodeRedController::NodeRedController(
    const char *wifissid,
    const char *wifipasswd,
    int wifitimeout,
    const char *mqttusername,
    const char *mqttpasswd,
    const char *mqttbroker,
    int mqttport,
    int mqtttimeout)
{
  wifi_ssid = wifissid;
  wifi_password = wifipasswd;
  wifi_timeout = wifitimeout;

  mqtt_username = mqttusername;
  mqtt_password = mqttpasswd;
  mqtt_broker = mqttbroker;
  mqtt_port = mqttport;
  mqtt_timeout = mqtttimeout;

  mqttClient.setClient(ESPWiFiClient);
  clientId += String(random(0xffff), HEX); // Geração de complemento do nome randomico
}

void NodeRedController::connectWiFi()
{
  WiFi.mode(WIFI_STA); // "Station mode": permite que o ESP32 seja um cliente
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Conectando à rede WiFi ..");

  // Captura o tempo, em milisegundos, desde que o programa começou a rodar o 'conectWifi'
  unsigned long startTime = millis();

  // Enquanto o status da conexão for diferente de conectado e não tenha ultrapassado o timeout
  while (WiFi.status() != WL_CONNECTED && ((int)(millis() - startTime) < wifi_timeout))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();

  // Informa se a conexão falhou ou, caso tenha conectado, informa o IP
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connection has failed!");
  }
  else
  {
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
  }
}

void NodeRedController::configMQTT()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  Serial.println("Configuração do broker efetuada!");
}

void NodeRedController::connectMQTT()
{
  Serial.print("Conectando ao MQTT Broker...");

  unsigned long startTime = millis();
  while (!mqttClient.connected() && ( (int)(millis() - startTime) < mqtt_timeout))
  {
    Serial.print(".");

    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println();
      Serial.print("Conectado ao broker MQTT!");
    }
    delay(1000);
  }
  if (!mqttClient.connected())
  {
    Serial.print("Conexão com o broker MQTT não estabelecida!");
  }
  Serial.println();
}

void NodeRedController::initWiFiMQTT()
{
  connectWiFi();
  if (WiFi.status() == WL_CONNECTED)
  {
    configMQTT();
    connectMQTT();
  }
}

void NodeRedController::publish(char *topic, char *msg)
{
  if (!mqttClient.connected())
  {
    connectMQTT();
  }

  if (mqttClient.connected())
  {
    mqttClient.loop();
    mqttClient.publish(topic, msg, true);
  }
}
