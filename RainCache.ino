#include <WiFi.h>
#include <ESP32Time.h>
#include <math.h>
#include <Wire.h>
#include "driver/temperature_sensor.h"
#include <Adafruit_ADS1X15.h>
#include "TMP100.h"

#include "WiFi_private.h"
/* WiFi_private.h includes:
    * ssid, pass - WiFi setup
    * staticIP, subnet - static IP address and network submask
    * gateway - gateway address
    * primaryDNS, secondaryDNS - DNS servers
*/

temperature_sensor_handle_t temp_sensor = NULL;
temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);

WiFiServer server(80);

ESP32Time rtc(0);
unsigned long lastNTPSync = 0;

Adafruit_ADS1115 ads1115;
TMP100 tmp100;

byte icon_raw[] = {0, 0, 1, 0, 1, 0, 32, 32, 0, 0, 1, 0, 32, 0, 168, 16, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 32, 0, 0, 0, 64, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 128, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 3, 0, 0, 76, 2, 0, 2, 119, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 2, 0, 2, 119, 3, 0, 0, 76, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 90, 3, 0, 2, 232, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 232, 2, 0, 2, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 114, 3, 1, 2, 254, 4, 1, 3, 255, 3, 0, 2, 205, 3, 0, 1, 138, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 3, 0, 1, 138, 3, 0, 2, 205, 4, 1, 3, 255, 3, 1, 2, 254, 2, 0, 2, 114, 0, 0, 0, 0, 0, 0, 0, 48, 3, 0, 2, 251, 3, 0, 2, 252, 2, 0, 2, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 102, 3, 0, 2, 252, 3, 0, 2, 251, 0, 0, 0, 48, 3, 0, 1, 164, 4, 1, 3, 255, 3, 0, 1, 141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 141, 4, 1, 3, 255, 3, 0, 1, 164, 3, 0, 2, 230, 4, 1, 3, 255, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 4, 1, 3, 255, 3, 0, 2, 230, 3, 0, 2, 251, 4, 1, 3, 255, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 1, 3, 255, 3, 1, 2, 252, 3, 0, 2, 229, 4, 1, 3, 255, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 4, 1, 3, 255, 3, 0, 2, 231, 3, 0, 1, 163, 4, 1, 3, 255, 3, 0, 1, 143, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 141, 4, 1, 3, 255, 3, 0, 1, 165, 0, 0, 0, 47, 3, 0, 2, 251, 3, 0, 2, 253, 2, 0, 2, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 103, 3, 0, 2, 252, 3, 0, 2, 251, 0, 0, 0, 49, 0, 0, 0, 0, 2, 0, 2, 112, 3, 1, 2, 254, 4, 1, 3, 255, 3, 0, 2, 207, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 3, 0, 1, 139, 3, 0, 2, 205, 4, 1, 3, 255, 3, 1, 3, 254, 2, 0, 2, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 88, 3, 1, 2, 254, 4, 1, 3, 255, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 68, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 232, 2, 0, 2, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 237, 4, 1, 3, 255, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 2, 194, 4, 1, 3, 255, 3, 0, 1, 144, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 194, 4, 1, 3, 255, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 3, 0, 1, 168, 4, 1, 3, 255, 3, 0, 2, 229, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 120, 4, 1, 3, 255, 2, 0, 2, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 175, 3, 0, 2, 238, 3, 0, 1, 159, 3, 0, 1, 130, 3, 0, 1, 159, 3, 0, 2, 236, 4, 1, 3, 255, 3, 0, 2, 241, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 3, 0, 2, 245, 3, 0, 2, 254, 3, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 69, 3, 0, 2, 254, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 192, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 119, 4, 1, 3, 255, 3, 0, 2, 237, 0, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 3, 0, 2, 236, 4, 1, 3, 255, 3, 0, 1, 140, 2, 0, 2, 105, 2, 0, 2, 125, 2, 0, 2, 105, 0, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 173, 4, 1, 3, 255, 3, 0, 2, 245, 2, 0, 2, 115, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2, 0, 2, 114, 3, 0, 2, 244, 4, 1, 3, 255, 2, 0, 2, 174, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 0, 1, 154, 4, 1, 3, 254, 4, 1, 3, 255, 3, 0, 2, 239, 2, 0, 2, 172, 3, 0, 1, 135, 3, 0, 1, 134, 2, 0, 2, 171, 3, 0, 2, 238, 4, 1, 3, 255, 4, 1, 3, 254, 3, 0, 1, 155, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 71, 3, 0, 2, 198, 3, 1, 2, 254, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 254, 3, 0, 2, 200, 3, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, 2, 0, 2, 92, 2, 0, 2, 120, 2, 0, 2, 121, 2, 0, 2, 93, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 231, 231, 255, 255, 231, 231, 255, 254, 102, 102, 127, 254, 102, 102, 127, 254, 102, 102, 127, 254, 126, 126, 127, 254, 126, 126, 127, 224, 0, 0, 7, 192, 0, 0, 3, 128, 0, 0, 1, 15, 255, 255, 240, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 15, 255, 255, 240, 131, 255, 255, 129, 195, 255, 255, 131, 227, 255, 255, 7, 227, 255, 206, 15, 227, 255, 192, 31, 225, 255, 128, 63, 240, 255, 0, 255, 240, 60, 15, 255, 248, 0, 31, 255, 254, 0, 127, 255, 255, 129, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

// Approximate LiPo state-of-charge as a function voltage in steps of 5%
float lipo_soc_volt[] = {4.2 , 4.15, 4.11, 4.08, 4.02,  // 100, 95, 90, 85, 80
                         3.98, 3.95, 3.91, 3.87, 3.85,  //  75, 70, 65, 60, 55
                         3.84, 3.82, 3.8 , 3.79, 3.77,  //  50, 45, 40, 35, 30
                         3.75, 3.73, 3.71, 3.69, 3.61,  //  25, 20, 15, 10,  5
                         3.27};                         //   0

#define LED_RED D1
#define LED_GREEN D2
#define LED_BLUE D3

#define PGOOD_PIN D9
#define CHRG_PIN D8

#define VBATT_PIN A0

void setup() {
  // Set the pins for the on-board RGB LED to output
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(PGOOD_PIN, INPUT);
  pinMode(CHRG_PIN, INPUT);

  pinMode(VBATT_PIN, INPUT);
  
  // Enable the SoC temperature sensor
  temperature_sensor_install(&temp_sensor_config, &temp_sensor);
  temperature_sensor_enable(temp_sensor);
  delay(10); 

  // Start the TMP100 sensor
  tmp100.begin();
  tmp100.setResolution(10);
  delay(10);  

  // Start the ADC
  ads1115.begin(0x49);
  ads1115.setGain(GAIN_ONE); 
  delay(10);

  // Set the static IP
  WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS);
  delay(10);

  // Set the last NTP sync time to zero
  lastNTPSync = 0;

  led_off();
}

