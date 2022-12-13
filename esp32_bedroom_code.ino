#include <WiFi.h>
#include <PubSubClient.h>

#define LED 2
#define PRESENCE_SENSOR 23

//PRESENCE SENSOR
int pinStatus;

// WiFi
const char *ssid = "42"; // Enter your WiFi name
const char *password = "F&Atdj4#PCAN";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.mqttdashboard.com";
const char *topic_leds = "10336661_lights"; //topic das luzes
const char *topic_led2 = "10336661_light_2"; //topic das luzes
const char *topic_presence = "10336661_presence"; //topic de presenca
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 pinMode (LED, OUTPUT);
 pinMode (PRESENCE_SENSOR, INPUT_PULLDOWN);
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic_leds, "Hi EMQX I'm ESP32 ^^");
 client.subscribe(topic_leds);
 client.subscribe(topic_led2);
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 String data = "";
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     data += (char)payload[i];
 }
 Serial.println();
 Serial.println("-----------------------");
if(data=="ON"){
  Serial.println("LED");
 digitalWrite(LED, HIGH);
}
else{
  digitalWrite(LED, LOW);
}
}

void loop() {
 client.loop();
 pinStatus = digitalRead(PRESENCE_SENSOR);
 if(pinStatus){
   client.publish(topic_presence, "ON");
 }
 else {
   client.publish(topic_presence, "OFF");
 }
 delay(50);
}