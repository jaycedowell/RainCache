#include <WiFi.h>
#include <ESP32Time.h>
#include <math.h>
#include "driver/temp_sensor.h"
#include "Adafruit_MAX1704X.h"
#include "Adafruit_VL53L1X.h"

#include "WiFi_private.h"
/* WiFi_private.h includes:
    * ssid, pass - WiFi setup
    * staticIP, subnet - static IP address and network submask
    * gateway - gateway address
    * primaryDNS, secondaryDNS - DNS servers
*/

WiFiServer server(80);

ESP32Time rtc(0);

Adafruit_MAX17048 maxlipo;

#define IRQ_PIN D5
#define XSHUT_PIN D6

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

byte icon_raw[] = {0, 0, 1, 0, 1, 0, 32, 32, 0, 0, 1, 0, 32, 0, 168, 16, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 32, 0, 0, 0, 64, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 128, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 3, 255, 4, 1, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 248, 3, 0, 2, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 77, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 3, 0, 0, 76, 2, 0, 2, 119, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 4, 0, 2, 127, 2, 0, 2, 119, 3, 0, 0, 76, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 90, 3, 0, 2, 232, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 232, 2, 0, 2, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 114, 3, 1, 2, 254, 4, 1, 3, 255, 3, 0, 2, 205, 3, 0, 1, 138, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 2, 0, 2, 127, 3, 0, 1, 138, 3, 0, 2, 205, 4, 1, 3, 255, 3, 1, 2, 254, 2, 0, 2, 114, 0, 0, 0, 0, 0, 0, 0, 48, 3, 0, 2, 251, 3, 0, 2, 252, 2, 0, 2, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 102, 3, 0, 2, 252, 3, 0, 2, 251, 0, 0, 0, 48, 3, 0, 1, 164, 4, 1, 3, 255, 3, 0, 1, 141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 141, 4, 1, 3, 255, 3, 0, 1, 164, 3, 0, 2, 230, 4, 1, 3, 255, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 4, 1, 3, 255, 3, 0, 2, 230, 3, 0, 2, 251, 4, 1, 3, 255, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 1, 3, 255, 3, 1, 2, 252, 3, 0, 2, 229, 4, 1, 3, 255, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 4, 1, 3, 255, 3, 0, 2, 231, 3, 0, 1, 163, 4, 1, 3, 255, 3, 0, 1, 143, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 141, 4, 1, 3, 255, 3, 0, 1, 165, 0, 0, 0, 47, 3, 0, 2, 251, 3, 0, 2, 253, 2, 0, 2, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 103, 3, 0, 2, 252, 3, 0, 2, 251, 0, 0, 0, 49, 0, 0, 0, 0, 2, 0, 2, 112, 3, 1, 2, 254, 4, 1, 3, 255, 3, 0, 2, 207, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 3, 0, 1, 139, 3, 0, 2, 205, 4, 1, 3, 255, 3, 1, 3, 254, 2, 0, 2, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 88, 3, 1, 2, 254, 4, 1, 3, 255, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 68, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 232, 2, 0, 2, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 237, 4, 1, 3, 255, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 2, 194, 4, 1, 3, 255, 3, 0, 1, 144, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 194, 4, 1, 3, 255, 3, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 3, 0, 1, 168, 4, 1, 3, 255, 3, 0, 2, 229, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 120, 4, 1, 3, 255, 2, 0, 2, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 175, 3, 0, 2, 238, 3, 0, 1, 159, 3, 0, 1, 130, 3, 0, 1, 159, 3, 0, 2, 236, 4, 1, 3, 255, 3, 0, 2, 241, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 3, 0, 2, 245, 3, 0, 2, 254, 3, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 69, 3, 0, 2, 254, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 3, 0, 2, 192, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 119, 4, 1, 3, 255, 3, 0, 2, 237, 0, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 3, 0, 2, 236, 4, 1, 3, 255, 3, 0, 1, 140, 2, 0, 2, 105, 2, 0, 2, 125, 2, 0, 2, 105, 0, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 173, 4, 1, 3, 255, 3, 0, 2, 245, 2, 0, 2, 115, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 2, 0, 2, 114, 3, 0, 2, 244, 4, 1, 3, 255, 2, 0, 2, 174, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 0, 1, 154, 4, 1, 3, 254, 4, 1, 3, 255, 3, 0, 2, 239, 2, 0, 2, 172, 3, 0, 1, 135, 3, 0, 1, 134, 2, 0, 2, 171, 3, 0, 2, 238, 4, 1, 3, 255, 4, 1, 3, 254, 3, 0, 1, 155, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 71, 3, 0, 2, 198, 3, 1, 2, 254, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 255, 4, 1, 3, 254, 3, 0, 2, 200, 3, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, 2, 0, 2, 92, 2, 0, 2, 120, 2, 0, 2, 121, 2, 0, 2, 93, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 231, 231, 255, 255, 231, 231, 255, 254, 102, 102, 127, 254, 102, 102, 127, 254, 102, 102, 127, 254, 126, 126, 127, 254, 126, 126, 127, 224, 0, 0, 7, 192, 0, 0, 3, 128, 0, 0, 1, 15, 255, 255, 240, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 31, 255, 255, 248, 15, 255, 255, 240, 131, 255, 255, 129, 195, 255, 255, 131, 227, 255, 255, 7, 227, 255, 206, 15, 227, 255, 192, 31, 225, 255, 128, 63, 240, 255, 0, 255, 240, 60, 15, 255, 248, 0, 31, 255, 254, 0, 127, 255, 255, 129, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

void setup() {
  // Set the pins for the on-board RGB LED to output
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(D8, INPUT);
  pinMode(D9, INPUT);

  // Start the battery monitor
  maxlipo.begin();
 
  // Connect with the ToF distance sensor
  Wire.begin();
  if( !vl53.begin(0x29, &Wire) ) {
    while (1)       delay(10);
  }
  
  // int16_t offset;
  // uint16_t xtalk;
  // for(int i=0; i<5; i++) {
  //   vl53.VL53L1X_CalibrateOffset(9.125*25.4, &offset);
  //   vl53.VL53L1X_SetOffset(offset);
  //   vl53.VL53L1X_CalibrateXtalk(9.125*25.4, &xtalk);
  //   vl53.VL53L1X_SetXtalk(xtalk);

  //   delay(500);
  // }

  if( !vl53.startRanging() ) {
    while (1)       delay(10);
  }
  
  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms
  vl53.setTimingBudget(500);

  delay(10);

  // Set the static IP
  
  WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS);
  
  // Connect to the WiFi
  WiFi.begin(ssid, pass);
  
  while( WiFi.status() != WL_CONNECTED ) {
    led_busy();
    delay(500);
  }
  
  // Connect to a NTP server to set the RTC
  configTime(0, 0, "0.north-america.pool.ntp.org");
  struct tm timeinfo;
  if( getLocalTime(&timeinfo) ){
    rtc.setTimeStruct(timeinfo); 
  }
  
  // Start the HTTP server
  led_off();
  server.begin();
}

