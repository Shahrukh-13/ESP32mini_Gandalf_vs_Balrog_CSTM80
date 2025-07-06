#include <WiFi.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include "Gandalf_vs_Balrog_ESP32_5.h"    

#define NORMAL_SPEED  // Comment out for rame rate for render speed test
#define GIF_IMAGE Gandalf_vs_Balrog_ESP32_5

AnimatedGIF gif;
TFT_eSPI tft = TFT_eSPI();

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;
String Date;
String Time;

bool wifi_flag;

const char* ssid_def = "*********"; //Enter SSID here
const char* password_def = "********"; //Enter Password here

static  char                chBuffer[81];

char        chDayOfMonth[3];                                    // Day of month (0 through 31).
char        chDayofWeek[4];                                     // Day of week (Sunday through Saturday).
char        chHour[3];                                          // Hour.
char        chMinute[3];                                        // Minute.
char        chMonth[4];                                         // Month.
const char* chNtpServer = "pool.ntp.org";                       // NTP time server.
char        chSecond[3];                                        // Second.
char        chYear[5];                                          // Year.
char        AM_PM[3];                                           // AM/PM.

uint8_t wifi_connection_timeout_count;

void setup() 
{
  Serial.begin(115200);

  wifi_connection_timeout_count = 0;
  tft.begin();
  tft.setRotation(1);
  tft.invertDisplay(1); // depending on the variant of LCD being used
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  gif.begin(BIG_ENDIAN_PIXELS);
  gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw);
  tft.startWrite(); 
  tft.setTextDatum(TC_DATUM);
    
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid_def);
  WiFi.begin(ssid_def, password_def);
  while (WiFi.status() != WL_CONNECTED && wifi_connection_timeout_count <10) 
  {
      delay(500);
      Serial.print(".");
      tft.drawString("Connecting", tft.width() / 2 + 75, 194, 2);
      wifi_connection_timeout_count++;
  }
  
  if(wifi_connection_timeout_count >=10)
  {
    ESP.restart();
  }
  Serial.println(" CONNECTED");
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Connected", tft.width() / 2 + 75, 194, 2);
      
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  GetLocalTime();
  
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  //tft.drawString(Date_Time, 0, 0, 4);;
  tft.drawString(Date, tft.width() / 2 + 75, 194, 2);
  tft.drawString(Time, tft.width() / 2 + 75, 214, 2);
  gif.playFrame(false, NULL);
  GetLocalTime(); 
} 

void GetLocalTime()
{
  time_t rawtime;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain date and time");
    Date = "Failed to obtain date";
    Time = "Failed to obtain time";
    ESP.restart();
    //return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  //Serial.println(timeinfo.tm_mday);
  /*tm_sec: seconds after the minute;
  tm_min: minutes after the hour;
  tm_hour: hours since midnight;
  tm_mday: day of the month;
  tm_year: years since 1900;
  tm_wday: days since Sunday;
  tm_yday: days since January 1;
  tm_isdst: Daylight Saving Time flag;*/ 

  //time (&rawtime);
  //Date_Time = asctime(localtime (&rawtime));

  strftime(chHour, sizeof(chHour), "%I", & timeinfo);
  strftime(chMinute, sizeof(chMinute), "%M", & timeinfo);
  
  // Then obtain day of week, day of month, month and year.
    
  strftime(chDayofWeek, sizeof(chDayofWeek), "%A", & timeinfo);
  strftime(chDayOfMonth, sizeof(chDayOfMonth), "%d", & timeinfo);
  strftime(chMonth, sizeof(chMonth), "%B", & timeinfo);
  strftime(chYear, sizeof(chYear), "%Y", & timeinfo);
  strftime(AM_PM, sizeof(AM_PM), "%p", & timeinfo);
  
   //sprintf(chBuffer, "%s, %s %s, %s, %s:%s %s", String(chDayofWeek), String(chMonth), String(chDayOfMonth), String(chYear), String(chHour), String(chMinute), String(AM_PM));
   
   sprintf(chBuffer, "%s, %s %s, %s", String(chDayofWeek), String(chMonth), String(chDayOfMonth), String(chYear));
   Date = String(chBuffer);
   sprintf(chBuffer, "%s:%s %s", String(chHour), String(chMinute), String(AM_PM));
   Time = String(chBuffer);
   Time = Time.substring(0,Time.indexOf(':')) + " : " + Time.substring(Time.indexOf(':')+1); // Adding a space between for ':' so it is easy to read
  //Serial.print(Date_Time);
}
