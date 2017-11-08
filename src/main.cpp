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


extern "C" {
#include "user_interface.h"
  uint16 readvdd33(void);
  bool wifi_set_sleep_type(sleep_type_t);
  sleep_type_t wifi_get_sleep_type(void);
}

const char* ssid = "CloudSenseMobile";
const char* password = "CloudPA55-201707";
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

    int32_t db = getRSSI(ssid);
    uint8_t bars = signalStrength(db);
    drawSignalStrenth(0,40, bars, WiFi.status() == WL_CONNECTED);

    drawCloud(0, 55);
}

// demo only
void drawPersons(int x, int y) {
    display.drawXbm(x, y, persons_width, persons_height, persons_bits);
    display.setFont(Monospaced_plain_10);
    display.drawString(x-4, y + 21, "AUTO");
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
    display.drawString(leftOffset + unitOffset1, 17, tempString.substring(2,4));
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

void printTargetTemp(int t) {
    display.fillRect(0, 0, 32,24);
    display.setColor(BLACK);
    display.setFont(Lato_Semibold_20);
    display.drawString(1, 0, String(t) + "°");
    display.setColor(WHITE);
}



void setup()
{
    pinMode(LDR_PIN, INPUT);
    display.init();
    display.flipScreenVertically();
    display.normalDisplay();
    dht.begin();

    Serial.setDebugOutput(true);
    Serial.begin(115200);

    delay(10);

    WiFi.begin(ssid, password);
    delay(100);
    /*
    
    Serial.setDebugOutput(true);
    

    WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print("Connecting..");
    }
    */

    
}

HTTPClient http;  //Declare an object of class HTTPClient
const char * server = "api.thingspeak.com";
uint16_t port = 80;
String apiKey = "39Y08CVP5FJVM9OZ";

uint16_t counter = 0;
uint16_t intervals = 60;
uint16_t intervalDurationMs = 5000;
bool lastSendToServerSuccess = true;

float temp = 0;
float hum = 0;

void loop()
{
    float t = dht.readTemperature();
    if(!isnan(t))
        temp = t;
    float h = dht.readHumidity();
    if(!isnan(h))
        hum = h;

    int32_t db = getRSSI(ssid);
    uint8_t bars = signalStrength(db);
    counter++;
    
    
    display.clear();
    // OLED printing hereclear

    drawStatusBarLeft();
    drawSignalStrenth(92,48, bars, WiFi.status() == WL_CONNECTED);
    drawPersons(8,29);
    //drawNightMode(5, 30);
    printTargetTemp(24);
    //printCoffeeTime();
    //printTempAndHum();

    if (isnan(h)|| isnan(t)) {
        Serial.println("Invalid value from sensor");
    }

    printLargeTemp(40, temp, hum);
        
    bool sendToServer = bool(counter == intervals);
    bool connectToServer = bool(counter + 1 == intervals);

    if(connectToServer && !WiFi.status()== WL_CONNECTED) {
        WiFi.begin(ssid, password);
        sleep = false;
    }
    
    if(sendToServer) {
        if(WiFi.status()== WL_CONNECTED){
            http.begin("http://rapidcubesapi.cloudapp.net/iotsensorslogs"); 
            http.addHeader("Content-Type", "application/json");
            String tempData = generateJsonObject("temperature", String(temp));
            int httpCode = http.POST(tempData);
            http.end();
            
            String humData = generateJsonObject("humidity", String(hum));
            int httpCode2 = http.POST(humData);
            http.end();

            if (httpCode2 > 0) {
                Serial.println("Transmit: ok");
            } else {
                Serial.print("Transmit: error, ");
                Serial.print(httpCode);
            }
            lastSendToServerSuccess = true;
            blinkCloud(108, 48);
            
            goToLightSleep();
            //ESP.deepSleep(2e6); // 2e6 is 2 seconds
        } else {
            Serial.println("Wifi not connected");
            WiFi.begin(ssid, password);
        }
    }
    if(lastSendToServerSuccess) {
        drawCloud(108, 48);
    }


    /*
    String json = "";
    StaticJsonBuffer<1000> jsonBuffer;
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
        HTTPClient http;  //Declare an object of class HTTPClient
        http.begin("http://jsonplaceholder.typicode.com/users/1");  //Specify request destination
        int httpCode = http.GET();                                                                  //Send the request

        if (httpCode > 0) { //Check the returning code
            json = http.getString();   //Get the request response payload
            JsonObject& root = jsonBuffer.parseObject(json);
            if (!root.success()) {
                Serial.println("parseObject() failed");
                return;
            } else {
                double email  = root["address"]["geo"]["lat"];
                Serial.println(email);
            }
            
            //Serial.println(json);                     //Print the response payload
        }

        http.end();   //Close connection
    }
    */

    
    display.display();

    Serial.println("counter: " + String(counter));
    if(counter == intervals)
        counter = 0;


    /*
    qrcode.init();
    qrcode.create("http://rapidcubesapi.cloudapp.net/iotsensorslogs");
    */

    delay(intervalDurationMs);
    //goToLightSleep();
}