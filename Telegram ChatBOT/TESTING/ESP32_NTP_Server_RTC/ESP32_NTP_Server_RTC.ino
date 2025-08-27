
#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"

const char *ssid = "Neatic";
const char *password = "1234567890";

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 7 * 3600;   /* CAMBODIA UTC+7 */ 
const int daylightOffset_sec = 0;      /* NO DAYLIGHT SAVING TIME */ 

/* Cambodia TimeZone (ICT, UTC+7, no DST) */ 
const char *time_zone = "ICT-7";

/* PRINT LOCAL TIME */ 
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

/* CALLBACK WHEN NTP UPDATES TIME */ 
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup() {
  Serial.begin(115200);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  esp_sntp_servermode_dhcp(1);  /* (OPTIONAL) */ 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  /* SET NTP SYNC CALLBACK */ 
  sntp_set_time_sync_notification_cb(timeavailable);

  /* Option 1: Using fixed GMT offset (UTC+7) */ 
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /* Option 2: Using time zone string (better for clarity) */ 
  configTzTime(time_zone, ntpServer1, ntpServer2);
}

void loop() {
  delay(1000);
  printLocalTime();
}
