/*
    Name:       HeltecOled.ino
    Created:	2019-01-24 20:58:08
    Author:     LAPTOP-FTI3RT4H\gusta
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AzureIotHub.h>
#include <Esp32MQTTClient.h>
#include "OledDisplay.h"


#define DEVICE_ID "DeviceIoT2"


static const char* connectionString = "HostName=assignment-3-IoTHub.azure-devices.net;DeviceId=DeviceIoT2;SharedAccessKey=dZB2s8I/dDJfutKyF77gfn6Tk48lC0b1WqE0DHj4k54=";


static bool hasWifi = false;
static bool ledState = false;

WiFiMulti wifiMulti;
Oled display;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities

void initMultiWifi() {

	wifiMulti.addAP("iPhone x", "hejhejhej");
	wifiMulti.addAP("Caesar's Palace 2.0", "kingofpop");
	wifiMulti.addAP("IoT", "IoT2018!");
	Serial.println("Connecting");
	int temp = 0;
	display.ConnectingWifi(temp);
	while (wifiMulti.run() != WL_CONNECTED) {
		temp++;
		delay(250);
		Serial.print('.');
		display.ConnectingWifi(temp, true);
	}
	Serial.println("\r\n");
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());
	Serial.print("IP address:\t");
	Serial.print(WiFi.localIP());
	Serial.println("\r\n");
	display.wifiConnected(WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
	hasWifi = true;
}
static void MessageCallback(const char* payLoad, int size)
{
	Serial.println("Message callback:");
	Serial.println(payLoad);
	StaticJsonBuffer<300> JSONBuffer;
	JsonObject&  parsed = JSONBuffer.parseObject(payLoad);
	if (parsed.measureLength() < 15) {
		display.setMessage(payLoad);
	}
	else {
		String deviceId = parsed["deviceId"];
		String Temp = parsed["temperature"];
		String Humid = parsed["humidity"];
		if (strcmp(parsed["lamp"], "ON") == 0) {
			ledState = true;
			display.setLampState(true);
		}
		else {
			ledState = false;
			display.setLampState(false);
		}

		display.setCurrentCloud("Azure");
		display.UpdateData(Temp.c_str(), Humid.c_str());
	}
	
	display.UpdateDisplay();

}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
	char *temp = (char *)malloc(size + 1);
	if (temp == NULL)
	{
		return;
	}
	memcpy(temp, payLoad, size);
	temp[size] = '\0';
	// Display Twin message.
	Serial.println(temp);
	free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
	LogInfo("Try to invoke method %s", methodName);
	const char *responseMessage = "\"Successfully invoke device method\"";
	int result = 200;

	if (strcmp(methodName, "LEDON") == 0)
	{
		LogInfo("LED state set to ON");
		responseMessage = "\" Successfully Turned LED ON \"";
		ledState = true;
		display.setLampState(true);
		display.UpdateDisplay();
	}
	else if (strcmp(methodName, "LEDOFF") == 0)
	{
		LogInfo("LED state set to OFF");
		responseMessage = "\" Successfully Turned LED OFF \"";
		ledState = false;
		display.setLampState(false);
		display.UpdateDisplay();
	}
	else
	{
		LogInfo("No method %s found", methodName);
		responseMessage = "\"No method found\"";
		result = 404;
	}

	*response_size = strlen(responseMessage) + 1;
	*response = (unsigned char *)strdup(responseMessage);

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino sketch
void setup()
{
	Serial.begin(115200);
	Serial.println("ESP32 Device");
	Serial.println("Initializing...");
	pinMode(22, OUTPUT);
	// Initialize the WiFi module
	Serial.println(" > WiFi");
	display.WelcomeScreen();

	hasWifi = false;
	initMultiWifi();
	randomSeed(micros());

	Serial.println(" > IoT Hub");
	Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "IoTDevice");
	Esp32MQTTClient_Init((const uint8_t*)connectionString, true);
	Esp32MQTTClient_SetMessageCallback(MessageCallback);
	Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
	Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

}

void loop()
{
	if (hasWifi)
		Esp32MQTTClient_Check();

	if (wifiMulti.run() != WL_CONNECTED) {
		Serial.println("WiFi not connected!");
		display.ConnectingWifi(0);
		delay(1000);
	}

	if (ledState)
		digitalWrite(22, HIGH);
	else
		digitalWrite(22, LOW);

	delay(10);
}
