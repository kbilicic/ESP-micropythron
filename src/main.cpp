#include <Wire.h>
#include <math.h>
#include "font.h"
#include "images.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <qrcode.h>
//#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "gpio.h"
#include "FS.h"
#include <WebSocketsClient.h>
#include <OneButton.h>
#include <Rotary.h>
#include <Hash.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP280.h>

#define P0 1012 // calibratie Thorbeckestraat 33, Leiden, zolder




WebSocketsClient webSocket;


extern "C" {
#include "user_interface.h"
  uint16 readvdd33(void);
  bool wifi_set_sleep_type(sleep_type_t);
  sleep_type_t wifi_get_sleep_type(void);
}

uint16_t ac_temp_cool[2][227] = {
    // temp = 24
    {3412, 1582,  490, 1164,  466, 1164,  466, 336,  466, 336,  490, 338,  438, 1190,  466, 336,  466, 336,  486, 1170,  436, 1194, 438, 364,  438, 1192,  458, 350,  452, 350,  452, 1198,  434, 1194,  458, 350,  450, 1180,  452, 1180,  454, 348,  398, 426,  374, 1256,  448, 354,  448, 354,  462, 1190,  464, 338,  440, 362,  440, 362,  466, 362,  440, 362,  440, 360,  466, 336,  464, 364,  438, 364,  438, 364,  438, 364,  450, 356, 448, 354,  446, 356,  446, 356,  474, 354,  448, 356,  446, 1184,  452, 350,  400, 428,  374, 1258,  450, 352,  450, 352,  472, 1184,  450, 1180,  452, 350,  452, 350,  476, 352,  452, 350,  450, 352,  450, 350,  472, 1184,  450, 1180,  452, 1198,  434, 368,  458, 350,  452, 348,  452, 352,  394, 406, 476, 352,  450, 352,  394, 408,  450, 1180,  420, 1236,  452, 1198,  434, 368,  434, 368,  402, 406,  452, 350,  452, 350,  452, 350,  476, 350,  452,350,  452, 350,  450, 350,  472, 356,  374, 428,  446, 356,  446, 356,  474, 354,  444, 356,  446, 356,  446, 356,  480, 350,  450, 352,  394, 408,  394, 408,  470, 356,  448, 354,  446, 358,  442, 358,  468, 358,  448, 354,  446, 356,  448, 354,  400, 426,  376, 426,  374, 428,  374, 428,  474, 1184,444, 1188,  452, 350,  394, 1236,  472, 1184,  374, 1258,  452, 1180,  396, 406,  454},
    // temp = 25
    {3306, 1692,  398, 1258,  374, 1256,  374, 428,  374, 428,  406, 422,  380, 1252,  372, 430,  372, 430,  404, 1248,  382, 1250, 382, 420,  380, 1252,  406, 420,  382, 420,  382, 1248,  382, 1248,  408, 420,  382, 1248,  382, 1248,  382, 420,  406, 422,  380, 1250,  382, 420,  382, 420,  398, 1258,  376, 426,  376, 426,  376, 426,  354, 452,  352, 450,  372, 430,  372, 430,  404, 420,  382, 418,  382, 420,  382, 422,  408, 420, 382, 420,  382, 420,  382, 420,  408, 420,  382, 420,  382, 1250,  382, 422,  406, 420,  382, 1250,  382, 420,  382, 420,  408, 1248,  382, 1248,  382, 420,  382, 420,  406, 420,  382, 420,  382, 420,  382, 420,  408, 420,  382, 1248,  382, 1248,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,408, 420,  382, 420,  382, 420,  382, 1250,  406, 1248,  382, 1248,  382, 420,  382, 420,  408, 422,  380, 420,  382, 420,  382, 420,  408, 420,  382, 420,  380, 422,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  406, 422,  380, 422,  380, 422,  380, 422,  408, 420,  382, 420,  382, 420,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  406, 422,  380, 422,  380, 420,  380, 422,  404, 424,  378, 424,  378, 424,  378, 424,  406, 422,  380, 1250,  382, 420,  382, 1248,  404, 1252,  380, 1252,  380, 1250,  382, 420,  382}
};

uint16_t ac_temp_heat[2][227] = {
    // temp = 24
};

uint16_t ac_off[227] = {3330, 1664,  406, 1250,  382, 1248,  382, 420,  382, 420,  408, 420,  382, 1248,  382, 420,  382, 420,  402, 1256,  378, 1252, 380, 422,  380, 1250,  378, 428,  374, 428,  374, 1278,  374, 1256,  376, 430,  372, 1258,  374, 1256,  376, 430,  376, 448,  352, 1278,  352, 450,  352, 450,  404, 1250,  380, 422,  380, 422,  380, 422,  406, 420,  382, 420,  382, 420,  382, 420,  404, 424,  378, 424,  376, 426,  376, 424,  376, 430, 372, 430,  372, 430,  374, 428,  398, 428,  372, 430,  374, 430,  372, 430,  376, 452,  352, 1278,  372, 430,  372, 430,  448, 1208,  372, 1258,  374,428,  374, 428,  398, 430,  372, 430,  372, 430,  372, 430,  452, 376,  426, 1204,  374, 1258,  374, 428,  398, 430,  372, 428,  372, 430,  372, 430,  452, 376,  352, 450,  352, 450,  426, 1206,  378, 1278,  372, 1258,  374, 428,  374, 428,  400, 428,  372, 430,  372, 430,  372, 430,  378, 450,  352, 450,  352, 450,  352, 450,  378, 450,  352, 450,  352, 450,  352, 450,  404, 424,  378, 422,  380, 424,  378, 422,  452, 378,  352, 450,  424, 376,  352,450,  378, 450,  352, 450,  372, 430,  352, 450,  404, 422,  380, 422,  380, 422,  380, 422,  406, 422,  380, 422,  380, 422,  380, 422,  406, 424,  378, 1252,  352, 1278,  352, 450,  404, 1252,  378, 1252,  352, 1278,  422, 380,  372}; 

