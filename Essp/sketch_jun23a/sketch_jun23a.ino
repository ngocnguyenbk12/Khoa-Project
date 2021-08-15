/*#include  <Adafruit_ST7735.h>
#include  <Adafruit_GFX.h>
#include  <SPI.h>
#define TFT_CS     15
#define TFT_RST    2                      
#define TFT_DC     0
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
#define TFT_SCLK 14   
#define TFT_MOSI 13  
#include "stdio.h"
*/
// Include for System Warning Control
const int Relay1 = 5;
const int Relay2 = 16;
const int Bluelight = 10;
const int Buzzer = 0;

// Include for LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//Include for DHT11
#include "DHT.h"
#define DHTPIN 2// what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Include for Variables
String Rx; 
String alert;
int i;
int t,h;

int H0 = 20;
int H1 = 85;
int T0 = 25;
int T1 = 35;




// Include for Server
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>  //Thư viện mDNS
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer webServer(80);
BlynkTimer timer_update;
char* ssid = "CoTram";
char* pass = "12345678"; 



const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
      
          .bt_1 {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#FF0F9F;
            border-radius:5px;
          }
          .bt_2 {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#00FF00;
            border-radius:5px;
          }
      .bt_3 {
            height:50px; 
            width:500px; 
            margin:10px 0;
            background-color:#00FF00;
            border-radius:5px;
          }
      .Tag{
      font-size: 25px;
      style: color:red;
      }
      .Header{
      font-size: 30px;
      style: color: black;
      background-color: pink;
      }
       </style>
       <meta name="viewport" content="width=device-width,user-scalable=0" charset="UTF-8">
   </head>
   <body onload="javascript:init()"> 
   
    <h1 class = 'Header'> DỰ ÁN GIÁM SÁT  </h1> 
    
    
    <div    <p class="Tag" > Thông số  </div>
  <div  <p class="Tag" > Nhiệt độ - Độ Ẩm - Gas </div>
  <div  <p  </div>
  
  <div>
    <button class="bt_2"style="color:red" id = "Server_T" >T </button>
    <button class="bt_2"style="color:red" id = "Server_H" >H </button>
    <button class="bt_2"style="color:red" id = "Server_Gas" >Gas </button>
    </div>
  

    <div  <p class="Tag" > Điều Khiển Hệ Thống </div>
    <div  <p  </div>
  
    <div>
        <button id = "Alert" class="bt_1" onclick="alertCS()">Check Server</button>
        <button id = "RL1_BT2" class="bt_1" onclick="sendData('Off1')">Tắt Quạt</button>
    <button id = "RL1_BT3" class="bt_1" onclick="sendData('On1')">Bật Quạt</button>
    </div>
    <div>
        <button id = "RL2_BT1" class="bt_1" onclick="sendData('SendData')">Gửi Thông Số</button>
        <button id = "RL2_BT2" class="bt_1" onclick="sendData('Off2')">Tắt Đèn</button>
    <button id = "RL2_BT2" class="bt_1" onclick="sendData('On2')">Bật Đèn</button>
  </div>
    
    
  <div    <p class="Tag" > Trạng Thái Hệ Thống </div>
  <div  <p  </div>
  
  <div>
    <button class="bt_2"style="color:red" id = "Server_Alert" > Nop </button>
    </div>
      
      <script>
        function alertCS(){
            alert("Xác nhận button 1");
        }
  </script>
  
  <script>
  function alerthight(){
      alert(" Server Error");
      }
      </script>
      
    
      <script>
            var Socket;      //Khai báo biến Socket
            function init(){
              //Khởi tạo websocket
              Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
              //Nhận broadcase từ server
              Socket.onmessage = function(event){   
                
                
                var JSONobj = JSON.parse(event.data);   //Tách dữ liệu json
               
                document.getElementById("Server_H").innerHTML = JSONobj.D1;
                document.getElementById("Server_T").innerHTML = JSONobj.D2;
                document.getElementById("Server_Gas").innerHTML = JSONobj.D3;
                document.getElementById("Server_Alert").innerHTML = JSONobj.D4;
               
              }
            }
            //-----------Gửi dữ liệu lên Server-------------------
            function sendData(data){
              Socket.send(data);
            }
      </script>
   </body> 
  </html>
)=====";







void setup() {
  
  //  setup for server
  

  pinMode(Bluelight, OUTPUT);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Bluelight,HIGH);
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay2, LOW);
  digitalWrite(Buzzer, LOW);

  // LCD Init //
  lcd.init();
lcd.backlight();
lcd.print("Whats Up");
lcd.setCursor(4,1);
lcd.print("Bitch !!!");


  // tft set up
/*
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_WHITE); //----------------------------------------------cài đặt màu nền
    printText("T:", ST7735_RED,10,10,2);
    printText("H:", ST7735_RED,10,30,2);//-46: lùi đầu dòng//-70: khoảng cách trên//-2: cỡ chữ
    printText("GAS:", ST7735_RED,10,50,2);
    
 */
  //===============Thiết lập kết nối WiFi=================//
  //WiFi.begin(ssid,pass);
  Serial.begin(9600);
  //Serial.print("Connecting");
