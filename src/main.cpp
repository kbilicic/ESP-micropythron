#include <Wire.h>
#include <math.h>
#include "font.h"
#include "images.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <qrcode.h>
//#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#include <ArduinoJson.h>
#include "Adafruit_Sensor.h"
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

uint16_t ac_temp_cool[15][227] = {
    // on, cool, 16, 546F0A20, 114 bits
    {3330, 1668,  420, 1236,  424, 1206,  396, 406,  422, 380,  404, 422,  406, 1226,  404, 398,  404, 398,  430, 1222,  410, 1222, 408, 394,  408, 1224,  404, 420,  410, 392,  408, 1222,  382, 1248,  434, 392,  410, 1222,  380, 1250,  382, 420,  434, 394,  380, 1250,  410, 392,  408, 394,  430, 1226,  378, 426,  376, 426,  378, 424,  356, 450,  352, 450,  396, 406,  372, 430,  432, 392,  408, 394,  408, 394,  408, 394,  434, 394, 382, 420,  410, 392,  382, 420,  436, 392,  382, 420,  410, 1220,  408, 394,  436, 392,  408, 1222,  392, 410,  408, 394,  436, 1220,  410, 1220,  410, 392,  408, 394,  432, 394,  382, 420,  410, 394,  408, 392,  460, 1196,  410, 1222,  406, 1224,  410, 1222,  436, 390,  408, 394,  382, 420,  410, 392,  434, 394,  410, 390,  410, 392,  436, 1194,  436, 1220,  410, 1220,  412, 390,  410, 392,  462, 364,  412, 390,  438, 364,  412, 390,  438, 390,  410, 392,  436, 366,  436, 364,  436, 392,  412, 390,  438, 364,  412, 392,  462, 366,  436, 364,  436, 366,  436, 366,  436, 392,  436, 366,  410, 392,  412, 390,  436, 392,  436, 366,  438, 364,  410, 392,  462, 366,  410, 392,  436, 366,  412, 390,  462, 366,  412, 390,  412, 390,  436, 366,  436, 1222, 436, 1194,  410, 392,  410, 390,  438, 390,  410, 392,  436, 366,  410, 1220,  436},
    // on, cool, 17, D083F90, 114 bits
    {3386, 1610,  432, 1220,  438, 1192,  438, 364,  436, 366,  464, 364,  412, 1220,  436, 366,  438, 364,  432, 1224,  408, 1222, 410, 392,  436, 1194,  430, 376,  426, 376,  426, 1226,  408, 1224,  376, 430,  374, 1258,  424, 1206,  426, 376,  402, 426,  376, 1256,  370, 430,  372, 430,  434, 1220,  412, 390,  436, 366,  434, 368,  436, 390,  438, 364,  438, 366,  436, 366,  434, 394,  434, 368,  408, 394,  408, 394,  376, 430, 372, 430,  422, 380,  372, 430,  398, 430,  372, 430,  372, 1258,  424, 378,  398, 430,  372, 1258,  424, 378,  424, 378,  398, 1258,  424, 1206,  426, 376,  426, 376,  448, 378,  374, 428,  424, 378,  424, 376,  448, 380,  420, 1210,  372, 1258,  424, 1228,  432, 376,  424, 376,  428, 376,  424, 376, 450, 378,  424, 378,  424, 378,  424, 1206,  400, 1258,  426, 1226,  406, 396,  406, 396,  430, 376,  426, 376,  426, 378,  424, 376,  450, 376,  426,376,  424, 380,  422, 378,  448, 380,  372, 430,  422, 380,  422, 382,  376, 452,  372, 430,  420, 382,  372, 430,  450, 378,  424, 378,  420, 382,  424, 378,  398, 430,  372, 430,  372, 430,  372, 430,  398, 430,  370, 432,  372, 430,  370, 432,  398, 428,  372, 430,  374, 428,  372, 430,  398, 430,  372, 1258,  372, 430,  422, 380,  460, 368,  410, 392,  434, 368,  410, 1222,  372},
    // on, cool, 18, 8FE6DF55, 114 bits
    {3360, 1636,  432, 1222,  434, 1196,  408, 394,  410, 392,  436, 392,  434, 1196,  410, 392,  412, 390,  428, 1228,  406, 1226, 408, 394,  406, 1224,  400, 406,  424, 378,  424, 1226,  404, 1226,  426, 380,  396, 1236,  424, 1206,  424, 378,  432, 394,  406, 1226,  352, 450,  350, 452,  432, 1220,  410, 392,  410, 392,  408, 394,  434, 392,  410, 392,  410, 392,  436, 366,  434, 394,  408, 394,  408, 394,  378, 424,  376, 432, 396, 404,  398, 404,  396, 406,  398, 430,  398, 404,  398, 1232,  426, 376,  424, 404,  398, 1232,  426, 376,  426, 376,  450, 1206,  424, 1208,  428, 374,  428, 374,  452, 376,  426, 376,  424, 378,  426, 376,  424, 1232,  426, 376,  426, 1204,  426, 1226,  432, 374,  428, 374,  428, 374,  428, 374, 426, 402,  398, 404,  426, 376,  424, 1206,  452, 1204,  428, 1222,  434, 368,  408, 394,  436, 372,  430, 372,  428, 374,  428, 374,  426, 402,  426,376,  426, 374,  426, 376,  424, 404,  398, 404,  396, 406,  398, 404,  376, 450,  374, 428,  372, 430,  352, 450,  452, 376,  424, 378,  400, 402,  398, 404,  424, 404,  400, 402,  372, 430,  374, 428,  400, 428,  374, 428,  374, 430,  372, 430,  460, 368,  434, 368,  432, 370,  408, 394,  400, 1258,398, 404,  374, 428,  400, 402,  460, 366,  434, 368,  408, 394,  406, 1226,  374},
    // on, cool, 19, 497ACFE, 114 bits
    {3360, 1638,  458, 1198,  406, 1224,  426, 378,  428, 392,  436, 392,  410, 1224,  398, 426,  404, 398,  432, 1224,  408, 1222, 436, 366,  410, 1220,  436, 392,  410, 392,  356, 1300,  406, 1224,  428, 378,  426, 1206,  424, 1206,  456, 366,  436, 392,  410, 1224,  394, 430,  408, 394,  432, 1224,  408, 394,  408, 394,  408, 394,  400, 406,  394, 408,  394, 408,  424, 378,  456, 370,  408, 394,  408, 394,  406, 396,  434, 390, 410, 392,  412, 390,  410, 392,  462, 366,  410, 390,  436, 1194,  410, 392,  436, 392,  434, 1196,  412, 390,  410, 392,  436, 1222,  408, 1222,  356, 450,  424, 378,  458, 390,  410, 392,  412, 390,  410, 392,  432, 394,  408, 394,  408, 1222,  410, 1220,  436, 392,  410, 392,  434, 368,  408, 394,460, 368,  408, 394,  412, 390,  410, 1222,  436, 1220,  410, 1222,  376, 430,  426, 376,  456, 394,  410, 392,  408, 394,  410, 392,  434, 394,  434, 368,  408, 394,  410, 392,  432, 396,  410, 394,  406, 398,  394, 428,  432, 376,  432, 390,  410, 392,  356, 452,  456, 392,  408, 394,  406, 394,  410, 392,  432, 396,  412, 392,  410, 396,  396, 428,  430, 376,  432, 390,  410, 392,  376, 430,  420, 408,  424, 378,  396, 406,  396, 406,  452, 376,  430, 392,  410, 392,  354, 452,  446, 402,  432, 370,  410, 394,  410, 1222,  412},
    // on, cool, 20, 55B33787, 114 bits
    {3384, 1614,  458, 1200,  376, 1254,  398, 404,  426, 374,  432, 390,  436, 1196,  408, 394,  408, 394,  436, 1218,  410, 1220, 410, 392,  410, 1220,  462, 366,  410, 390,  436, 1196,  412, 1218,  460, 366,  410, 1220,  412, 1218,  410, 392,  460, 368,  408, 1222,  410, 392,  408, 392,  404, 1232,  426, 376,  426, 376,  400, 402,  432, 394,  434, 368,  408, 394,  436, 366,  458, 366,  408, 392,  412, 390,  412, 390,  464, 364, 410, 392,  436, 366,  410, 390,  438, 390,  436, 366,  412, 1220,  410, 390,  438, 390,  412, 1220,  410, 392,  436, 366,  462, 1194,  436, 1196,  412, 390,  412, 390,  436, 392,  410, 392,  438, 364,  408, 392,  438, 1218,  418, 1214,  438, 364,  412, 1218,  436, 392,  438, 364,  412, 390,  410, 392, 438, 390,  412, 390,  436, 366,  438, 1192,  438, 1218,  410, 1220,  412, 390,  410, 392,  462, 366,  414, 388,  410, 392,  438, 364,  436, 390,  410,392,  412, 390,  412, 390,  438, 390,  436, 366,  408, 394,  436, 366,  436, 392,  412, 392,  434, 366,  410, 392,  462, 366,  412, 390,  410, 392,  436, 366,  436, 392,  436, 366,  436, 364,  412, 390,  434, 394,  410, 392,  408, 394,  410, 392,  436, 410,  416, 368,  408, 392,  436, 366,  436, 1220,412, 1220,  436, 1194,  436, 1194,  462, 1194,  412, 1218,  412, 1220,  412, 390,  410},
    // on, cool, 21, 451366EF, 114 bits
    {3330, 1668,  404, 1252,  350, 1280,  372, 430,  372, 430,  406, 420,  380, 1250,  382, 420,  380, 422,  408, 1248,  382, 1250, 382, 420,  382, 1248,  408, 418,  382, 420,  382, 1248,  382, 1248,  408, 420,  382, 1250,  382, 1250,  382, 420,  402, 426,  376, 1254,  378, 424,  378, 422,  376, 1258,  374, 428,  374, 426,  374, 428,  404, 422,  380, 404,  396, 422,  380, 422,  406, 420,  382, 420,  382, 420,  382, 420,  408, 420, 380, 422,  380, 422,  380, 422,  406, 422,  380, 420,  380, 1250,  382, 420,  406, 422,  380, 1250,  382, 420,  380, 422,  406, 1250,  382, 1248,  382, 420,  382, 420,  410, 418,  382, 420,  382, 420,  382, 420,  406, 422,  380, 1250,  382, 420,  382, 1248,  408, 420,  380, 422,  380, 422,  382, 420,406, 422,  382, 420,  380, 422,  380, 1250,  406, 1250,  382, 1250,  380, 422,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  408, 420,  382, 422,  380, 422,  380, 422,  404, 424,  378, 424,  378, 424,  378, 422,  402, 426,  376, 426,  376, 426,  376, 426,  406, 422,  380, 422,  380, 422,  380, 420,  406, 422,  380, 422,  380, 424,  378, 424,  402, 426,  376, 426,  376, 426,  376, 426,  378, 428,  396, 406,  394, 406,  396, 406,  424, 426,  376, 1254,  378, 1252,  382, 1250,  402, 1254,  378, 1252,  382, 1250,  382, 420,  382},
    // on, cool, 22, 8503CC05, 114 bits
    {3332, 1668,  404, 1250,  352, 1278,  372, 430,  352, 450,  404, 420,  382, 1250,  380, 422,  380, 422,  404, 1250,  382, 1248, 380, 422,  380, 1250,  408, 420,  382, 420,  380, 1250,  382, 1250,  406, 422,  382, 1250,  382, 1248,  382, 420,  402, 426,  376, 1254,  378, 424,  378, 424,  376, 1260,  372, 428,  374, 430,  372, 428,  404, 424,  378, 424,  378, 424,  380, 422,  404, 422,  382, 420,  380, 420,  382, 422,  406, 422, 380, 422,  380, 422,  380, 422,  406, 422,  380, 422,  380, 1250,  382, 420,  406, 422,  382, 1250,  382, 420,  382, 420,  406, 1250,  380, 1250,  382, 420,  400, 402,  406, 422,  382, 420,  382, 420,  382, 420,  406, 1250,  382, 420,  382, 420,  382, 1248,  408, 422,  382, 420,  382, 420,  382, 420,406, 422,  380, 422,  380, 422,  380, 1250,  406, 1250,  382, 1248,  382, 420,  382, 420,  406, 420,  382, 420,  382, 420,  382, 420,  406, 420,  380, 422,  382, 420,  380, 422,  404, 424,  378, 424,  378, 424,  378, 424,  400, 426,  376, 426,  376, 426,  376, 426,  406, 422,  380, 422,  380, 422,  380, 420,  404, 424,  378, 424,  378, 424,  378, 424,  402, 426,  376, 426,  376, 426,  376, 426,  398, 408,  394, 408,  394, 408,  394, 408,  422, 1256,  378, 424,  378, 1252,  380, 1250,  402, 1256,  378, 1252,  380, 1252,  382, 420,  380},
    // on, cool, 23, C86A016D, 114 bits
    {3330, 1666,  402, 1254,  352, 1278,  352, 450,  424, 378,  404, 420,  380, 1252,  380, 422,  382, 420,  406, 1248,  382, 1250, 382, 420,  380, 1250,  406, 420,  382, 418,  382, 1250,  382, 1250,  406, 422,  382, 1250,  382, 1248,  382, 420,  402, 426,  376, 1254,  380, 424,  378, 424,  376, 1258,  374, 428,  374, 428,  374, 428,  404, 424,  380, 422,  380, 422,  378, 422,  406, 420,  382, 420,  382, 420,  382, 420,  406, 422, 380, 422,  380, 422,  380, 420,  408, 422,  380, 422,  380, 1250,  380, 422,  406, 420,  382, 1250,  382, 420,  382, 422,  406, 1250,  382, 1250,  382, 420,  382, 420,  408, 420,  382, 420,  382, 420,  382, 420,  406, 422,  380, 422,  380, 422,  380, 1250,  406, 422,  380, 420,  382, 422,  380, 422,  404, 424,  378, 424,  378, 424,  378, 1252,  404, 1252,  382, 1250,  382, 420,  382, 420,  406, 422,  380, 420,  380, 422,  380, 422,  404, 424,  378, 424,  378, 422,  378, 424,  402, 426,  374, 426,  376, 426,  376, 426,  398, 408,  374, 428,  374, 428,  396, 406,  426, 424,  380, 424,  378, 424,  378,424,  402, 426,  376, 426,  376, 426,  376, 426,  378, 428,  394, 408,  394, 408,  394, 408,  398, 428,  374, 428,  374, 428,  374, 428,  400, 428,  396, 406,  394, 1258,  376, 1256,  376, 1258,  394, 1258,  376, 1254,  378, 424,  380},
    // on, cool, 24, 222BA713, 114 bits
    {3358, 1662,  400, 1256,  376, 1254,  406, 396,  406, 396,  400, 406,  424, 1206,  394, 408,  394, 408,  432, 1224,  404, 1226, 352, 452,  350, 1280,  432, 392,  410, 392,  384, 1248,  408, 1222,  432, 392,  434, 1196,  410, 1222,  408, 392,  436, 390,  408, 1222,  410, 392,  410, 392,  460, 1196,  434, 366,  410, 392,  410, 392,  428, 378,  426, 376,  424, 378,  424, 376,  434, 394,  432, 370,  406, 394,  408, 394,  430, 394, 438, 364,  408, 394,  412, 390,  460, 366,  410, 392,  410, 1220,  410, 392,  438, 390,  410, 1220,  410, 392,  410, 390,  460, 1196,  436, 1196,  410, 392,  410, 394,  434, 390,  412, 392,  410, 392,  410, 390,  460, 1196,  412, 1220,  408, 1224,  410, 392,  458, 368,  406, 394,  436, 366,  410, 392, 434, 392,  410, 390,  412, 390,  410, 1222,  460, 1196,  408, 1224,  406, 396,  410, 394,  458, 368,  410, 392,  434, 368,  408, 394,  434, 392,  410,392,  410, 392,  412, 390,  436, 392,  410, 392,  412, 390,  410, 392,  462, 366,  410, 390,  410, 392,  436, 366,  436, 392,  412, 390,  410, 392,  436, 366,  436, 392,  436, 366,  410, 392,  410, 392,  436, 392,  412, 390,  410, 392,  410, 392,  462, 366,  408, 394,  412, 390,  410, 392,  436, 1220,410, 1220,  440, 362,  410, 1222,  436, 1220,  436, 1194,  410, 1220,  410, 392,  436},
    // on, cool, 25, EC8E235A, 114 bits
    {3376, 1660,  430, 1226,  406, 1224,  408, 394,  436, 366,  402, 404,  372, 1258,  426, 376,  422, 380,  432, 1250,  382, 1222, 396, 406,  372, 1258,  432, 396,  406, 396,  406, 1222,  408, 1270,  424, 354,  410, 1264,  446, 1180,  450, 314,  438, 390,  436, 1196,  452, 346,  438, 366,  460, 1196,  436, 414,  362, 406,  396, 390,  440, 368,  426, 376,  426, 398,  458, 320,  446, 380,  432, 414,  376, 382,  406, 396,  434, 392, 412, 404,  424, 400,  412, 354,  470, 412,  354, 394,  410, 1220,  410, 392,  436, 390,  438, 1194,  412, 390,  410, 402,  452, 1194,  412, 1220,  410, 392,  436, 366,  460, 366,  438, 364,  410, 392,  410, 392,  436, 390,  410, 1220,  434, 1198,  408, 394,  434, 392,  408, 392,  414, 388,  412, 390,462, 366,  412, 390,  412, 390,  412, 1218,  438, 1220,  412, 1220,  410, 392,  436, 366,  434, 392,  436, 366,  410, 392,  410, 392,  462, 366,  410, 392,  410, 390,  412, 390,  436, 392,  410, 390,  412, 390,  412, 390,  438, 390,  434, 368,  410, 392,  436, 366,  436, 392,  410, 392,  412, 390,  412, 390,  436, 392,  436, 366,  410, 392,  408, 392,  438, 390,  410, 392,  438, 364,  438, 364,  436, 392,  436, 364,  412, 390,  410, 392,  436, 390,  410, 1222,  436, 366,  410, 1220,  438, 1220,  436, 1194,  408, 1222,  412, 390,  410},
    // on, cool, 26, 9C4C07E1, 114 bits
    {3402, 1634,  426, 1230,  432, 1198,  436, 368,  434, 368,  378, 430,  420, 1210,  422, 380,  422, 380,  458, 1198,  432, 1198, 374, 428,  374, 1258,  458, 366,  434, 368,  434, 1196,  436, 1196,  460, 364,  438, 1192,  436, 1196,  436, 366,  462, 364,  438, 1192,  438, 364,  438, 366,  436, 1220,  438, 364,  436, 364,  436, 366,  428, 378,  426, 376,  426, 376,  426, 376,  460, 368,  436, 366,  436, 366,  434, 368,  460, 364, 434, 366,  438, 364,  436, 366,  464, 362,  436, 366,  436, 1194,  438, 364,  438, 390,  438, 1192,  412, 390,  438, 364,  462, 1192,  440, 1192,  436, 366,  410, 392,  462, 364,  440, 364,  438, 364,  438, 364,  462, 1194,  436, 366,  438, 1194,  438, 364,  462, 362,  438, 364,  436, 366,  438, 364,462, 366,  436, 364,  436, 366,  436, 1194,  464, 1192,  440, 1192,  410, 392,  436, 366,  462, 364,  410, 392,  438, 364,  436, 364,  464, 364,  438, 364,  438, 364,  410, 392,  464, 362,  438, 364,  436, 368,  436, 364,  464, 364,  438, 364,  436, 366,  410, 390,  438, 390,  438, 364,  438, 364,  438, 364,  436, 392,  436, 366,  438, 364,  428, 374,  438, 390,  412, 390,  436, 366,  436, 366,  462, 366,  412, 390,  412, 390,  438, 364,  462, 1196,  410, 392,  434, 366,  438, 1192,  436, 1220,  438, 1194,  412, 1218,  410, 392,  436},
    // on, cool, 27, 5D16DBC1, 114 bits
    {3330, 1668,  404, 1252,  352, 1278,  372, 432,  352, 450,  404, 422,  382, 1250,  380, 422,  380, 422,  406, 1248,  382, 1250, 382, 420,  382, 1248,  406, 420,  382, 420,  382, 1248,  382, 1250,  406, 422,  380, 1250,  382, 1248,  382, 420,  400, 426,  376, 1256,  378, 424,  378, 424,  376, 1258,  374, 428,  374, 428,  372, 430,  404, 424,  378, 424,  378, 424,  378, 422,  406, 420,  382, 420,  382, 420,  382, 420,  406, 420, 382, 420,  382, 420,  380, 422,  406, 422,  380, 422,  380, 1252,  380, 420,  406, 422,  380, 1250,  382, 420,  382, 420,  406, 1250,  380, 1250,  382, 420,  382, 420,  408, 420,  382, 422,  380, 420,  382, 420,  406, 422,  380, 422,  380, 1250,  382, 420,  406, 422,  380, 422,  380, 422,  380, 422,  404, 424,  376, 424,  378, 424,  378, 1252,  404, 1252,  380, 1250,  380, 422,  380, 420,  406, 422,  380, 422,  380, 422,  380, 422,  404, 424,  378, 424,  378, 424,  378, 424,  402, 426,  376, 426,  376, 426,  376, 426,  378, 428,  394, 408,  394, 408,  394, 408,  424, 424,  378, 424,  378, 424,  378,424,  402, 426,  376, 424,  376, 426,  376, 426,  398, 410,  394, 406,  394, 408,  394, 408,  398, 430,  374, 428,  374, 428,  374, 428,  422, 406,  396, 406,  394, 408,  394, 1256,  376, 1260,  392, 1240,  394, 1256,  376, 426,  378},
    // on, cool, 28, E0DE98B, 114 bits
    {3282, 1676,  422, 1226,  452, 1200,  408, 408,  408, 408,  410, 410,  410, 1206,  410, 414,  376, 426,  402, 1254,  380, 1252, 408, 392,  408, 1222,  434, 394,  382, 420,  356, 1300,  378, 1252,  376, 430,  372, 1258,  394, 1236,  430, 392,  434, 394,  382, 1254,  368, 454,  378, 424,  402, 1254,  406, 396,  406, 396,  378, 424,  376, 430,  372, 430,  392, 410,  372, 430,  432, 394,  406, 396,  408, 394,  408, 394,  436, 390, 382, 420,  410, 392,  410, 398,  430, 392,  408, 394,  408, 1222,  410, 392,  436, 390,  410, 1220,  410, 392,  410, 392,  406, 1250,  410, 1220,  354, 452,  398, 404,  458, 392,  410, 392,  410, 392,  434, 370,  432, 1224,  408, 1222,  412, 392,  374, 432,  420, 408,  394, 406,  396, 406,  396, 406,398, 430,  372, 430,  372, 428,  372, 1258,  418, 1240,  394, 1236,  432, 392,  410, 392,  438, 394,  406, 396,  408, 394,  402, 400,  432, 394,  408, 394,  408, 394,  406, 394,  434, 396,  370, 452,  378, 426,  410, 394,  432, 394,  354, 450,  398, 406,  430, 392,  408, 422,  408, 394,  406, 396,  380, 422,  434, 398,  370, 452,  376, 426,  410, 394,  432, 394,  356, 450,  398, 406,  430, 392,  434, 394,  412, 392,  408, 394,  410, 394,  434, 1226,  372, 1260,  372, 1258,  394, 408,  420, 1258,  376, 1254,  406, 1224,  410, 392,  408},
    // on, cool, 29, C6A71EFB, 114 bits
    {3388, 1608,  464, 1190,  438, 1192,  440, 362,  442, 360,  466, 362,  440, 1190,  442, 360,  440, 362,  458, 1198,  436, 1194, 438, 364,  436, 1194,  430, 378,  424, 376,  424, 1228,  432, 1200,  378, 428,  372, 1258,  422, 1210,  426, 376,  464, 364,  434, 1196,  374, 430,  372, 430,  460, 1194,  438, 364,  438, 364,  438, 364,  464, 362,  440, 362,  438, 364,  438, 362,  460, 368,  436, 366,  434, 368,  432, 368,  376, 430, 372, 430,  420, 384,  416, 386,  398, 428,  418, 384,  418, 1212,  422, 380,  398, 430,  372, 1258,  422, 380,  374, 428,  398, 1258,  422, 1208,  424, 378,  424, 378,  448, 380,  420, 382,  420, 380,  418, 382,  400, 428,  374, 1258,  422, 380,  422, 380,  398, 430,  372, 428,  372, 430,  372, 430,  398, 430,  372, 428,  372, 430,  372, 1258,  398, 1258,  372, 1258,  418, 384,  394, 408,  398, 430,  372, 428,  372, 430,  374, 428,  398, 430,  372, 430,  372, 430,  372, 430,  460, 366,  434, 368,  434, 368,  434, 368,  462, 364,  436, 366,  436, 366,  434, 368,  400, 430,  372, 428,  374, 428,  374,428,  460, 368,  434, 368,  436, 366,  434, 368,  460, 366,  436, 366,  434, 368,  434, 366,  460, 368,  436, 364,  438, 364,  436, 366,  462, 366,  436, 1196,  434, 1198,  372, 428,  460, 1196,  436, 1196,  432, 1198,  372, 430,  372},
    // on, cool, 30, 73F02B49, 114 bits
    {3376, 1644,  460, 1198,  408, 1224,  408, 392,  438, 364,  404, 402,  400, 1230,  426, 376,  426, 374,  434, 1222,  378, 1254, 372, 430,  396, 1234,  430, 394,  410, 392,  406, 1226,  406, 1226,  460, 362,  412, 1220,  438, 1196,  434, 366,  434, 392,  410, 1220,  436, 366,  412, 390,  438, 1220,  436, 366,  436, 366,  410, 390,  432, 374,  428, 374,  430, 372,  454, 348,  430, 398,  376, 426,  352, 452,  376, 426,  434, 390, 410, 392,  408, 392,  412, 390,  436, 392,  412, 390,  410, 1222,  412, 390,  438, 390,  410, 1222,  412, 390,  410, 390,  460, 1196,  434, 1198,  406, 396,  434, 368,  434, 392,  436, 366,  410, 392,  410, 392,  436, 1220,  412, 390,  412, 390,  410, 392,  462, 364,  412, 390,  410, 392,  412, 390,  436, 392,  412, 390,  412, 390,  412, 1220,  436, 1220,  410, 1222,  412, 392,  410, 392,  434, 392,  436, 366,  436, 366,  410, 392,  436, 392,  436, 366,  412, 390,  412, 390,  436, 390,  412, 390,  438, 364,  408, 394,  438, 390,  410, 392,  412, 390,  412, 390,  438, 390,  412, 392,  436, 366,  436,366,  438, 390,  412, 392,  436, 364,  412, 390,  438, 390,  438, 364,  410, 392,  410, 392,  436, 392,  436, 366,  412, 390,  412, 392,  462, 1194,  412, 390,  436, 1194,  410, 392,  464, 1194,  434, 1196,  412, 1218,  438, 364,  410}
};

