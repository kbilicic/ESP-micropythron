#include <Wire.h>
#include <math.h>
#include <font.h>
#include <images.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <qrcode.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "gpio.h"
#include "FS.h"
#include <WebSocketsClient.h>
//#include <Hash.h>
#include "fauxmoESP.h"
#include <OneButton.h>
#include <Rotary.h>

WebSocketsClient webSocket;


extern "C" {
#include "user_interface.h"
  uint16 readvdd33(void);
  bool wifi_set_sleep_type(sleep_type_t);
  sleep_type_t wifi_get_sleep_type(void);
}


typedef struct ThermostatSettings {
    bool on; // on,off
    String mode; // auto, manual
    String type; // heat, cool. (In the future add ventilate, moisturize, dryout)
    uint8_t targetTemp; // 23
    uint8_t oldTargetTemp;
    String ssid;
    String password;
    String apiUrl;
};

typedef struct Status {
    String deviceState; // initializing, working
    bool running; // true, false - indicates if for example boiler is activly heating
    float temperature;
    float humidity;
    String deviceId;
    time_t time;
    bool webSocketConnected;
    bool changingTemp;
    unsigned long lastChangedTemp;
};

ThermostatSettings settings;
Status status;

bool tempChanged = false, 
    humChanged = false, 
    settingsChanged = false, 
    dataChanged = false;


void changeTemp(uint8_t newTemp) {
    settings.oldTargetTemp = settings.targetTemp;
    status.changingTemp = true;
    tempChanged = true;
    settingsChanged = true;
    status.lastChangedTemp = millis();
    settings.targetTemp = newTemp;
}

//const char* ssid = "CloudSenseMobile";
//const char* password = "CloudPA55-201707";
const char* ssid = "octofus";
const char* password = "990999997995";
const char* deviceId = "DemoThermostat";


#define LDR_PIN A0
#define DHTPIN D5    // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321



SSD1306  display(0x3c, D2, D1);
QRcode qrcode (&display);
DHT dht(DHTPIN, DHTTYPE);


int32_t getRSSI(const char * target_ssid) {
    byte available_networks = WiFi.scanNetworks();

    for (int network = 0; network < available_networks; network++) {
        if (WiFi.SSID(network) == target_ssid) {
            return WiFi.RSSI(network);
        }
    }
    return 0;
}

uint8_t signalStrength(int32_t db) {
    if(db > -50) {
        return 5;
    } else if(db < -50 && db >= -60) {
        return 4;
    } else if(db < -60 && db >= -70) {
        return 3;
    } else if(db < -70 && db >= -80) {
        return 2;
    } else if(db < -80 && db >= -90) {
        return 1;
    } else {
        return 0;
    }
}

void drawSignalStrenth(int x, int y, uint8_t bars, bool connected) {
    if(bars > 4 && connected) display.drawLine(x, y, x+16, y);
    if(bars > 3 && connected) display.drawLine(x + 2, y + 2, x+14, y + 2);
    if(bars > 2 && connected) display.drawLine(x + 4, y + 4, x+12, y + 4);
    if(bars > 1 && connected) display.drawLine(x + 6, y + 6, x+10, y + 6);
    if(bars > 0 && connected) display.drawLine(x + 8,y + 8, x+8, y+ 8);

    if(!connected) {
        display.drawLine(x,y, x+9, y+9);
        display.drawLine(x,y+9, x+9, y);
    }
}

void drawCloud(int x, int y) {
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
}

void blinkCloud(int x, int y) {
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
    display.display();
    delay(300);
    display.setColor(WHITE);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
    display.display();
    delay(300);
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
    display.display();
    delay(300);
    display.setColor(WHITE);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
    display.display();
    delay(300);
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, cloud_small);
    display.display();
    delay(300);
    display.setColor(WHITE);
}

