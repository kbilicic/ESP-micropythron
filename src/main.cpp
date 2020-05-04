#include <Arduino.h>
#include "WebSocketClient.h"
#include "ESP8266WiFi.h"

WebSocketClient ws(true);

void setup() {
	Serial.begin(115200);
	WiFi.begin("kruno", "arduino1");

	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
}

void loop() {
	if (!ws.isConnected()) {
		ws.connect("api.homejinnee.com", "/", 443);
	} else {
		ws.send("hello");

		String msg;
		if (ws.getMessage(msg)) {
			Serial.println(msg);
		}
	}
	delay(500);
}