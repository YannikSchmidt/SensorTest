/*
 Name:		SensorTest.ino
 Created:	22.10.2022 11:25:57
 Author:	maily
*/
/*
// the setup function runs once when you press reset or power the board
int echo= 10;

int trig= 9;

void setup() {

	pinMode(trig, OUTPUT);

	pinMode(echo, INPUT);

}

void loop(){

	digitalWrite(trig, LOW);

	delayMicroseconds(2);

	digitalWrite(trig, HIGH);

	delayMicroseconds(10);

	float distance = pulseIn(echo, HIGH);

	Serial.println(distance);

	delay(1);

}
*/

#include <dummy.h>
#include <PubSubClient.h>
#include <SimpleDHT.h> 
#include <ArduinoOTA.h>
#include <string.h> 
#include <iostream>

using namespace std;
using std::string;


#define wifi_ssid "Angeldrive Studios 2,4"
#define wifi_password "Garten123!"
#define mqtt_server "192.168.178.183"
#define mqtt_user "mqtt"         
#define mqtt_password "Garten123!"

#define ESPHostname "Arduino Test"
#define data_topic "esp01/data"
#define inTopic "esp01/inTopic"
#define outTopic "esp01/outTopic"

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {

	Serial.begin(115200);
	pinMode(4, INPUT);
	
	setup_wifi();
	ArduinoOTA.setHostname(ESPHostname);
	// ArduinoOTA.setPassword("admin");
	ArduinoOTA.begin();

	client.setServer(mqtt_server, 1883);

} 



void loop() {
	static bool boolValue;

	boolValue = digitalRead(4);
	string value1 = "On";
	string value2 = "Off";

	mqttPublish(toggelButton(boolValue, value1, value2));
	delay(100);
}


string toggelButton(bool signal,string value1, string value2) {
	static string lastValue;
	static bool lastSignal;
	static bool toggle;
	Serial.print("signal:"); Serial.println(signal);
	if (signal) {
		if (signal != lastSignal) {
			lastSignal = signal;
			if (!toggle) {
				toggle = true; lastValue = value1; Serial.print("toggle ON"); Serial.println(toggle); return value1;
			}
			else {
				toggle = false; lastValue = value2; Serial.print("toggle OFF"); Serial.println(toggle); return value2;
			}
		}
		
	}
	else lastSignal = false;
	return lastValue;
}

string toggelSwitch(bool signal, string value1, string value2) {
	static bool lastSignal;
	static bool toggle = true;

	if (signal) return value1;
	if (!signal) return value2;
}

void mqttPublish(string payload) {
	static string printValue;
	static string lastPrint;

	if (!client.connected()) reconnect();

	if (lastPrint != payload) {
		client.publish(data_topic, payload.c_str(), false);

		lastPrint = payload;
	}
	
}

void mqttPublish(string payload, const char *topic) {

	if (!client.connected()) reconnect();

	client.publish(topic, payload.c_str(), false);
}

void setup_wifi() {
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(wifi_ssid);
	WiFi.begin(wifi_ssid, wifi_password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = "ESP01-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish(outTopic, ESPHostname);
			// ... and resubscribe
			client.subscribe(inTopic);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

/*
int pinDHT11 = 2;

SimpleDHT11 dht11;

void DHT11Loop() {

	Serial.println("======================"); byte temperature = 0;
	byte humidity = 0;
	byte data[40] = { 0 };
	if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
		Serial.print("FEHLER!");
		return;
	} for (int i = 0; i < 40; i++) {
		Serial.print((int)data[i]);
		if (i > 0 && ((i + 1) % 4) == 0) {
			Serial.print(' ');
		}
	}
	Serial.println(""); Serial.print((int)temperature); Serial.print(" *C, ");
	//Serial.print((int)humidity); Serial.println(" %"); delay(1000);
	
}*/
