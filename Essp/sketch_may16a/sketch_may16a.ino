// Include for Server
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>  //Thư viện mDNS
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer webServer(80);
BlynkTimer timer_update;
char* ssid = "Control SysTem";
char* pass = "12345678"; 


//Include for DHT11
#include "DHT.h"
#define DHTPIN 2// what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);


// Include for LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);


// Include for Comunicate
typedef struct {
  String Hum;
  String Temp;
  String rTemp;
  String rHum;
}DataCom  ;

int H0 = 75;
int H1 = 30;
int T0 = 35;
int T1 = 25;
String D3, D4;


String FinalSend;




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
   
    <h1 class = 'Header'> ĐỒ ÁN GIÁM SÁT  </h1> 
    
    
    <div    <p class="Tag" > Thông số  </div>
  <div  <p class="Tag" > Nhiệt độ - Độ Ẩm </div>
  <div  <p  </div>
  

    <button class="bt_2"style="color:red" id = "Temp" >D4 </button>
   <button class="bt_2"style="color:red" id = "Hum" >D4 </button>
   
    </div>
    <div  <p  </div>
  

    <button class="bt_2"style="color:red" id = "Temp_A" >Temp_Alert </button>
  <button class="bt_2"style="color:red" id = "Hum_A" >Hum_Alert </button>
  

   
    </div>

   
  <div>
    <button class="bt_2"style="color:red"onclick="alertCS()" id = "Check Connection" > Check_Connection </button>
    </div>
      
      <script>
        function alertCS(){
            alert("Connect Successfully");
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
               
                
                document.getElementById("Hum").innerHTML = JSONobj.D1;
        document.getElementById("Temp").innerHTML = JSONobj.D2;
        document.getElementById("Hum_A").innerHTML = JSONobj.D3;
        document.getElementById("Temp_A").innerHTML = JSONobj.D4;
               
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
  // LCD Init //
  lcd.init();
lcd.backlight();
lcd.print("Whats Up");
lcd.setCursor(4,1);
lcd.print("Bitch !!!");

  
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


  // Setup for Project 
  dht.begin();


}
 
void loop() {
  MDNS.update();  //mDNS
  webServer.handleClient();
  webSocket.loop();
  
// LCD
lcd.setCursor(0,0);
lcd.print("Nhiet Do:");
lcd.setCursor(0,1);
lcd.print("Do Am:");

// Read DHT11
  int h = dht.readHumidity();
  int t = dht.readTemperature();

// upload to lcd

lcd.setCursor(10,0);
lcd.print(t);
lcd.setCursor(10,1);
lcd.print(h);
  

  DataCom  DataSend;
 
  DataSend.Hum=char( h/10+48);
  DataSend.rHum=char(h%10+48);
  DataSend.Temp=char(t/10+48);
  DataSend.rTemp=char(t%10+48);

 

  FinalSend = DataSend.Hum;
  FinalSend += DataSend.rHum;
  FinalSend += DataSend.Temp;
  FinalSend += DataSend.rTemp;


  Serial.println(FinalSend);

  

  Compareh( h);
  Comparet( t);


 /* 
  Serial.println(DataSend.Hum);
  Serial.println(DataSend.Temp);
  Serial.println(DataSend.UpTemp);
  Serial.println(DataSend.DownTemp);
  Serial.println(DataSend.UpHum);
  Serial.println(DataSend.DownTemp);
  */
  
  String  D1 = convert(h);
  String  D2 = convert(t);


  
  String JSONtxt = "{\"D1\":\""+D1+"\",\"D2\":\""+D2+"\",\"D3\":\""+D3+"\",\"D4\":\""+D4+"\" }";
  webSocket.broadcastTXT(JSONtxt);
//  String RX = webSocketEvent();
//  Serial.print("Rx nay :");
//  Serial.println(Rx);
  
  delay(500);
}

//==================Void===================//
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
///////////////////////////

void Compareh(int h)
{
  if( h> H0 )
  {
   D3 = "Do Am Cao" ;
    
  }
  else  {
    if(h <H1){
        D3 = "Do Am Thap ";
    }
    else{
   D3 = "non";
    }
  }
}

void Comparet(int t)
{
  if( t>T0)
  {
      D4 = "Nhiet Do Cao";
  }
  else  if( t< T1){
       D4 = "Nhiet Do Thap";
    }
    else {
      D4 = "non";
      }
  
}




//===================Chương trình con====================//
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
 
  
}


ICACHE_RAM_ATTR void updateStateD(){
  String d1 = "";
  String d2 = "";
  
  String JSONtxt = "{\"D1\":\""+d1+"\",\"D2\":\""+d2+"\"}";
//  webSocket.broadcastTXT(JSONtxt);
//  Serial.println(JSONtxt);
}