uint16_t ac_temp_heat[2][227] = {
    // temp = 24
};

// ac, off, 24, 75BF854E, 114 bits
uint16_t ac_off_24[227] = {3326, 1672,  446, 1210,  374, 1256,  450, 352,  422, 380,  462, 366,  436, 1194,  454, 348,  464, 358,  468, 1190,  440, 1192, 438, 364,  438, 1194,  462, 362,  440, 362,  440, 1190,  380, 1278,  420, 408,  436, 1194,  438, 1192,  440, 362,  452, 354,  422, 1208,  460, 364,  440, 362,  466, 1194,  418, 406,  432, 370,  438, 364,  462, 364,  440, 364,  418, 406,  434, 368,  462, 366,  440, 364,  440, 366,  372, 450,  376, 452, 434, 368,  440, 362,  440, 366,  442, 406,  436, 366,  438, 364,  438, 368,  398, 428,  426, 1226,  440, 364,  438, 368,  396, 1280,  434, 1196,  438,364,  438, 364,  458, 370,  440, 364,  440, 366,  372, 450,  456, 1200,  438, 1192,  442, 1190,  440, 362,  466, 362,  442, 360,  440, 362,  440, 362,466, 364,  438, 366,  372, 452,  434, 1196,  458, 1198,  436, 1194,  438, 364,  440, 362,  462, 366,  436, 366,  436, 364,  438, 364,  462, 366,  438, 364,  442, 364,  416, 408,  458, 348,  462, 362,  442, 360,  376, 430,  476, 352,  424, 376,  424, 378,  426, 376,  484, 366,  440, 364,  440, 366,  374, 450,  456, 352,  460, 364,  440, 362,  378, 428,  476, 352,  450, 352,  424, 378,  424, 376,  446, 382,  420, 382,  420, 382,  422, 380,  476, 1180,  462, 1192,  440, 1192,  438, 364,  462, 1192,  376, 1280,  434, 1198,  436, 366,  438}; 