void printTempAndHum() {
    uint16_t temp = 15;
    String tempString;
    String humString;
    uint16_t mainOffset = 23;

    float t = dht.readTemperature();
    float h = dht.readHumidity();
    temp = (uint16_t)t;

    tempString = String(t, (char)1);
    humString = String(h, (char)1);

    display.clear();

    // display temp image
    display.drawXbm(6, 3, thermometer_small_width, thermometer_small_height, thermometer_small_bits);

    // display temp
    display.setFont(DejaVu_Sans_Mono_32);
    display.drawString(mainOffset, 0, tempString);
    uint16_t unitOffset = display.getStringWidth(tempString);
    display.setFont(Nimbus_Sans_L_Regular_Condensed_24);
    display.drawString(mainOffset + unitOffset, 1, "°C");
    
    // display humidity
    display.setFont(DejaVu_Sans_Mono_32);
    display.drawString(mainOffset, 32, humString);
    uint16_t unitOffseth = display.getStringWidth(humString);
    display.setFont(Nimbus_Sans_L_Regular_Condensed_24);
    display.drawString(mainOffset + unitOffseth, 33, "%");

    /*
    int32_t db = getRSSI(ssid);
    uint8_t bars = signalStrength(db);
    drawSignalStrenth(0,40, bars, WiFi.status() == WL_CONNECTED);
    
    drawCloud(0, 55);
    */
}

// demo only
void drawWorkingMode(int x, int y) {
    if(settings.mode == "auto") {
        display.drawXbm(x, y, persons_width, persons_height, persons_bits);
        display.setFont(Monospaced_plain_10);
        display.drawString(x-4, y + 21, "AUTO");
    } else if (settings.mode == "manual") {
        display.drawXbm(x, y, persons_width, persons_height, persons_bits);
        display.setFont(Monospaced_plain_10);
        display.drawString(x-2, y + 21, "MAN");
    }
}

void drawNightMode(int x, int y) {
    display.drawXbm(x, y, moon_width, moon_height, moon_bits);
}

void printLargeTemp(uint8_t leftOffset, float t, float h) {
    String tempString;
    String humString;

    tempString = String(t, (char)1);
    humString = String(h, (char)1);

    display.setFont(Lato_Thin_50);
    display.drawString(leftOffset, -8, tempString.substring(0,2));
    uint16_t unitOffset1 = display.getStringWidth(tempString.substring(0,2));
    display.setFont(Lato_Thin_24);
    display.drawString(leftOffset + unitOffset1, 18, tempString.substring(2,4));
    display.setFont(Lato_Thin_16);
    display.drawString(leftOffset + unitOffset1, 2, "°C");

    display.drawString(leftOffset + 1, 42, humString);
    uint16_t unitOffset2 = display.getStringWidth(humString);
    display.drawString(leftOffset + 1 + unitOffset2, 42, "%");
}

void drawStatusBarLeft() {
    display.drawLine(33,0, 33, 63);
    display.drawLine(0,0, 0, 63);
    display.drawLine(0,0, 127, 0);
    display.drawLine(127,0, 127, 63);
    display.drawLine(0,63, 127, 63);
}

bool sleep = false;

void goToLightSleep() {
    if (!sleep)
    {
        Serial.println("switching off Wifi Modem and CPU");
        //stop any clients
        ///thisclient.stop();
        delay(1000);
        wifi_set_sleep_type(LIGHT_SLEEP_T);
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        // how do we call this: 
        //gpio_pin_wakeup_enable(GPIO_ID_PIN(switchPin),GPIO_PIN_INTR_NEGEDGE);
        //wifi_fpm_open();
        //wifi_fpm_do_sleep(26843455);
        //if(WiFi.forceSleepBegin(26843455)) sleep = true;
    }
}

String generateJsonObject2(String deviceSn, String dateTime, String temp, String hum) {
    StaticJsonBuffer<1000> jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    root["sn"] = deviceSn;
    root["time"] = dateTime;
    root["temp"] = temp;
    root["hum"] = hum;

    char buffer[1000];
    root.printTo((char*)buffer, root.measureLength() + 1);

    return String(buffer);
}

String generateJsonObject(String measure, String value) {
    StaticJsonBuffer<500> jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    root["$type"] = "RapidCubes.Repository.Models.IotSensorsLog, RapidCubes.Repository";
    root["DeviceId"] = deviceId;
    root["Measure"] = measure;
    root["Value"] = value;

    char buffer[500];
    root.printTo((char*)buffer, root.measureLength() + 1);

    return String(buffer);
}