typedef enum AC_MODE{
    COOL,
    HEAT,
    OFF
};

uint16_t* getIrCode(int temp, AC_MODE mode) {
    if(mode == COOL)
        return (uint16_t*)ac_temp_cool[temp - 15];
    else if(mode == HEAT)
        return (uint16_t*)ac_temp_heat[temp - 15];
    else
        return (uint16_t*)ac_off;
};


////////////////////////////////////////////  INITAL SETTINGS  /////////////////////////////////////////////

// DEFAULT VALUES
//char* ssid = "B.net_97432";
//char* password = "36nrdhjum2yv";
//char* ssid = "octofus";
//char* password = "990999997995";
//const char* ssid = "CloudSenseMobile";
//const char* password = "CloudPA55-201707";
const char* ssid = "kruno";
const char* password = "arduino1";
String deviceModel = "HST1";
String deviceManufacturer = "HomeStuff";
String deviceSn = "1234567890";
//const char* deviceSn = "HST118-4L6MA5A2C4";
////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////  PIN ASSIGNMENT  //////////////////////////////////////////////
// GPIO4  - D1 - OLED
// GPIO5  - D2 - OLED
// GPIO12 - D6 - green - MOVE_DOWN
// GPIO13 - D7 - blue - SELECT
// GPIO14 - D5 - DHTPIN
// GPIO15 - D8 - red - MOVE_UP
// GPIO16 - D0 - RELAY
// GPIO2  - D4 - builtin LED
#define DEBUG_ESP_SSL
#define DEBUG_ESP_PORT
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


#define HEATING_RELAY D0
#define OLED_PIN1 D1
#define OLED_PIN2 D2 
#define LED D4      // LED or IR sensor
#define DHTPIN D5   // GPIO14
#define LDR_PIN A0
#define MOVE_UP D3      // GPIO15 - D8 - red
#define MOVE_DOWN D6    // GPIO12 - D6 - green
#define SELECT D7       // GPIO13 - D7 - blue


// As this program is a special purpose capture/decoder, let us use a larger
// than normal buffer so we can handle Air Conditioner remote codes.
uint16_t CAPTURE_BUFFER_SIZE = 1024;
IRrecv irrec(LED, CAPTURE_BUFFER_SIZE, 60, false);
IRsend irsend(MOVE_DOWN);



uint16_t fan_on_off[67] = {8892, 4490,  518, 580,  540, 578,  536, 586,  518, 580,  542, 576,  534, 588,  518, 580,  542, 1698,  544, 1672,  514, 1706,  542, 1698,  518, 1698,  514, 1704,  542, 1698,  518, 1700,  514, 608,  518, 1698,  514, 606,  518, 578,  542, 576,  514, 608,  518, 580,  540, 578,  514,608,  518, 580,  542, 1698,  518, 1698,  534, 1684,  542, 1698,  544, 1674,  534, 1684,  542, 1698,  518};

uint16_t ac_on_25[227] = {3306, 1692,  398, 1258,  374, 1256,  374, 428,  374, 428,  406, 422,  380, 1252,  372, 430,  372, 430,  404, 1248,  382, 1250, 382, 420,  380, 1252,  406, 420,  382, 420,  382, 1248,  382, 1248,  408, 420,  382, 1248,  382, 1248,  382, 420,  406, 422,  380, 1250,  382, 420,  382, 420,  398, 1258,  376, 426,  376, 426,  376, 426,  354, 452,  352, 450,  372, 430,  372, 430,  404, 420,  382, 418,  382, 420,  382, 422,  408, 420, 382, 420,  382, 420,  382, 420,  408, 420,  382, 420,  382, 1250,  382, 422,  406, 420,  382, 1250,  382, 420,  382, 420,  408, 1248,  382, 1248,  382, 420,  382, 420,  406, 420,  382, 420,  382, 420,  382, 420,  408, 420,  382, 1248,  382, 1248,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,408, 420,  382, 420,  382, 420,  382, 1250,  406, 1248,  382, 1248,  382, 420,  382, 420,  408, 422,  380, 420,  382, 420,  382, 420,  408, 420,  382, 420,  380, 422,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  406, 422,  380, 422,  380, 422,  380, 422,  408, 420,  382, 420,  382, 420,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  406, 422,  380, 422,  380, 420,  380, 422,  404, 424,  378, 424,  378, 424,  378, 424,  406, 422,  380, 1250,  382, 420,  382, 1248,  404, 1252,  380, 1252,  380, 1250,  382, 420,  382};
uint16_t ac_on_24[227] = {3412, 1582,  490, 1164,  466, 1164,  466, 336,  466, 336,  490, 338,  438, 1190,  466, 336,  466, 336,  486, 1170,  436, 1194, 438, 364,  438, 1192,  458, 350,  452, 350,  452, 1198,  434, 1194,  458, 350,  450, 1180,  452, 1180,  454, 348,  398, 426,  374, 1256,  448, 354,  448, 354,  462, 1190,  464, 338,  440, 362,  440, 362,  466, 362,  440, 362,  440, 360,  466, 336,  464, 364,  438, 364,  438, 364,  438, 364,  450, 356, 448, 354,  446, 356,  446, 356,  474, 354,  448, 356,  446, 1184,  452, 350,  400, 428,  374, 1258,  450, 352,  450, 352,  472, 1184,  450, 1180,  452, 350,  452, 350,  476, 352,  452, 350,  450, 352,  450, 350,  472, 1184,  450, 1180,  452, 1198,  434, 368,  458, 350,  452, 348,  452, 352,  394, 406, 476, 352,  450, 352,  394, 408,  450, 1180,  420, 1236,  452, 1198,  434, 368,  434, 368,  402, 406,  452, 350,  452, 350,  452, 350,  476, 350,  452,350,  452, 350,  450, 350,  472, 356,  374, 428,  446, 356,  446, 356,  474, 354,  444, 356,  446, 356,  446, 356,  480, 350,  450, 352,  394, 408,  394, 408,  470, 356,  448, 354,  446, 358,  442, 358,  468, 358,  448, 354,  446, 356,  448, 354,  400, 426,  376, 426,  374, 428,  374, 428,  474, 1184,444, 1188,  452, 350,  394, 1236,  472, 1184,  374, 1258,  452, 1180,  396, 406,  454}; 
////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct MenuItem {
    int index;
    String title;
    char ImageData[];
};