typedef enum AC_MODE{
    COOL,
    HEAT,
    OFF
};

uint16_t* getIrCode(int temp, AC_MODE mode) {
    if(mode == COOL)
        return (uint16_t*)ac_temp_cool[temp - 16];
    else if(mode == HEAT)
        return (uint16_t*)ac_temp_heat[temp - 16];
    else
        return (uint16_t*)ac_off_24;
};


////////////////////////////////////////////  INITAL SETTINGS  /////////////////////////////////////////////

// DEFAULT VALUES
//char* ssid = "B.net_97432";
//char* password = "36nrdhjum2yv";
char* ssid = "octofus";
char* password = "990999997995";
//const char* ssid = "CloudSenseMobile";
//const char* password = "CloudPA55-201707";
//const char* ssid = "kruno";
//const char* password = "arduino1";
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
    String dateTimeString;
    bool webSocketConnected;
    bool changingTemp;
    unsigned long lastChangedTemp;
    bool changingMode;
    unsigned long lastChangedMode;
    String lastEventName;
    unsigned long lastEventMillis;
    int animationIndex;
    bool sendToServer;
    bool sentToServer;
    unsigned long lastSentToServer;
    unsigned long lastServerEvent;
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



String generateJsonStatusObject(String deviceSn, String dateTime, String targetTemp, String targetLowTemp, String targetHighTemp, String temp, String hum, String mode) {
    //StaticJsonBuffer<250> jsonBuffer;
    DynamicJsonDocument root(250);

    //JsonObject& root = jsonBuffer.createObject();
    root["sn"] = deviceSn;
    root["time"] = dateTime;
    root["temp"] = temp;
    root["hum"] = hum;
    root["target_temp"] = targetTemp;
    root["target_temp_low"] = targetLowTemp;
    root["target_temp_high"] = targetHighTemp;
    root["mode"] = mode;
    root["running"] = String(status.running);
    root["event"] = status.lastEventName;
    char buffer[300];

    serializeJson(root, buffer);
    //root.printTo((char*)buffer, root.measureLength() + 1);

    return String(buffer);
}


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

    DynamicJsonDocument root(size);

    DeserializationError parseStatus = deserializeJson(root, buffer.get());

    //StaticJsonBuffer<1000> jsonBuffer;
    //JsonObject& root = jsonBuffer.parseObject(buffer.get());

    if (parseStatus != DeserializationError::Ok) {
        Serial.println("LoadSettings.. deserializeJson() failed");

        settings.on = "false";
        settings.mode = "heat";
        settings.targetTemp = 22;

        return false;
    } else 
    {
        settings.targetTemp = root["targetTemp"];
        settings.targetTempLow = root["targetTempLow"];
        settings.targetTempHigh = root["targetTempHigh"];

        settings.mode = (const char*) root["mode"];
        if(settings.mode == NULL)
            settings.mode = "manual";
        //settings.type = root["type"].asString();
        //if(settings.type == NULL)
        //    settings.type = "heat";
        settings.on = (const char*) root["on"];
        settings.apiUrl = (const char*) root["apiUrl"];
        settings.ssid = (const char*) root["ssid"];
        settings.password = (const char*) root["password"];

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
    //StaticJsonBuffer<1000> jsonBuffer;
    DynamicJsonDocument root(1024);

    //JsonObject& root = jsonBuffer.createObject();
    root["apiUrl"]      = settings.apiUrl;
    root["mode"]        = settings.mode;
    root["password"]    = settings.password;
    root["ssid"]        = settings.ssid;
    root["on"]          = settings.on;
    root["targetTemp"]  = settings.targetTemp;
    root["targetTempLow"]  = settings.targetTempLow;
    root["targetTempHigh"]  = settings.targetTempHigh;

    //root.printTo(newSettingsFile);
    serializeJson(root, newSettingsFile);
    newSettingsFile.flush();
    newSettingsFile.close();
    //Serial.println("Changes saved to memory.");
    return SPIFFS.exists("/settings.json");
}