void printTargetTemp() {
    display.fillRect(0, 0, 32,24);
    display.setColor(BLACK);
    display.setFont(Lato_Semibold_20);
    if(settings.on)
        display.drawString(1, 0, String(settings.targetTemp) + "°");
    else
    {
        display.setFont(Lato_Medium_16);
        display.drawString(1, 2, "OFF");
    }
    display.setColor(WHITE);
}




bool loadSettings(ThermostatSettings &settings) {
    File settingsFile = SPIFFS.open("/settings.json", "r");
    if(!settingsFile) {
        return false;
    }
    size_t size = settingsFile.size();
    std::unique_ptr<char[]> buffer(new char[size]);
    settingsFile.readBytes(buffer.get(), size);
    //Serial.println(buffer.get());

    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(buffer.get());

    if (!root.success()) {
        Serial.println("parseObject() failed");
        return false;
    } else {
        settings.targetTemp = root["targetTemp"];
        settings.mode = root["mode"].asString();
        if(settings.mode == NULL)
            settings.mode = "manual";
        settings.type = root["type"].asString();
        if(settings.type == NULL)
            settings.type = "heat";
        settings.on = root["on"];
        settings.apiUrl = root["apiUrl"].asString();
        settings.ssid = root["ssid"].asString();
        settings.password = root["password"].asString();

        Serial.println(settings.targetTemp);
        Serial.println(settings.mode);
        Serial.println(settings.type);
        Serial.println(settings.on);
        Serial.println(settings.apiUrl);
        Serial.println(settings.ssid);
        Serial.println(settings.password);
    }

    settingsFile.close();

    return true;
}

bool saveSettingsFile(ThermostatSettings &settings) {
    File newSettingsFile = SPIFFS.open("/settings.json", "w");
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["apiUrl"]      = settings.apiUrl;
    root["mode"]        = settings.mode;
    root["type"]        = settings.type;
    root["password"]    = settings.password;
    root["ssid"]        = settings.ssid;
    root["on"]          = settings.on;
    root["targetTemp"]  = settings.targetTemp;

    root.printTo(newSettingsFile);
    newSettingsFile.flush();
    newSettingsFile.close();
    
    return SPIFFS.exists("/settings.json");
}

void turnBoilerOnOff() {
    if(!settings.on)
        status.running = false;

    if(status.running)
        digitalWrite(D0, LOW);
    else
        digitalWrite(D0, HIGH);
}

void turnHeatingOnOff(bool isOn) {
    settingsChanged = true;
    settings.on = isOn;
    turnBoilerOnOff();
}

void processPayload(uint8_t *payload) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonObj = jsonBuffer.parseObject(payload);
    if (!jsonObj.success()) {
        Serial.println("parseObject() failed");
        return;
    } else {
        if(jsonObj["temperature"] != NULL) {
            //settings.targetTemp = jsonObj["temperature"];
            changeTemp(jsonObj["temperature"]);
        }
        if(jsonObj["mode"] != NULL) {
            settings.mode = jsonObj["mode"].asString();   // auto, manual
        }
        if(jsonObj["type"] != NULL) {
            settings.type = jsonObj["type"].asString();    // heat, cool. (In the future add ventilate, moisturize, dryout)
        }

        turnHeatingOnOff(bool(jsonObj["on"]));
    }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            status.webSocketConnected = false;
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED: {
            status.webSocketConnected = true;
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            // send message to server when Connected
            
        }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);
            processPayload(payload);

            // send message to server
            // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}


int red = 15;
int green = 12;
int blue = 13;

const char * server = "api.thingspeak.com";
uint16_t port = 80;
String apiKey = "39Y08CVP5FJVM9OZ";

uint32_t intervalDurationMs = 10000;
uint32_t intervalsSendToServer = 20000;
bool lastSendToServerSuccess = true;
unsigned long lastEvent = 0;
unsigned long lastServerEvent = millis();
bool sendToServer = false;
bool processData = true;

float temp = 0;
float hum = 0;

