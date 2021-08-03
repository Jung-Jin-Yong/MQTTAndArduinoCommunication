// Basic demo for accelerometer readings from Adafruit MPU6050

// ESP32 Guide: https://RandomNerdTutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/
// ESP8266 Guide: https://RandomNerdTutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/
// Arduino Guide: https://RandomNerdTutorials.com/arduino-mpu-6050-accelerometer-gyroscope/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Adafruit_MPU6050 mpu;

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

char MQTT_send_data[100] = "";

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  //와이파이 셋팅
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

  Serial.println("");
  
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
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

double a_Acceleration_x, a_Acceleration_y, a_Acceleration_z;
double g_gyro_x, g_gyro_y, g_gyro_z;
double temp_temperature;
void loop() {
  client.loop();
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  a_Acceleration_x = a.acceleration.x;
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  a_Acceleration_y = a.acceleration.y;
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  a_Acceleration_z = a.acceleration.z;
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  g_gyro_x = g.gyro.x;
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  g_gyro_y = g.gyro.y;
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  g_gyro_z = g.gyro.z;
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  temp_temperature = temp.temperature;
  Serial.println(" degC");

  Serial.println("");
  
  sprintf(MQTT_send_data, "Acceleration : %.2lf %.2lf %.2lf m/s^2\nRotation : %.2lf %.2lf %.2lf rad/s\nTemperature : %.2lf degC", a_Acceleration_x, a_Acceleration_y, a_Acceleration_z, g_gyro_x, g_gyro_y, g_gyro_z, temp_temperature); //데이터값 입력하는 부분
  client.publish("1003rocket/pub", MQTT_send_data);

  delay(500);
}