//  while(WiFi.status()!=WL_CONNECTED){
//    delay(500);
//    Serial.print("...");
//  }
//  Serial.println(WiFi.localIP());
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  //======Khởi tạo mDNS cấp phát Tên miền E-SMART HOME====//
  if (!MDNS.begin("E-SMARTHOME")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http","tcp",80);
  //=================Khởi tạo Web Server=================//   
 
  webServerStart();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  timer_update.setInterval(1000L, updateStateD);


  //  setup for dht11
  
  delay(10);
  dht.begin();

}
 
void loop() {

// Web Socket Loop
  MDNS.update();  //mDNS
  webServer.handleClient();
  webSocket.loop();
//  timer_update.run();
   h = dht.readHumidity();
   t = dht.readTemperature();
  
  Compareh( h);
  Comparet( t);
  String  D1 = convert(h);
  String  D2 = convert(t);

  // LCD Update

  lcd.setCursor(10,0);
  lcd.print(t);
  lcd.setCursor(10,1);
  lcd.print(h);
  
  String D3, D4;
  
  String JSONtxt = "{\"D1\":\""+D1+"\",\"D2\":\""+D2+"\",\"D3\":\""+D3+"\",\"D4\":\""+D4+"\" }";
  webSocket.broadcastTXT(JSONtxt);
  webSocket.broadcastTXT(JSONtxt);
  delay(500);
}

//*********************************************** Processing ***********************************************************//
//---------------- Convert Data to communicate with Socket Server --------------------//
String convert(int val)
{
  int i;
  char buff[10];
  char valueString[100] = "";
  for (i = 0; i < 10; i++) {
    dtostrf(val, 4, 2, buff);  //4 is mininum width, 6 is precision
    strcat(valueString, buff);
    strcat(valueString, ", ");
  }
  return buff;
}


//---------------- Compare Data to Warning User with all the functions ----------------//

void Compareh(int h)
{
  if( h> H0 && i == 0)
  {
    String Content = "Do am cao";
    digitalWrite(Bluelight, LOW);
    alert = Content;
    i = 1;
    Sendsms(Content);
    
  }
  else  {
    if(h <H1 && i == 0){
        String Content = "Do am thap";
        digitalWrite(Bluelight, LOW);
        alert = Content;
        i =1;
        Sendsms(Content);
    }
    else{
          digitalWrite(Bluelight, HIGH);
          alert  = "NOP";
          i = 0;
    }
  }
}

void Comparet(int t)
{
  if( t>T0 && i ==  0)
  {
      String Content = "Nhiet do cao";
      digitalWrite(Bluelight, LOW);
      alert = Content;
      i = 1;
      Sendsms(Content);
  }
  else{
    if( t< T1 && i == 0){
       String Content = "Nhiet do thap";
       digitalWrite(Bluelight, LOW);
       alert = Content;
       i =1;
       Sendsms(Content);
    }
    else {
      digitalWrite(Bluelight, HIGH);
      i =0;
      alert = "NOP";
      }
  }
}


//=================== WebSocket Functions - Send receive Process  ====================//
void webServerStart(){
  webServer.on("/",mainpage);
  webServer.begin();
}
void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}

// ham xu ly tin hieu doc ve tu webserver //
void webSocketEvent(uint8_t num, WStype_t type,
                    uint8_t * payload,
                    size_t length)
{
  String payloadString = (const char *)payload;
//  Serial.print("payloadString= ");
//  Serial.println(payloadString);
  Rx = (const char*)payload;

  if(payloadString == "Off1")
  {
    digitalWrite(Relay1, HIGH);
  }
  if(payloadString == "On1")
  {
    digitalWrite(Relay1, LOW);
  }
  if(payloadString == "Off2")
  {
    digitalWrite(Relay2, HIGH);
  }
  if(payloadString == "On2")
  {
    digitalWrite(Relay2, LOW);
  }
  if(payloadString == "SMS")
  { 
     String Content2 = "SMS";
    Sendsms(Content2);
  }
  if(payloadString == "SendData")
  {  
  String  D1 = convert(t);
  String  D2 = convert(h);
  String content3 = "{\"Nhietdo\":\""+D1+"\",\"Do am\":\""+D2+"\"}";
  Sendsms(content3);
  }
  
}

void Sendsms(String Content)
{ 
    Serial.print("AT");  //Start Configuring GSM Module
    delay(1000);         //One second delay
    Serial.println();
    Serial.println("AT+CMGF=1");  // Set GSM in text mode
    delay(1000);                  // One second delay
    Serial.println();
    Serial.print("AT+CMGS=");     // Enter the receiver number
    Serial.print("\"+84868696720\"");
    Serial.println();
    delay(1000);
    Serial.print(Content); // SMS body - Sms Text
    delay(1000);
    Serial.println();
    Serial.write(26);
}


ICACHE_RAM_ATTR void updateStateD(){
  String d1 = "";
  String d2 = "";
  
  String JSONtxt = "{\"D1\":\""+d1+"\",\"D2\":\""+d2+"\"}";
//  webSocket.broadcastTXT(JSONtxt);
//  Serial.println(JSONtxt);
}