MenuItem menu[2];
MenuItem mi_display;
MenuItem mi_settings;
/*mi_display.index = 0;
mi_display.title = "Display";
mi_display.ImageData = leaf_bits;

mi_settings.index = 1;
mi_settings.title = "Settings";
mi_settings.ImageData = persons_bits;
menu[1] = mi_settings;
*/

typedef struct ThermostatSettings {
    String on; // on,off
    String mode; //"off,heat,cool,auto,evo,on"
    uint8_t targetTemp; // 23
    uint8_t targetTempLow;
    uint8_t targetTempHigh;
    uint8_t oldTargetTemp;
    String ssid;
    String password;
    String apiUrl;
    String wifiConfigured;
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
    bool changingMode;
    unsigned long lastChangedMode;
    String lastEventName;
    unsigned long lastEventMillis;
    int animationIndex;
    bool sentToServer;
    unsigned long lastSentToServer;
    bool eventCoolingOnOff;
    bool eventHeatingOnOff;
    bool dataChanged;
    bool settingsChanged;
    bool tempChanged;
    bool humChanged;
};

ThermostatSettings settings;
Status status;




SSD1306  display(0x3c, OLED_PIN2, OLED_PIN1); // GPIO5, GPIO4
QRcode qrcode (&display);
DHT dht(DHTPIN, DHTTYPE); // GPIO14


/*
int32_t getRSSI(String target_ssid) {
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
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
}

void blinkCloud(int x, int y) {
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
    display.display();
    delay(300);
    display.setColor(WHITE);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
    display.display();
    delay(300);
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
    display.display();
    delay(300);
    display.setColor(WHITE);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
    display.display();
    delay(300);
    display.setColor(BLACK);
    display.drawXbm(x, y, cloud_small_width, cloud_small_height, (uint8_t *)cloud_small);
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
    display.drawXbm(6, 3, thermometer_small_width, thermometer_small_height, (uint8_t *)thermometer_small_bits);

    // display temp
    display.setFont((uint8_t *)DejaVu_Sans_Mono_32);
    display.drawString(mainOffset, 0, tempString);
    uint16_t unitOffset = display.getStringWidth(tempString);
    display.setFont((uint8_t *)Nimbus_Sans_L_Regular_Condensed_24);
    display.drawString(mainOffset + unitOffset, 1, "°C");
    
    // display humidity
    display.setFont((uint8_t *)DejaVu_Sans_Mono_32);
    display.drawString(mainOffset, 32, humString);
    uint16_t unitOffseth = display.getStringWidth(humString);
    display.setFont((uint8_t *)Nimbus_Sans_L_Regular_Condensed_24);
    display.drawString(mainOffset + unitOffseth, 33, "%");

    
    int32_t db = getRSSI(ssid);
    uint8_t bars = signalStrength(db);
    drawSignalStrenth(0,40, bars, WiFi.status() == WL_CONNECTED);
    
    drawCloud(0, 55);
    
}


// demo only
void drawWorkingMode(int x, int y) {
    display.setColor(BLACK);
    if(settings.mode == "auto") {
        display.drawXbm(x, y, persons_width, persons_height, (uint8_t *)persons_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        display.drawString(x-4, y + 21, "AUTO");
    } else if (settings.mode == "eco") {
        display.drawXbm(x-5, y-6, leaf_width, leaf_height, (uint8_t *)leaf_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        display.drawString(x-2, y + 22, "ECO");
    } else if (settings.mode == "manual") {
        display.drawXbm(x, y, persons_width, persons_height, (uint8_t *)persons_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        display.drawString(x-2, y + 21, "MAN");
    }
    else if (settings.mode == "heat") {
        display.drawXbm(x-1, y, fire_width, fire_height, (uint8_t *)fire_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        display.drawString(x-4, y + 22, "HEAT");
    }
    else if (settings.mode == "cool") {
        display.drawXbm(x-1, y+2, cool_width, cool_height, (uint8_t *)cool_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        display.drawString(x-4, y + 22, "COOL");
    }
    display.setColor(WHITE);
}



void drawNightMode(int x, int y) {
    display.drawXbm(x, y, moon_width, moon_height, (uint8_t *)moon_bits);
}


void printLargeTemp(uint8_t leftOffset, float t, float h) {
    String tempString;
    String humString;

    tempString = String(t, (char)1);
    humString = String(h, (char)1);

    display.setFont((uint8_t *)Lato_Thin_50);
    display.drawString(leftOffset, -8, tempString.substring(0,2));
    uint16_t unitOffset1 = display.getStringWidth(tempString.substring(0,2));
    display.setFont((uint8_t *)Lato_Thin_24);
    display.drawString(leftOffset + unitOffset1, 18, tempString.substring(2,4));
    display.setFont((uint8_t *)Lato_Thin_16);
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

    display.fillRect(0, 0, 34, 64);
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

*/