void sendToWsServer() {
    String tempData = generateJsonStatusObject(deviceSn, "2018-3-31", String(settings.targetTemp), String(settings.targetTempLow), String(settings.targetTempHigh), String(status.temperature), String(status.humidity), String(settings.mode));
    webSocket.sendTXT(tempData);
    status.sentToServer = true;
    status.lastSentToServer = millis();
    Serial.println("Sending data to WS server...");
}

void turnBoilerOnOff() {
    if(settings.on == "false")
        status.running = false;

    if(status.eventHeatingOnOff) {
        if(status.running && settings.mode == "heat") {
            digitalWrite(HEATING_RELAY, LOW); // torn ON heating
            sendToWsServer();
        }
        else {
            digitalWrite(HEATING_RELAY, HIGH); // torn OFF heating
            sendToWsServer();
        }

        status.eventHeatingOnOff = false;
        Serial.println("eventHeatingOnOff...");
    }
    
    if(status.eventCoolingOnOff) {
        if(status.running && settings.mode == "cool") {
            Serial.println("Set AC to " + String(settings.targetTemp));   
            irsend.sendRaw(getIrCode(settings.targetTemp, COOL), 227, 38);// torn ON heating
            //irsend.sendNEC(0x9D713C88, 114);
            sendToWsServer();
        }
        else {
            Serial.println("Turn Off AC"); 
            irsend.sendRaw(ac_off_24, 227, 38);// torn OFF heating
            sendToWsServer();
        }
        
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

String getCurrentTime() {
    DynamicJsonDocument jsonObj(1024);
    //StaticJsonBuffer<300> jsonBuffer;
    //jsonBuffer.clear();
    //JsonObject& jsonObj = jsonBuffer.parseObject(status.dateTimeString);
    deserializeJson(jsonObj, status.dateTimeString);
    //jsonObj["hour"] = uint32.parseObject(jsonObj["hour"]) + 2;
    Serial.println( (const char*) jsonObj["hour"]);
    return (const char*) jsonObj["hour"];
}

void processPayload(uint8_t *payload) {
    //StaticJsonBuffer<300> jsonBuffer;
    DynamicJsonDocument jsonObj(300);

    //jsonBuffer.clear();
    //JsonObject& jsonObj = jsonBuffer.parseObject(payload);
    /*
    if (!jsonObj.success()) {
        Serial.println("parseObject() failed");
        return;
    } else*/ 
    {
        status.dateTimeString = (const char*) jsonObj["dateTimeJson"];
        //Serial.println("time: " + status.dateTimeString);
        
        //status.time
        String temp = (const char*) jsonObj["temp"];
        String mode = (const char*) jsonObj["mode"];
        //String type = jsonObj["type"].asString();
        String status_on = (const char*) jsonObj["on"];

        Serial.println("temp: " + temp);
        Serial.println("mode: " + mode);
        //Serial.println("type: " + type);
        Serial.println("status_on: " + status_on);

        if(temp != NULL && temp != String(settings.targetTemp)) {
            //settings.targetTemp = jsonObj["temperature"];
            changeTemp(jsonObj["temp"]);
            Serial.println("temp changed to: " + temp);
            status.dataChanged = true;
            status.changingTemp = true;
            status.settingsChanged = true;
        }
        if(mode != NULL && mode != settings.mode || settings.on == "false") {
            changeMode(mode);
            Serial.println("mode changed to: " + mode);
            status.dataChanged = true;
            status.changingMode = true;
            status.settingsChanged = true;
        }
        /*if(type != NULL) {
            settings.type = type;    // heat, cool. (In the future add ventilate, moisturize, dryout)
            Serial.println("type changed to: " + type);
        }*/
        if(status_on != NULL) {
            Serial.println("on changed to: " + status_on);
            settings.on = status_on;
            status.dataChanged = true;
            turnHeatingOnOff(status_on);
        }
        if(status.running && settings.mode == "cool")
            irsend.sendRaw(getIrCode(settings.targetTemp, COOL), 227, 38);
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

unsigned long lastEvent = 0;
bool processData = true;
unsigned long periodBeginMillis;

float temp = 0;
float hum = 0;

void loopThermostat() {
    processData = false;
    status.tempChanged = false;
    status.humChanged = false;
    //status.dataChanged = false;
    //status.sendToServer = false;

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


    // read values from sensor
    //////////////////////////////////////////////////////////////////
    float t = dht.readTemperature();
    if(!isnan(t))
        temp = t;
    float h = dht.readHumidity();
    if(!isnan(h))
        hum = h;

    if (isnan(h)|| isnan(t)) {
        //Serial.println("Invalid value from sensor");
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
    //////////////////////////////////////////////////////////////////


    // log events timestamps
    if(lastEvent > millis()) {
        status.lastServerEvent = 0;
        lastEvent = 0;
    }
    if(status.lastServerEvent + minimumIntervalSendToServer < millis())
    {
        status.lastServerEvent = millis();
        status.sendToServer = true;
    }
    if(lastEvent + intervalDurationMs < millis())
    { 
        lastEvent = millis();
        processData = true;
    }

    
    // update status and events for later processing
    if(settings.on == "true" && status.dataChanged || status.settingsChanged) {
        if(settings.mode == "heat" && (status.tempChanged || status.settingsChanged)) {
            Serial.println("Heating - " + String(status.temperature) + "C - " + String(status.humidity) + "%");
            if(status.running) {
                if(temp < (float)settings.targetTemp - 0.5) {
                    if(!status.running) {
                        Serial.println("Turn On Heating");
                        status.eventHeatingOnOff = true;
                        status.running = true;
                        //status.changingMode = true;
                        //status.lastChangedMode = millis();
                    }
                } else {
                    if(status.running) {
                        Serial.println("Turn Off Heating");
                        status.eventHeatingOnOff = true;
                        status.running = false;
                        //status.changingMode = true;
                        //status.lastChangedMode = millis();
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
        else if (settings.mode == "cool" && (status.tempChanged || status.settingsChanged)) {
            Serial.println("AC - " + String(status.temperature) + "C - " + String(status.humidity) + "%");
            if(temp > (float)settings.targetTemp + 1) {
                if(!status.running) {
                    Serial.println("Turn On AC");
                    status.eventCoolingOnOff = true;
                    status.running = true;
                    //status.changingMode = true;
                    //status.lastChangedMode = millis();
                }
            } else if (temp < (float)settings.targetTemp) {
                if(status.running) {
                    Serial.println("Turn Off AC");
                    status.eventCoolingOnOff = true;
                    status.running = false;
                    //status.changingMode = true;
                    //status.lastChangedMode = millis();
                }
            }
        }
    }

    if(processData) 
    {
        display.clear();
        displayToScreen2(temp, hum);
    }
    

    
    if(settings.on == "true" && processData || status.dataChanged || status.sendToServer) {
        lastEvent = millis();
    
        /*
        int32_t db = getRSSI(settings.ssid);
        uint8_t bars = signalStrength(db);
        drawSignalStrenth(92,48, bars, WiFi.status() == WL_CONNECTED);
        display.display();
        */

        if(status.webSocketConnected && status.sendToServer) {
            String tempData = generateJsonStatusObject(deviceSn, "2018-3-31", String(settings.targetTemp), String(settings.targetTempLow), String(settings.targetTempHigh), String(temp), String(hum), String(settings.mode));
            webSocket.sendTXT(tempData);
            status.sentToServer = true;
            status.lastSentToServer = millis();
            Serial.println("Sending data to WS server...");
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
    if(status.settingsChanged || status.dataChanged || processData || status.sendToServer) {
        status.dataChanged = false;
        status.settingsChanged = false;
        processData = false;
        if(status.sendToServer)
            status.sendToServer = false;

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

        /*if(loadSettings(settings)) {
            //digitalWrite(green, HIGH);
            Serial.println("Ok");
        } else {
            //digitalWrite(red, HIGH);
            Serial.println("Error");
        }*/
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
        //dumpRaw(&irresults);            // Output the results in RAW format
        //dumpCode(&irresults);           // Output the results as source code
        Serial.println("");
        irrec.resume();  // Receive the next value

        
        //irsend.sendRaw(vol_down, 99, 38);
        //irsend.sendPanasonic(0x1000405, 48);
        //irsend.sendPanasonic(0x4004, 0x400401008485);
    }
    //delay(100);
    
    
}