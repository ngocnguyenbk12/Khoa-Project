#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//Include for DHT11

#include "DHT.h"

#define DHTPIN 12 // what digital pin we're connected to

#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

typedef struct {
  String Hum;
  String Temp;
  String rTemp;
  String rHum;

}DataCom  ;
String FinalSend;




void setup() {
lcd.init();
lcd.backlight();
lcd.print("Nhiet do:");
lcd.setCursor(0,1); // ( vi trí, hàng )
lcd.print("Do am");
 Serial.begin(9600);


}
void loop() {
    int h = dht.readHumidity();
  int t = dht.readTemperature();
  lcd.setCursor(11,0);
  lcd.print(h);
  lcd.setCursor(11,1);
  lcd.print(t);
  
  DataCom  DataSend;
 
  DataSend.Hum=char( h/10+48);
  DataSend.rHum=char(h%10+48);
  DataSend.Temp=char(t/10+48);
  DataSend.rTemp=char(t%10+48);

 

  FinalSend = DataSend.Hum;
  FinalSend += DataSend.rHum;
  FinalSend += DataSend.Temp;
  FinalSend += DataSend.rTemp;

Serial.println(h);
Serial.println(t);
//  Serial.println(FinalSend);
  delay(500);

  
  
}
