#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Enter your wifi credentials
const char* ssid = "nyong";
const char* password =  "321321321";

//Enter your mqtt server configurations
const char* mqttServer = "driver.cloudmqtt.com";    //Enter Your mqttServer address
const int mqttPort = 18659;       //Port number
const char* mqttUser = "qgigavut"; //User
const char* mqttPassword = "vhLIo8HOH6wQ"; //Password

WiFiClient espClient;
PubSubClient client(espClient);

char MQTT_send_data[20] = "";
int a = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());

  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);

    }
  }

  client.publish("1003rocket/pub", "Hello from ESP8266");
  client.subscribe("1003rocket/sub");

}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  //Conver *byte to String
  }
  Serial.println(message);
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
  sprintf(MQTT_send_data, "%s %2d", "rocketMotorValue : ", a); //데이터값 입력하는 부분
  client.publish("1003rocket/pub", MQTT_send_data);
  a++;
  if (a > 30) {
    a = 0;
  }
  delay(1000);
}