void loop() {
  while( WiFi.status() != WL_CONNECTED ) {
    led_busy();
    
    // Connect to the WiFi
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, pass);

    // Give it 30 s to establish the connection
    unsigned long startTime = millis();
    bool timeOut = false;
    while( WiFi.status() != WL_CONNECTED ) {
      if( (millis() - startTime) > 30000 ) {
        timeOut = true;
        break;
      }
      led_busy();
      delay(500);
    }
    if( timeOut ) {
      // Nope, restart the whole process
      continue;
    }

    // Connect to a NTP server to set the RTC
    configTime(0, 0, "0.north-america.pool.ntp.org", "1.north-america.pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if( getLocalTime(&timeinfo) ) {
      rtc.setTimeStruct(timeinfo);
      lastNTPSync = millis();
    }

    // LED off and get ready to receive HTTP requests
    led_off();
    server.begin();
  }

  if( (millis() - lastNTPSync) > 86400*1000 ) {
    // Connect to a NTP server to set the RTC
    configTime(0, 0, "0.north-america.pool.ntp.org", "1.north-america.pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    if( getLocalTime(&timeinfo) ) {
      rtc.setTimeStruct(timeinfo);
      lastNTPSync = millis();
    }
  }

  WiFiClient client = server.available();

  if( client ) {
    bool currentLineIsBlank = true;
    String lastLine = "begin:";
    while( client.connected() ) {
      led_blue();

      String currentLine = "";
      if( client.available() ) {
        char c = client.read();
        
        if( c == '\n' ) {
          if (currentLine.length() == 0) {
            if( lastLine.indexOf("GET /favicon.ico") != -1 ) {
              client.println("HTTP/1.1 200 OK");
              //client.println("Content-Type: image/x-icon");
              client.println("Connection: close");
              client.println();

              client.write(&(icon_raw[0]), sizeof(icon_raw));
              
            } else if( lastLine.indexOf("GET /restart") != -1 ) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<head>");
              client.println("<meta http-equiv='refresh' content='10;url=/' />");
              client.println("<link href='favicon.ico' rel='shortcut icon' type='image/x-icon'>");
              client.println("<title>RainCache</title>");
              client.println("</head>");
              client.println("<body>");

              client.println("Restarting...<br>");

              client.println("</body>");
              client.println("</html>");

              delay(1);

              client.stop();
              
              //ESP.restart();
              uint64_t sleepTime = 5 * 1000000;
              esp_sleep_enable_timer_wakeup(sleepTime);
              esp_deep_sleep_start();
              
            } else if( lastLine.indexOf("GET /power") != -1 ) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<head>");
              client.println("<link href='favicon.ico' rel='shortcut icon' type='image/x-icon'>");
              client.println("<title>RainCache</title>");
              client.println("</head>");
              client.println("<body>");

              client.print("Input Power? ");
              if( digitalRead(PGOOD_PIN) == 0 ) {
                client.print("Yes");
              } else {
                client.print("No");
              }
              client.println("<br>");

              client.print("Charging? ");
              if( digitalRead(CHRG_PIN) == 0 ) {
                client.print("Yes");
              } else {
                client.print("No");
              }
              client.println("<br>");

              float cellVoltage = get_battery_voltage();
              float cellCharge = get_battery_charge(cellVoltage);
              client.print("Battery voltage: ");
              client.print(cellVoltage);
              client.println(" V<br>");
              client.print("Battery charge: ");
              client.print(cellCharge);
              client.println(" %<br>");

              client.println("<p></p>");
    
              client.print(rtc.getTime("%Y/%m/%d %H:%M:%S"));
              client.println("<br>");

              client.println("</body>");
              client.println("</html>");
              
            } else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<head>");
              client.println("<link href='favicon.ico' rel='shortcut icon' type='image/x-icon'>");
              client.println("<title>RainCache</title>");
              client.println("</head>");
              client.println("<body>");
    
              float temp_soc = get_soc_temp();
              client.print("SoC temperature is ");
              client.print(c_to_f(temp_soc));
              client.println(" F<br>");
  
              float temp_air = get_temp();
              client.print("Air temperature is ");
              if( temp_air > -90 ) {
                client.print(c_to_f(temp_air));
                client.println(" F<br>");
              } else {
                client.println("unknown<br>");
              }

              float ref = get_reference();
              client.print("Reference voltage is ");
              if( ref >= 0 ) {
                client.print(ref);

              } else {
                client.println("unknown<br>");
                client.println(" V<br>");
              }
  
              float depth = get_depth();
              client.print("Distance to water is ");
              if( depth >= 0 ) {
                client.print(depth);
                client.print(" +/- ");
                client.print(0.04);
                client.println(" in<br>");
              } else {
                client.println("unknown<br>");
              }
              
              client.print("Current volume is ");
              if( depth >= 0 )  {
                float vol = 4 * depth_to_volume_per_tank(depth);
                float vol_hi = 4 * depth_to_volume_per_tank(depth + 0.04);
                float vol_lo = 4 * depth_to_volume_per_tank(depth - 0.04);
                float vol_err = vol_hi - vol;
                if( (vol - vol_lo) > vol_err ) {
                  vol_err = vol - vol_lo;
                }
                client.print(vol);
                client.print(" +/- ");
                client.print(vol_err);
                client.print(" gal");
              } else {
                client.print("unknown");
              }
              client.println(" assuming all tanks have equilibrated<br>");
              
              client.println("<p></p>");
    
              client.print(rtc.getTime("%Y/%m/%d %H:%M:%S"));
              client.println("<br>");

              client.println("</body>");
              client.println("</html>");
              
            }
            
            led_off();
            
            break;
          } else {
            currentLine = "";
          }
        } else if( c != '\r' ) {
          currentLine += c;
          lastLine += c;
        }
      }
    }

    delay(1);

    client.stop();
  }
}

