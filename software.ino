#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <dht11.h>
#include <TimeLib.h>
#include <EasyNTPClient.h>
#include <WiFiUdp.h>
#include "temp.c"
#include "him.c"
#include "weather.c"

const char* ssid     = "XXXXXXXXXX";
const char* password = "XXXXXXXXXX";

int UTC_FARKI=3;


WiFiUDP udp;
EasyNTPClient ntpClient(udp, "pool.ntp.org", (UTC_FARKI*3600));
                                                    

#define dhtPin D6
#define DHTTYPE DHT11

extern const unsigned char derece [];
extern const unsigned char nem [];
extern const unsigned char weather [];

Adafruit_SSD1306 ekran(-1);
dht11 dht;
void setup() {
  ekran.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ekran.clearDisplay();
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);

}

void loop() {
  const char* host = "api.openweathermap.org";
  const int httpPort = 80;
 Serial.print(host); 
 Serial.println(" sitesine bağlanılıyor.");  
 WiFiClient client;

 if (!client.connect(host, httpPort)) 
   {
    Serial.println("Bağlantı sağlanamadı");
    return; 
   }

 String url = "/data/2.5/weather?q=Ankara,TR&units=metric&appid=";
        url+="1d017e0241305671f60da0995f169c3b";
 
 client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

 int bekleme_suresi=0;
 while (!client.available()) 
      {
       delay(1);
       bekleme_suresi++;
       if(bekleme_suresi>=5000)
          {
           Serial.println(">>> Host veri göndermedi!"); 
           client.stop();
           return;
          }     
     }
 
 String cevap=""; 
 while(client.available()){
    cevap += client.readStringUntil('\r');           
  }        
  Serial.println("Veri alındı..");  
  
  int baslangic=cevap.indexOf("temp");
  int bitis=cevap.indexOf(",",baslangic);
                         
             
  String sicaklik="";  
  for(int i=(baslangic+6);i<bitis;i++)
    {
      sicaklik+=cevap[i];  
    }         
  time_t tarih; 
  tarih=ntpClient.getUnixTime(); 
                                             
  int ch = dht.read(dhtPin);
  delay(1000);
  saat();
  ekran.setTextSize(3);
  ekran.setTextColor(WHITE);
  ekran.setCursor(20,26);
  ekran.print(dht.temperature,1);
  ekran.drawBitmap(0,5,derece,128,64,1);
  ekran.display();
  delay(3000);
  ekran.clearDisplay();

  saat();
  ekran.setTextSize(3);
  ekran.setTextColor(WHITE);
  ekran.setCursor(20,26);
  ekran.print(dht.humidity,1);
  ekran.drawBitmap(0,0,nem,128,62,1);
  ekran.display();
  delay(3000);
  ekran.clearDisplay();

  saat();
  ekran.setTextSize(3);
  ekran.setTextColor(WHITE);
  ekran.setCursor(0,26);
  ekran.print(sicaklik);
  ekran.drawBitmap(0,0,weather,128,62,1);  
  ekran.display();
  delay(3000);
  ekran.clearDisplay();  
}

void saat()
{
  time_t tarih; 
  tarih=ntpClient.getUnixTime(); 
                          
  ekran.setTextSize(1);
  ekran.setCursor(0,0);
  ekran.print(day(tarih));
  ekran.write('/');
  ekran.print(month(tarih));
  ekran.write('/');
  ekran.print(year(tarih));
  ekran.write('-');
  if(hour(tarih)<10)ekran.write('0');
  ekran.print(hour(tarih));         
  ekran.write(':');
  if(minute(tarih)<10)ekran.write('0');
  ekran.print(minute(tarih));
}