void loopThermostat() {
    processData = false;
    tempChanged = false;
    humChanged = false;
    dataChanged = false;
    sendToServer = false;

    if(status.changingTemp && status.lastChangedTemp + 3000 > millis()) {
        int verticalOff = 0;
        if(settings.oldTargetTemp > settings.targetTemp) {
            for(int i = -2; i > -63; i=i-3) {
                verticalOff = i;
                display.clear();
                display.setFont(URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.oldTargetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.oldTargetTemp));
                display.setFont(Lato_Thin_16);
                display.drawString(35 + unitOffset1, 8, "°C");
                display.display();
            }

            for(int i = 64; i > -2; i=i-3) {
                verticalOff = i;
                display.clear();
                display.setFont(URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.targetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.targetTemp));
                display.setFont(Lato_Thin_16);
                display.drawString(35 + unitOffset1, 8, "°C");
                display.display();
            }

            settings.oldTargetTemp = settings.targetTemp;
        } else if (settings.oldTargetTemp < settings.targetTemp) {
            for(int i = -2; i < 63; i=i+3) {
                verticalOff = i;
                display.clear();
                display.setFont(URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.oldTargetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.oldTargetTemp));
                display.setFont(Lato_Thin_16);
                display.drawString(35 + unitOffset1, 8, "°C");
                display.display();
            }

            for(int i = -63; i < -2; i=i+3) {
                verticalOff = i;
                display.clear();
                display.setFont(URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.targetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.targetTemp));
                display.setFont(Lato_Thin_16);
                display.drawString(35 + unitOffset1, 8, "°C");
                display.display();
            }

            settings.oldTargetTemp = settings.targetTemp;
        }
        //delay(1000);
        
        return;
    } else if (status.changingTemp) {
        display.clear();
        dataChanged = true;
        settingsChanged = true;
        status.changingTemp = false;
    }

    if(lastEvent > millis()) {
        lastServerEvent = 0;
        lastEvent = 0;
    }

    if(lastServerEvent + intervalsSendToServer < millis())
    {
        Serial.println("Period ended... should send to server");
        lastServerEvent = millis();
        sendToServer = true;
        dataChanged = true;
    }

    if(lastEvent + intervalDurationMs < millis())
    {
        Serial.println("Period ended... processing");
        lastEvent = millis();
        processData = true;
        dataChanged = true;
    }

    if(settings.on && dataChanged || settingsChanged) {
        Serial.println("thermostat is on...");
        if(settings.type == "heat" && (tempChanged || settingsChanged)) {
            Serial.println("heating changed...");
            if(status.running) {
                if(temp < (float)settings.targetTemp - 0.5) {
                    status.running = true;
                } else {
                    status.running = false;
                }
            } else {
                if(temp <= (float)settings.targetTemp - 1) {
                    status.running = true;
                } else {
                    status.running = false;
                }
            }

            Serial.println("Running: " + status.running);
        } else if (settings.type == "cool" && (tempChanged || settingsChanged)) {
            if(temp > (float)settings.targetTemp + 1) {
                status.running = true;
            } else {
                status.running = false;
            }
        }
    }

    if(dataChanged) {
        display.clear();
        // OLED printing hereclear
        drawStatusBarLeft();
        drawWorkingMode(8,29);
        //drawNightMode(5, 30);
        printTargetTemp();
        //printTempAndHum();
        printLargeTemp(40, temp, hum);
    }
    
    if(settings.on && processData || dataChanged) {
        lastEvent = millis();

        float t = dht.readTemperature();
        if(!isnan(t))
            temp = t;
        float h = dht.readHumidity();
        if(!isnan(h))
            hum = h;

        if (isnan(h)|| isnan(t)) {
            Serial.println("Invalid value from sensor");
        }

        if(temp != status.temperature) {
            tempChanged = true;
            dataChanged = true;
        }
        if(hum != status.humidity) {
            humChanged = true;
            dataChanged = true;
        }
    
        status.temperature = temp;
        status.humidity = hum;
    
        int32_t db = getRSSI(ssid);
        uint8_t bars = signalStrength(db);
        drawSignalStrenth(92,48, bars, WiFi.status() == WL_CONNECTED);

        
        if(sendToServer && dataChanged) {

            if(WiFi.status()== WL_CONNECTED){

                // prepare JSON
                String tempData = generateJsonObject2("1234567890", "2018-3-31", String(temp), String(hum));
                Serial.print("JSON data: ");
                Serial.println(tempData);

                Serial.println("Sending data to server");
                // post to server
                HTTPClient http;
                //http.begin("http://homestuff.me/devicelogs/"); 
                http.begin("http://192.168.10.136:5000/devicelogs/"); 
                http.addHeader("Content-Type", "application/json");
                int httpCode = http.POST(tempData);
                http.end();

                if (httpCode > 0) {
                    Serial.println("Transmit: ok");
                    lastSendToServerSuccess = true;
                } else {
                    lastSendToServerSuccess = false;
                    Serial.print("Transmit: error, ");
                    Serial.println(http.errorToString(httpCode));
                }

                
                //blinkCloud(108, 48);
                
                //goToLightSleep();
                //ESP.deepSleep(2e6); // 2e6 is 2 seconds
            } else {
                Serial.println("Wifi not connected");
                WiFi.begin(ssid, password);
                delay(100);
            }

            if(lastSendToServerSuccess) {
                blinkCloud(108, 48);
            }
            display.display();
        } else {
            display.display();
        }
        

        if(status.webSocketConnected) {
            webSocket.sendTXT("Temperature: " + String(status.temperature) + "°C");
            webSocket.sendTXT("Humidity: " + String(status.humidity) + "%");
        }

        //settings.targetTemp = 24;
        
        /*
        qrcode.init();
        qrcode.create("http://rapidcubesapi.cloudapp.net/iotsensorslogs");
        */
    }
    //Serial.println("millis: " + String(millis()));

    if(settingsChanged || dataChanged) {
        //printTargetTemp();
        display.display();

        Serial.println("turnBoilerOnOff");
        turnBoilerOnOff(); // depends on settings.running

        dataChanged = false;
        settingsChanged = false;

        saveSettingsFile(settings);
        //Serial.println("Target: " + String(settings.targetTemp) + " is on: " + settings.on + " mode: " + settings.mode + " running: " + String(status.running)); 
    }
}