float get_soc_temp() {
  // Return the temperature in C of the SoC.
  float result = 0.0;
  if( temperature_sensor_get_celsius(temp_sensor, &result) != ESP_OK ) {
    result = -99.0;
  }
  return result;
}

float c_to_f(float temp_c) {
  // Convert a temperature in C to F
  return temp_c * 9 / 5 + 32;
}

float get_temp() {
  // Poll the TMP100 sensor for the current temperature in C.  Returns a
  // temperature of -99 C if the polling failed.
  
  if( !tmp100.isDeviceReady() ) {
    led_fail(2, 100);
    return -99.0;
  }

  float result = tmp100.getTemp();
  if( result < -90 ) {
    led_fail(3, 100);
    return -99.0;
  }

  return result;
}

float get_battery_voltage() {
  // Poll the onboard ADC to get the battery voltage (corrected for
  // the voltage divider).

  float mV_bat = 0.0;
  int count = 0;
  for(int i=0; i<50; i++) {
    int value_bat = analogReadMilliVolts(VBATT_PIN);
    mV_bat += value_bat;
    count++;
  }

  mV_bat /= count;
  
  return mV_bat / 1000.0 * 4 / 3;
}

float get_battery_charge(float V_bat) {
  // Given a battery voltage convert that to an approximate state of
  // charge in percent.  This assumes a single cell LiPo batter.

  // Deal with the max/min limits
  if( V_bat >= 4.2 ) {
    return 100.0;
  } else if( V_bat <= 3.27 ) {
    return 0.0;
  }

  // Interpolate between the closest values in the state of charge curve
  float result = 0.0;
  for(int i=0; i<20; i++) {
    if( (V_bat <= lipo_soc_volt[i]) && (V_bat > lipo_soc_volt[i+1]) ) {
      result = (100-i*5) + (V_bat - lipo_soc_volt[i]) * 5 / (lipo_soc_volt[i] - lipo_soc_volt[i+1]);
      break;
    }
  }

  return result;
}