String generateJsonStatusObject(String deviceSn, String dateTime, String targetTemp, String targetLowTemp, String targetHighTemp, String temp, String hum, String mode) {
    StaticJsonBuffer<250> jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    root["sn"] = deviceSn;
    root["time"] = dateTime;
    root["temp"] = temp;
    root["hum"] = hum;
    root["target_temp"] = targetTemp;
    root["target_temp_low"] = targetLowTemp;
    root["target_temp_high"] = targetHighTemp;
    root["mode"] = mode;
    root["event"] = status.lastEventName;
    char buffer[250];
    root.printTo((char*)buffer, root.measureLength() + 1);

    return String(buffer);
}


/*
void displayToScreen2(float temp, float hum, float pr, float alt) {
    display.clear();
    display.setFont((uint8_t *)DejaVu_Sans_Mono_Bold_8);
    display.drawString(5, 1, "T: " + temp.asString() + " C");
    display.drawString(5, 10, "H: " + hum.asString() + " %");
    display.drawString(5, 19, "P: " + pr.asString() + " HPa");
    display.drawString(5, 28, "A: " + alt.asString() + " m");
    display.display();
}
*/

void displayToScreen2(float temp, float hum) {
    //Serial.println("Printing to screen....");

    int x = 0, y = 4;
    if(settings.mode == "auto") {
        display.drawXbm(x, y, persons_width, persons_height, (uint8_t *)persons_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        //display.drawString(x-4, y + 21, "AUTO");
    } else if (settings.mode == "eco") {
        display.drawXbm(x-5, y-6, leaf_width_32, leaf_height_32, (uint8_t *)leaf_bits_32);
        display.setFont((uint8_t *)Monospaced_plain_10);
        //display.drawString(x-2, y + 22, "ECO");
    } else if (settings.mode == "manual") {
        display.drawXbm(x, y, persons_width, persons_height, (uint8_t *)persons_bits);
        display.setFont((uint8_t *)Monospaced_plain_10);
        //display.drawString(x-2, y + 21, "MAN");
    }
    else if (settings.mode == "heat") {
        display.drawXbm(x, y, fire_width_32, fire_height_32, (uint8_t *)fire_bits_32);
        display.setFont((uint8_t *)Monospaced_plain_10);
        //display.drawString(x-4, y + 22, "HEAT");
    }
    else if (settings.mode == "cool") {
        if(status.running)
            status.animationIndex++;
        status.animationIndex = status.animationIndex % 6;
        //Serial.println("Animate");
        display.drawXbm(x-1, y+2, snowflake_width_32, snowflake_height_32, (uint8_t *)snow_animation_bits[status.animationIndex]);
        display.setFont((uint8_t *)Monospaced_plain_10);
        //display.drawString(x-4, y + 22, "COOL");
    }

    //drawNightMode(5, 30);
    //printTempAndHum();
    int leftOffset = 45;
    String tempString;
    String humString;

    tempString = String(temp, (char)1);
    humString = String(hum, (char)1);

    display.setFont((uint8_t *)URW_Gothic_L_Book_53);
    display.drawString(leftOffset, -2, tempString.substring(0,2));
    uint16_t unitOffset1 = display.getStringWidth(tempString.substring(0,2));
    display.setFont((uint8_t *)URW_Gothic_L_Demi_22);
    display.drawString(leftOffset + unitOffset1, 28, tempString.substring(2,4));
    display.setFont((uint8_t *)URW_Gothic_L_Book_16);
    display.drawString(leftOffset + unitOffset1, 8, "°C");

    display.setFont((uint8_t *)Lato_Semibold_20);

    if(settings.on == "true")
        display.drawString(0, 35, String(settings.targetTemp) + "°");
    else
        display.drawString(0, 35, "OFF");

    //display.drawRect(0,0,128,64);
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

        settings.on = "false";
        settings.mode = "heat";
        settings.targetTemp = 22;

        return false;
    } else {
        settings.targetTemp = root["targetTemp"];
        settings.targetTempLow = root["targetTempLow"];
        settings.targetTempHigh = root["targetTempHigh"];

        settings.mode = root["mode"].asString();
        if(settings.mode == NULL)
            settings.mode = "manual";
        //settings.type = root["type"].asString();
        //if(settings.type == NULL)
        //    settings.type = "heat";
        settings.on = root["on"].asString();
        settings.apiUrl = root["apiUrl"].asString();
        settings.ssid = root["ssid"].asString();
        settings.password = root["password"].asString();

        Serial.println("Loaded settings: ");
        Serial.println("settings.targetTemp = " + settings.targetTemp);
        Serial.println("settings.mode = " + settings.mode);
        //Serial.println("settings.type = " + settings.type);
        Serial.println("settings.on = " + settings.on);
        Serial.println("settings.apiUrl = " + settings.apiUrl);
        Serial.println("settings.ssid = " + settings.ssid);
        Serial.println("settings.password = " + settings.password);
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
    root["password"]    = settings.password;
    root["ssid"]        = settings.ssid;
    root["on"]          = settings.on;
    root["targetTemp"]  = settings.targetTemp;
    root["targetTempLow"]  = settings.targetTempLow;
    root["targetTempHigh"]  = settings.targetTempHigh;

    root.printTo(newSettingsFile);
    newSettingsFile.flush();
    newSettingsFile.close();
    
    return SPIFFS.exists("/settings.json");
}

void turnBoilerOnOff() {
    if(settings.on == "false")
        status.running = false;

    if(status.eventHeatingOnOff) {
        if(status.running && settings.mode == "heat")
            digitalWrite(HEATING_RELAY, LOW); // torn ON heating
        else
            digitalWrite(HEATING_RELAY, HIGH); // torn OFF heating

        status.eventHeatingOnOff = false;
        Serial.println("eventHeatingOnOff...");
    }
    
    if(status.eventCoolingOnOff) {
        if(status.running && settings.mode == "cool")
            irsend.sendRaw(getIrCode(settings.targetTemp, COOL), 227, 38);// torn ON heating
            //irsend.sendNEC(0x9D713C88, 114);
        else
            irsend.sendRaw(ac_off, 227, 38);// torn OFF heating
        
        status.eventCoolingOnOff = false;
        Serial.println("eventCoolingOnOff...");
    }
}

void turnHeatingOnOff(String isOn) {
    Serial.println("turnHeatingOnOff...");
    status.settingsChanged = true;
    settings.on = isOn;
    Serial.println("settings.on = " + settings.on);
    turnBoilerOnOff();
}

void changeTemp(uint8_t newTemp) {
    settings.oldTargetTemp = settings.targetTemp;
    status.changingTemp = true;
    status.tempChanged = true;
    status.settingsChanged = true;
    status.lastChangedTemp = millis();
    settings.targetTemp = newTemp;
    status.lastEventName = "TARGET_TEMP_CHANGED";
    status.lastEventMillis = millis();
}

void changeMode(String newMode) {
    if(newMode == "off") {
        turnHeatingOnOff("false");
    } else {
        Serial.println("Changing mode to: " + newMode);
        if(newMode != "on")
            settings.mode = newMode;
        settings.on = "true";
        turnHeatingOnOff("true");
    }

    status.changingMode = true;
    status.settingsChanged = true;
    status.lastChangedMode = millis();
    status.lastEventName = "MODE_CHANGED";
    status.lastEventMillis = millis();
}



void processPayload(uint8_t *payload) {
    StaticJsonBuffer<300> jsonBuffer;
    jsonBuffer.clear();
    JsonObject& jsonObj = jsonBuffer.parseObject(payload);
    if (!jsonObj.success()) {
        Serial.println("parseObject() failed");
        return;
    } else {
        String temp = jsonObj["temp"].asString();
        String mode = jsonObj["mode"].asString();
        //String type = jsonObj["type"].asString();
        String status_on = jsonObj["on"].asString();

        Serial.println("temp: " + temp);
        Serial.println("mode: " + mode);
        //Serial.println("type: " + type);
        Serial.println("status_on: " + status_on);

        if(temp != NULL && temp != String(settings.targetTemp)) {
            //settings.targetTemp = jsonObj["temperature"];
            changeTemp(jsonObj["temp"]);
            Serial.println("temp changed to: " + temp);
            status.dataChanged = true;
        }
        if(mode != NULL && mode != settings.mode || settings.on == "false") {
            changeMode(mode);
            Serial.println("mode changed to: " + mode);
            status.dataChanged = true;
        }
        /*if(type != NULL) {
            settings.type = type;    // heat, cool. (In the future add ventilate, moisturize, dryout)
            Serial.println("type changed to: " + type);
        }*/
        if(status_on != NULL) {
            Serial.println("on changed to: " + status_on);
            settings.on = status_on;
            status.changingTemp = true;
            status.dataChanged = true;
            turnHeatingOnOff(status_on);
        }
    }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            status.webSocketConnected = false;
            Serial.printf("[WS] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            status.webSocketConnected = true;
            Serial.printf("[WS] Connected to url: %s\n", payload);
            // send message to server when Connected
            webSocket.sendTXT("{ \"sn\" : \"" + deviceSn + "\", \"event\" : \"CONNECTED\" }");
            break;
        case WStype_TEXT:
            Serial.printf("[WS] data from WS server: %s\n", payload);
            processPayload(payload);

            // send message to server
            // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WS] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

const char * server = "api.thingspeak.com";
uint16_t port = 80;
String apiKey = "39Y08CVP5FJVM9OZ";

unsigned long  intervalDurationMs = 500;
unsigned long  minimumIntervalSendToServer = 30000;
unsigned long  intervalsSendToServer = 45000;
bool lastSendToServerSuccess = true;
unsigned long lastEvent = 0;
unsigned long lastServerEvent = millis();
bool sendToServer = false;
bool processData = true;
unsigned long periodBeginMillis;

float temp = 0;
float hum = 0;

void loopThermostat() {
    processData = false;
    status.tempChanged = false;
    status.humChanged = false;
    //status.dataChanged = false;
    sendToServer = false;

    // Animation for changing target temp
    if(status.changingTemp && status.lastChangedTemp + 3000 > millis()) {
        int verticalOff = 0;
        if(settings.oldTargetTemp > settings.targetTemp) {
            for(int i = -2; i > -63; i=i-5) {
                verticalOff = i;
                display.clear();
                display.setFont((uint8_t *)URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.oldTargetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.oldTargetTemp));
                display.setFont((uint8_t *)Lato_Thin_16);
                display.drawString(35 + unitOffset1, 7, "°C");
                display.display();
            }

            for(int i = 64; i > -2; i=i-5) {
                verticalOff = i;
                display.clear();
                display.setFont((uint8_t *)URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.targetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.targetTemp));
                display.setFont((uint8_t *)Lato_Thin_16);
                display.drawString(35 + unitOffset1, 7, "°C");
                display.display();
            }

            settings.oldTargetTemp = settings.targetTemp;
        } else if (settings.oldTargetTemp < settings.targetTemp) {
            for(int i = -2; i < 63; i=i+5) {
                verticalOff = i;
                display.clear();
                display.setFont((uint8_t *)URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.oldTargetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.oldTargetTemp));
                display.setFont((uint8_t *)Lato_Thin_16);
                display.drawString(35 + unitOffset1, 7, "°C");
                display.display();
            }

            for(int i = -63; i < -2; i=i+5) {
                verticalOff = i;
                display.clear();
                display.setFont((uint8_t *)URW_Gothic_L_Book_53);
                display.drawString(35, verticalOff, String(settings.targetTemp));
                uint16_t unitOffset1 = display.getStringWidth(String(settings.targetTemp));
                display.setFont((uint8_t *)Lato_Thin_16);
                display.drawString(35 + unitOffset1, 7, "°C");
                display.display();
            }

            settings.oldTargetTemp = settings.targetTemp;
        }
        //delay(1000);
        
        return;
    } else if (status.changingTemp) {
        display.clear();
        status.dataChanged = true;
        status.settingsChanged = true;
        status.changingTemp = false;
    }

    // Animation for changing thermostat mode
    if(status.changingMode && status.lastChangedMode + 6000 > millis()) {
        if(settings.on == "false") {
            display.clear();
            display.setFont((uint8_t *)DejaVu_Sans_Mono_32);
            display.drawString(32, 18, "OFF");
            display.display();
        } else if(settings.mode == "cool") {
            display.clear();
            display.drawXbm(48, 6, cool_width_32, cool_height_32, (uint8_t *)cool_bits_32);
            display.setFont((uint8_t *)Lato_Medium_16);
            display.drawString(30, 42, "COOLING");
            display.display();
        } else if(settings.mode == "heat") {
            display.clear();
            display.drawXbm(50, 6, fire_width_27, fire_height_32, (uint8_t *)fire_bits_32);
            display.setFont((uint8_t *)Lato_Medium_16);
            display.drawString(30, 42, "HEATING");
            display.display();
        } else if(settings.mode == "eco") {
            display.clear();
            display.drawXbm(50, 6, leaf_width, leaf_height, (uint8_t *)leaf_bits);
            display.setFont((uint8_t *)Lato_Medium_16);
            display.drawString(27, 42, "ECO MODE");
            display.display();
        }
        return;
    } else if (status.changingMode) {
        display.clear();
        status.dataChanged = true;
        status.settingsChanged = true;
        status.changingMode = false;
    }


    // log events timestamps
    if(lastEvent > millis()) {
        lastServerEvent = 0;
        lastEvent = 0;
    }
    if(lastServerEvent + intervalsSendToServer < millis())
    {
        lastServerEvent = millis();
        sendToServer = true;
    }
    if(lastEvent + intervalDurationMs < millis())
    { 
        lastEvent = millis();
        processData = true;
    }

    
    // update status and events for later processing
    if(settings.on == "true" && status.dataChanged || status.settingsChanged) {
        if(settings.mode == "heat" && (status.tempChanged || status.settingsChanged)) {
            Serial.println("heating changed...");
            if(status.running) {
                if(temp < (float)settings.targetTemp - 0.5) {
                    if(!status.running) {
                        status.eventHeatingOnOff = true;
                        status.running = true;
                    }
                } else {
                    if(status.running) {
                        status.eventHeatingOnOff = true;
                        status.running = false;
                    }
                }
            } 
            /*
            else {
                if(temp <= (float)settings.targetTemp - 1) {
                    status.running = true;
                } else {
                    status.running = false;
                }
            }
            */

            Serial.println("Running: " + String(status.running));
        } 
        else 
        if (settings.mode == "cool" && (status.tempChanged || status.settingsChanged)) {
            if(temp > (float)settings.targetTemp + 1) {
                if(!status.running) {
                    status.eventCoolingOnOff = true;
                    status.running = true;
                }
            } else {
                if(status.running) {
                    status.eventCoolingOnOff = true;
                    status.running = false;
                }
            }
        }
    }

    if(processData) 
    {
        display.clear();
        displayToScreen2(temp, hum);
    }
    

    // read values from sensor
    if(settings.on == "true" && processData || status.dataChanged || sendToServer) {
        lastEvent = millis();

        //////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////

        float t = dht.readTemperature();
        if(!isnan(t))
            temp = t;
        float h = dht.readHumidity();
        if(!isnan(h))
            hum = h;

        if (isnan(h)|| isnan(t)) {
            Serial.println("Invalid value from sensor");
        }

        
        // humidity change by 1%
        if(String(hum, (char)0) != String(status.humidity, (char)0)) {
            status.humChanged = true;
            status.dataChanged = true;
            status.lastEventName = "HUM_CHANGED";
            status.lastEventMillis = millis();
        }
        // temp change by 0.1 degree
        if(String(temp, (char)1) != String(status.temperature, (char)1)) {
            status.tempChanged = true;
            status.dataChanged = true;
            status.lastEventName = "TEMP_CHANGED";
            status.lastEventMillis = millis();
        }
    
        status.temperature = temp;
        status.humidity = hum;
    
        /*
        int32_t db = getRSSI(settings.ssid);
        uint8_t bars = signalStrength(db);
        drawSignalStrenth(92,48, bars, WiFi.status() == WL_CONNECTED);
        display.display();
        */

        if(status.webSocketConnected && (status.dataChanged || sendToServer)) {
            String tempData = generateJsonStatusObject(deviceSn, "2018-3-31", String(settings.targetTemp), String(settings.targetTempLow), String(settings.targetTempHigh), String(temp), String(hum), String(settings.mode));
            webSocket.sendTXT(tempData);
            status.sentToServer = true;
            status.lastSentToServer = millis();
            if(sendToServer)
                Serial.println("Sending data to WS server... Periodic send");
            else if(status.dataChanged)
                Serial.println("Sending data to WS server... data changed");
        } else if(status.webSocketConnected && !status.dataChanged) { 
            //Serial.println("Nothing changed...");
        } else if (!status.webSocketConnected){
            Serial.println("Websocket is not connected...");
            //WiFi.mode(WIFI_OFF);
            //WiFi.mode(WIFI_STA);
            /*Serial.println("Connecting to Wifi " + String(settings.ssid) + ", pass:" + String(settings.password));
            WiFi.begin(settings.ssid.c_str(), settings.password.c_str());
            delay(100);
            while(WiFi.status() != WL_CONNECTED) {
                //WiFi.begin(ssid, password);
                delay(500);
                //sleep = false;
                Serial.print(".");
                status.deviceState = "Reconnecting";
            }
            if(status.deviceState == "Reconnecting") {
                status.deviceState = "Connected";
                delay(3000);
            }*/
        }

        //settings.targetTemp = 24;
        
        /*
        qrcode.init();
        qrcode.create("http://rapidcubesapi.cloudapp.net/iotsensorslogs");
        */
    }
    //Serial.println("millis: " + String(millis()));


    // turn heating/cooling devices on/off
    if(status.settingsChanged || status.dataChanged)
        turnBoilerOnOff();

    // update variables, save settings file
    if(status.settingsChanged || status.dataChanged || processData || sendToServer) {
        status.dataChanged = false;
        status.settingsChanged = false;
        processData = false;
        if(sendToServer)
            sendToServer = false;

        saveSettingsFile(settings);
    }

    // delay and draw to display
    unsigned long periodDelay = intervalDurationMs - (millis() - periodBeginMillis);
    if(periodDelay > intervalDurationMs)
        periodDelay = 0;

    delay(periodDelay);
    display.display();
}