const byte interruptPin = D8;
volatile byte interruptCounter = 0;
unsigned long lastButtonEvent = 0;
int numberOfInterrupts = 0;

void handleInterrupt() {
    if(lastButtonEvent + 300 > millis())
        return;
    interruptCounter++;
    changeTemp(settings.targetTemp + 1);
    lastButtonEvent = millis();
}


fauxmoESP fauxmo;


void setup()
{
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
    pinMode(LDR_PIN, INPUT);
    display.init();
    display.flipScreenVertically();
    display.normalDisplay();
    dht.begin();

    Serial.setDebugOutput(true);
    Serial.begin(115200);

    delay(10);

    pinMode(red, OUTPUT);
    pinMode(blue, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(D0, OUTPUT);

    if(!SPIFFS.begin()) {
        digitalWrite(blue, HIGH);
    } else {

        Serial.println("...");
        Serial.println("Loading settings file...");

        if(loadSettings(settings)) {
            digitalWrite(green, HIGH);
        } else {
            digitalWrite(red, HIGH);
        }
    }

    float t = dht.readTemperature();
    if(!isnan(t))
        temp = t;
    float h = dht.readHumidity();
    if(!isnan(h))
        hum = h;

    settings.on = false;
    turnBoilerOnOff();
    settingsChanged = true;
    tempChanged = true;
    status.deviceState = "Initializing";

    WiFi.begin(ssid, password);
    delay(100);
    while(!WiFi.status()== WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(1000);
        sleep = false;
        Serial.print(".");
    }

    /*
    Serial.setDebugOutput(true);
    */

    // WEBSOCKETS
    // server address, port and URL
	webSocket.begin("192.168.10.136", 8888);
    // event handler
    webSocket.onEvent(webSocketEvent);
    // use HTTP Basic Authorization this is optional remove if not needed
    //webSocket.setAuthorization("user", "Password");
    // try ever 5000 again if connection has failed
    webSocket.setReconnectInterval(5000);

    //fauxmo.addDevice("relay");


    if(status.deviceState == "Initializing") {
        Serial.println("print welcome on screen...");
        display.setFont(Lato_Thin_16);
        display.drawString(35, 8, "Welcome!");
        display.display();
    }
    else {
        Serial.println("status.deviceState: " + status.deviceState);
    }
}

void loop()
{
    if(WiFi.status() == WL_CONNECTED)
        webSocket.loop();

    loopThermostat();

    //fauxmo.handle();
}