#include "stdio.h"

#include "DHT.h"
const int DHTTYPE = DHT11; 

const int DHTPin = 4;
const int Relay1 = 5;
const int Relay2 = 16;


String Rx; 
String alert;
int Ttemp1 = 0;
int Htemp1 = 0;
int Datatemp = 0;

float H0 = 90;
float H1  = 20;
float T0 = 33;  
float T1 = 20;


float Tx,Hx;
String D4; 

 
DHT dht(DHTPin, DHTTYPE);     // Initialize DHT sensor.
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];



// Include for Server
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>  //Thư viện mDNS
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer webServer(80);
BlynkTimer timer_update;
char* ssid = "Nguyen";
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
            background-color:#00FE00;
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
  
    <button class="bt_2"style="color:pink" id = "Nhiệt" >D1 </button>
  <button class="bt_2"style="color:pink" id = "Độ ẩm" >D2 </button>
  
    </div>
<h2 class = "Tag"> Bật Tắt </h2>  

  <div>
    <button class="bt_2"style="color:red"onclick="sendData(OFF1)" id = "BUTTON1" > OFF1 </button>
  <button class="bt_2"style="color:red"onclick="sendData(ON1)" id = "BUTTON2" > ON1 </button>
  
<div>
  <button class="bt_2"style="color:red"onclick="sendData(OFF2)" id = "BUTTON3" > OFF2 </button>
  <button class="bt_2"style="color:red"onclick="sendData(ON2)" id = "BUTTON4" > ON2 </button>
<div> 
  <button class="bt_2"style="color:red"onclick="alertCS()" id = "check" > check </button>
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
               
                
                document.getElementById("Nhiệt").innerHTML = JSONobj.D1;
        document.getElementById("Độ ẩm").innerHTML = JSONobj.D2;
        
               
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
  


  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);


 
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
  MDNS.update();  //mDNS
  webServer.handleClient();
  webSocket.loop();

 
  float hx =  readdht11_readh();
  float tx =  readdht11_readt();


  Hx = hx;
  Tx = tx;
  float blank  ;


  String  D1 = convert(hx);
  String  D2 = convert(tx);

  Comparet(tx);
  Compareh(hx);


 
  
  String JSONtxt = "{\"D1\":\""+D1+"\",\"D2\":\""+D2+"\"}";
  webSocket.broadcastTXT(JSONtxt);
  
  delay(500);
  
  
}

float readdht11_readh(){
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  float hic;
  float hif;
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
   
  }
  else {
    /*
    delay(10);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
//    Serial.print(f);
//    Serial.print(" *F\t Heat index: ");
//    Serial.print(hic);
 //   Serial.print(" *C ");
//    Serial.println(hif);
*/
  }
  return h;
}
float readdht11_readt(){
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  float hic;
  float hif;
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
   
  }
  else {
    delay(20);
/*    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.println(hif);
    */
  }
  return t;
}

void Compareh(float h)
{
  if( h> H0 && Ttemp1  == 0)
  {
    String Content = "Do am cao";
    alert = Content;
    Ttemp1 = 1;
    }
  else  {
    if(h <H1 && Ttemp1  == 0){
        String Content = "Do am thap";
        alert = Content;
        Ttemp1  =1;

    }
    else{
          alert  = "NOP";
          Ttemp1  = 0;
    }
  }
}

void Comparet(float t)
{
  if( t>T0 && Htemp1  ==  0)
  {
      String Content = "Nhiet do cao";
      alert = Content;
      Htemp1  = 1;
  }
  else  if( t< T1 && Htemp1  == 0){
       String Content = "Nhiet do thap";
       alert = Content;
       Htemp1  =1;
    }
    else {
      Htemp1  =0;
      alert = "NOP";
      }
  
}



String convert(float val)
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
  Rx = (const char*)payload;

  if(payloadString == "OFF1")
  {
    digitalWrite(Relay1, HIGH);
  }
  if(payloadString == "ON1")
  {
    digitalWrite(Relay1, LOW);
  }
  if(payloadString == "OFF2")
  {
    digitalWrite(Relay2, HIGH);
  }
  if(payloadString == "ON2")
  {
    digitalWrite(Relay2, LOW);
  }  
}



ICACHE_RAM_ATTR void updateStateD(){
  String d1 = "";
  String d2 = "";
  
  String JSONtxt = "{\"D1\":\""+d1+"\",\"D2\":\""+d2+"\"}";
//  webSocket.broadcastTXT(JSONtxt);
//  Serial.println(JSONtxt);
}