float get_depth() {
  // Poll the eTape liquid level sensor and return the water depth in
  // inches.  Returns < 0 inch if the polling failed.

  float mV_sensor = 0.0;
  int count = 0;
  for(int i=0; i<50; i++) {
    int value_sensor = ads1115.readADC_Differential_2_3();
    mV_sensor -= value_sensor;  // Negative because of how the signals are connected
    count++;
    delay(1);
  }

  mV_sensor /= count;
  
  float dist_in = (60 - 1) * mV_sensor * 0.000125 / 3.3;

  return dist_in;
}

float depth_to_volume_per_tank(float depth_in) {
  // Convert a depth in inches to a volume in gallons.

  float vol_gal = 0.0;
  if( depth_in < 0 ) {
    vol_gal = 0.0;
  } else if( depth_in < 10 ) {
    vol_gal = -0.02011084 * depth_in*depth_in*depth_in \
              +0.86926657 * depth_in*depth_in \
              +17.29879176 * depth_in \
              -1.34967672;
  } else if( depth_in < 20 ) {
    vol_gal = -7.02548889e-03 * depth_in*depth_in*depth_in \
              +3.72465989e-01 * depth_in*depth_in \
              +2.33919033e+01 * depth_in \
              -2.55243404e+01;
  } else {
    vol_gal = -8.82034899e-03 * depth_in*depth_in*depth_in \
              +5.82355747e-01 * depth_in*depth_in \
              +1.68830818e+01 * depth_in \
              +3.50212544e+01;
  }

  if( vol_gal < 0 ) {
    vol_gal = 0.0;
  }
  return vol_gal;
}

float get_reference() {
  // Poll the 2.5 V voltage divider and return the voltage.  Returns
  // < 0 inch if the polling failed.

  float mV_ref = 0.0;
  int count = 0;
  for(int i=0; i<50; i++) {
    int value_ref = ads1115.readADC_Differential_0_1();
    mV_ref -= value_ref;  // Negative because of how the signals are connected
    count++;
    delay(1);
  }

  mV_ref /= count;
  
  return mV_ref;
}

int led_state = 0;
void led_busy() {
  // Move the on-board LED through a sequence of colors, updated every
  // time this function is called.

  if( led_state == 0 || led_state == 1 || led_state == 5 ) {
     digitalWrite(LED_RED, LOW);
  } else {
    digitalWrite(LED_RED, HIGH);
  }

  if( led_state == 1 || led_state == 2 || led_state == 3 ) {
     digitalWrite(LED_GREEN, LOW);
  } else {
    digitalWrite(LED_GREEN, HIGH);
  }

  if( led_state == 3 || led_state == 4 || led_state == 5 ) {
     digitalWrite(LED_BLUE, LOW);
  } else {
    digitalWrite(LED_BLUE, HIGH);
  }

  led_state += 1;
  if( led_state > 5 ) {
    led_state = 0;
  }
}

void led_off() {
  // On-board LED off

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  led_state = 0;
}

void led_red() {
  // On-board LED red

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void led_yellow() {
  // On-board LED yellow

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void led_green() {
  // On-board LED green

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void led_blue() {
  // On-board LED blue

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

void led_warn(int count, int interval_ms) {
  // Blink the red LED to inidicate an error

  for(int i=0; i<2*count; i++) {
    if( i % 2 == 0 ) {
      led_off();
    } else {
      led_yellow();
    }
    delay(interval_ms);
  }
}

void led_fail(int count, int interval_ms) {
  // Blink the red LED to inidicate an error

  for(int i=0; i<2*count; i++) {
    if( i % 2 == 0 ) {
      led_off();
    } else {
      led_red();
    }
    delay(interval_ms);
  }
}