volatile byte interruptCounter = 0;
unsigned long lastButtonEvent = 0;
int numberOfInterrupts = 0;

void interrupt_MoveUp() {
    if(lastButtonEvent + 300 > millis())
        return;
    interruptCounter++;
    changeTemp(settings.targetTemp + 1);
    lastButtonEvent = millis();
    Serial.println("interrupt_MoveUp");
}

void interrupt_MoveDown() {
    if(lastButtonEvent + 300 > millis())
        return;
    interruptCounter++;
    changeTemp(settings.targetTemp - 1);
    lastButtonEvent = millis();
    Serial.println("interrupt_MoveDown");
}

void interrupt_Select() {
    if(lastButtonEvent + 300 > millis())
        return;
    interruptCounter++;
    //changeTemp(settings.targetTemp + 1);
    lastButtonEvent = millis();
    Serial.println("interrupt_Select");
}


void setup()
{
    pinMode(MOVE_UP, INPUT_PULLUP);
    //pinMode(MOVE_DOWN, INPUT_PULLUP);
    pinMode(SELECT, INPUT_PULLUP);
    pinMode(HEATING_RELAY, OUTPUT); // GPIO16 - RELAY
    //pinMode(LED, OUTPUT);
    pinMode(LED, INPUT);
    pinMode(LDR_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(MOVE_UP), interrupt_MoveUp, FALLING);
    //attachInterrupt(digitalPinToInterrupt(MOVE_DOWN), interrupt_MoveDown, FALLING);
    attachInterrupt(digitalPinToInterrupt(SELECT), interrupt_Select, FALLING);
    
    display.init();
    display.flipScreenVertically();
    display.normalDisplay();
    dht.begin();

    Serial.setDebugOutput(true);
    Serial.begin(115200);

    delay(10);


    
    
    status.deviceState = "Initializing";

    // LOAD SETTINGS FROM FILE SYSTEM
    if(!SPIFFS.begin()) {
        Serial.println("Error accessing disk!!!");
    } else {

        Serial.println("...");
        Serial.print("Loading settings file...");

        if(loadSettings(settings)) {
            //digitalWrite(green, HIGH);
            Serial.println("Ok");
        } else {
            //digitalWrite(red, HIGH);
            Serial.println("Error");
        }
    }
    if(settings.ssid == NULL || settings.ssid == "" || settings.ssid != ssid)
        settings.ssid = ssid;
    if(settings.password == NULL || settings.password == "" || settings.password != password)
        settings.password = password;
    if(settings.wifiConfigured == NULL)
        settings.wifiConfigured = "false";

    float t = dht.readTemperature();
    if(!isnan(t))
        temp = t;
    float h = dht.readHumidity();
    if(!isnan(h))
        hum = h;

    turnBoilerOnOff();
    status.settingsChanged = true;
    status.tempChanged = true;
    



    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////////      WiFi      ///////////////////////////
    if(settings.ssid.c_str() != "") {
        status.deviceState = "Connecting";
        display.setFont((uint8_t *)Lato_Thin_16);
        display.drawString(35, 20, "Connecting");
        display.display();
        Serial.println("Connecting to Wifi " + String(settings.ssid)+ ", pass:" + String(settings.password));
        WiFi.begin(settings.ssid.c_str(), settings.password.c_str());
        delay(100);
        int i = 0;
        while(WiFi.status() != WL_CONNECTED) {
            display.clear();
            Serial.print(".");
            display.drawXbm(34, 3, WiFi_Logo_width, WiFi_Logo_height, (uint8_t *)WiFi_Logo_bits);
            //display.drawString(35, 20, "Connecting");
            delay(350);

            /*
            if(i % 2 == 0) {
                digitalWrite(LED, HIGH);
            } else {
                digitalWrite(LED, LOW);
            }
            */
            
            if(i % 3 == 0) {
                display.fillCircle(44, 52, 3);
                display.drawCircle(64, 52, 3);
                display.drawCircle(84, 52, 3);
            }
            else if(i % 3 == 1) {
                display.drawCircle(44, 52, 3);
                display.fillCircle(64, 52, 3);
                display.drawCircle(84, 52, 3);
            }
            else if(i % 3 == 2) {
                display.drawCircle(44, 52, 3);
                display.drawCircle(64, 52, 3);
                display.fillCircle(84, 52, 3);
            }
                
            display.display();
            i++;
        }
        //digitalWrite(LED, HIGH);
        Serial.println("\nConnected!");
        display.clear();
        display.drawString(35, 20, "Connected!");
        status.deviceState = "Connected";
        display.display();
        //delay(2000);
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ///////////////////////////   WPS   /////////////////////////////
    else {
        //startWPSPBC();
    }

    /*
    Serial.setDebugOutput(true);
    */

    // WEBSOCKETS
    // server address, port and URL
	webSocket.beginSSL("api.homestuff.me",443,"/");
    // event handler
    webSocket.onEvent(webSocketEvent);
    // use HTTP Basic Authorization this is optional remove if not needed
    //webSocket.setAuthorization("user", "Password");
    // try ever 5000 again if connection has failed
    webSocket.setReconnectInterval(10000);

    changeTemp(settings.targetTemp);

    irrec.enableIRIn();
    irsend.begin();
}




decode_results irresults;



// Display encoding type
//
void encoding(decode_results *results) {
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break;
    case NEC:          Serial.print("NEC");           break;
    case NEC_LIKE:     Serial.print("NEC (non-strict)");  break;
    case SONY:         Serial.print("SONY");          break;
    case RC5:          Serial.print("RC5");           break;
    case RC5X:         Serial.print("RC5X");          break;
    case RC6:          Serial.print("RC6");           break;
    case RCMM:         Serial.print("RCMM");          break;
    case DISH:         Serial.print("DISH");          break;
    case SHARP:        Serial.print("SHARP");         break;
    case JVC:          Serial.print("JVC");           break;
    case SANYO:        Serial.print("SANYO");         break;
    case SANYO_LC7461: Serial.print("SANYO_LC7461");  break;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break;
    case SAMSUNG:      Serial.print("SAMSUNG");       break;
    case LG:           Serial.print("LG");            break;
    case WHYNTER:      Serial.print("WHYNTER");       break;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break;
    case PANASONIC:    Serial.print("PANASONIC");     break;
    case DENON:        Serial.print("DENON");         break;
    case COOLIX:       Serial.print("COOLIX");        break;
    case NIKAI:        Serial.print("NIKAI");         break;
  }
  if (results->repeat) Serial.print(" (Repeat)");
}