void loop() {
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
              
              ESP.restart();
              
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
              if( digitalRead(D9) == 0 ) {
                client.print("Yes");
              } else {
                client.print("No");
              }
              client.println("<br>");

              client.print("Charging? ");
              if( digitalRead(D8) == 0 ) {
                client.print("Yes");
              } else {
                client.print("No");
              }
              client.println("<br>");

              float cellVoltage = maxlipo.cellVoltage();
              if( !isnan(cellVoltage) ) {
                float cellCharge = maxlipo.cellPercent();
                float cellRate = maxlipo.chargeRate();
                client.print("Battery voltage: ");
                client.print(cellVoltage);
                client.println(" V<br>");
                client.print("Battery charge: ");
                client.print(cellCharge);
                client.println(" %<br>");
                client.print("Battery charge rate: ");
                client.print(cellRate);
                client.println(" %/hr<br>");
              } else {
                client.println("Battery voltage: unknown<br>");
                client.println("Battery charge: unknown<br>");
                client.println("Battery charge rate: unknown<br>");
              }
              
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
              
              float dist = get_distance();
              float dist_err = 3/1000.0 + dist*0.01;
              client.print("Distance to water is ");
              if( (dist > 0) && (dist < 4) )  {
                client.print(m_to_in(dist));
                client.print(" +/- ");
                client.print(m_to_in(dist_err));
                client.println(" in<br>");
              } else {
                client.println("unknown<br>");
              }
              
              client.print("Current volume is ");
              if( (dist > 0) && (dist < 4) )  {
                float vol = 4 * depth_to_volume_per_tank(61.25 - 4.625 - m_to_in(dist));
                float vol_hi = 4 * depth_to_volume_per_tank(61.25 - 4.625 - m_to_in(dist - dist_err));
                float vol_lo = 4 * depth_to_volume_per_tank(61.25 - 4.625 - m_to_in(dist + dist_err));
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
  temp_sensor_read_celsius(&result);
  return result;
}

float c_to_f(float temp_c) {
  // Convert a temperature in C to F
  return temp_c * 9 / 5 + 32;
}

float get_distance() {
  int16_t dist_mm;
  float dist_m = -1.0;
  
  if (vl53.dataReady()) {
    dist_mm = vl53.distance();
    if (dist_mm != -1) {
      dist_m = dist_mm / 1000.0;
    }
    
    vl53.clearInterrupt();
  }
  
  return dist_m;
}

float m_to_in(float dist_m) {
  return dist_m * 39.3700787402;
}

float depth_to_volume_per_tank(float depth_in) {
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