// Dump out the decode_results structure.
//
void dumpInfo(decode_results *results) {
  if (results->overflow)
    Serial.printf("WARNING: IR code too big for buffer (>= %d). "
                  "These results shouldn't be trusted until this is resolved. "
                  "Edit & increase CAPTURE_BUFFER_SIZE.\n",
                  CAPTURE_BUFFER_SIZE);

  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}

uint16_t getCookedLength(decode_results *results) {
  uint16_t length = results->rawlen - 1;
  for (uint16_t i = 0; i < results->rawlen - 1; i++) {
    uint32_t usecs = results->rawbuf[i] * RAWTICK;
    // Add two extra entries for multiple larger than UINT16_MAX it is.
    length += (usecs / UINT16_MAX) * 2;
  }
  return length;
}

// Dump out the decode_results structure.
//
void dumpRaw(decode_results *results) {
  // Print Raw data
  Serial.print("Timing[");
  Serial.print(results->rawlen - 1, DEC);
  Serial.println("]: ");

  for (uint16_t i = 1; i < results->rawlen; i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    if (i % 2 == 0) {  // even
      Serial.print("-");
    } else {  // odd
      Serial.print("   +");
    }
    Serial.printf("%6d", results->rawbuf[i] * RAWTICK);
    if (i < results->rawlen - 1)
      Serial.print(", ");  // ',' not needed for last one
    if (!(i % 8)) Serial.println("");
  }
  Serial.println("");  // Newline
}

// Dump out the decode_results structure.
//
void dumpCode(decode_results *results) {
  // Start declaration
  Serial.print("uint16_t ");               // variable type
  Serial.print("rawData[");                // array name
  Serial.print(getCookedLength(results), DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (uint16_t i = 1; i < results->rawlen; i++) {
    uint32_t usecs;
    for (usecs = results->rawbuf[i] * RAWTICK;
         usecs > UINT16_MAX;
         usecs -= UINT16_MAX)
      Serial.printf("%d, 0", UINT16_MAX);
    Serial.print(usecs, DEC);
    if (i < results->rawlen - 1)
      Serial.print(", ");  // ',' not needed on last one
    if (i % 2 == 0) Serial.print(" ");  // Extra if it was even.
  }

  // End declaration
  Serial.print("};");  //

  // Comment
  Serial.print("  // ");
  encoding(results);
  Serial.print(" ");
  serialPrintUint64(results->value, HEX);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {
    // Some protocols have an address &/or command.
    // NOTE: It will ignore the atypical case when a message has been decoded
    // but the address & the command are both 0.
    if (results->address > 0 || results->command > 0) {
      Serial.print("uint32_t address = 0x");
      Serial.print(results->address, HEX);
      Serial.println(";");
      Serial.print("uint32_t command = 0x");
      Serial.print(results->command, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("uint64_t data = 0x");
    serialPrintUint64(results->value, 16);
    Serial.println(";");
  }
}




void loop()
{
    
    periodBeginMillis = millis();
    if(WiFi.status() == WL_CONNECTED)
        webSocket.loop();

    loopThermostat();
    
    
    
    if(irrec.decode(&irresults)) {
        dumpInfo(&irresults);           // Output the results
        dumpRaw(&irresults);            // Output the results in RAW format
        dumpCode(&irresults);           // Output the results as source code
        Serial.println("");
        irrec.resume();  // Receive the next value

        
        //irsend.sendRaw(vol_down, 99, 38);
        //irsend.sendPanasonic(0x1000405, 48);
        //irsend.sendPanasonic(0x4004, 0x400401008485);
    }
    //delay(100);
    
    
}