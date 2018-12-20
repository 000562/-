  /*******  *HC-SR04超声波传感器、有源蜂鸣器、LED发光二极管******/
#define INTERVAL_NET      1000             //定义发送时间
#include <SoftwareSerial.h>//软件模拟串口
SoftwareSerial mySerial(2, 3); // RX0, TX1  
#include <Wire.h> //调用库  用于I2C通讯的，两个pin是SCL和SDA
#include"ESP8266.h"//wifi模块
////#include <LiquidCrystal.h>//字体库
//#include <avr/mddevice.h>
const int TrigPin = 5; //超声波发射端
const int EchoPin = 4; //超声波接收端
float cm; //定义超声波测量距离变量
int led_pin=7; //LED指示灯
int buzzer_pin=8; //蜂鸣器
 #define  sensorPin_1  A0


#define SSID           "HUAWEI P20"                   // cannot be longer than 32 characters!
#define PASSWORD       "20c2b18124a2"

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

//WEBSITE     
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "503150571"
#define PROJECTID "184230"
#define HOST_PORT   (80)
String apiKey="VyyWIbCk2D7uZD=d=CF=Xyd=9oI=";
char buf[10];
#include <SoftwareSerial.h>
ESP8266 wifi(mySerial);
////ESP8266 wifi(Serial1);                                      //定义一个ESP8266（wifi）的对象？
unsigned long net_time1 = millis();                          //数据上传服务器时间
void setup(void)     //初始化函数  
{       
  //初始化串口波特率  
    Wire.begin();
    Serial.begin(9600);   
    while(!Serial);
    pinMode(sensorPin_1, INPUT);

   //ESP8266初始化
    Serial.print("setup begin\r\n");   

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {      //加入无线网
    Serial.print("Join AP success\r\n");  
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

  Serial.print("setup end\r\n");
    pinMode(TrigPin, OUTPUT);
pinMode(EchoPin, INPUT);
  //lcd.begin(16, 2);
    Serial.begin(9600); //设置通讯的频率
    pinMode(TrigPin, OUTPUT); //超声波发射端设置针脚为输出
    pinMode(EchoPin, INPUT); //接收端设置为输入
   pinMode(led_pin,OUTPUT);   //LED灯的正极针脚，是输出端
      pinMode(buzzer_pin,OUTPUT);   //蜂鸣器的正极针脚，是输出端
} 
void loop() 
{ 
        digitalWrite(led_pin,LOW);//先让LED熄灭
                digitalWrite(buzzer_pin,LOW);//先让蜂鸣器不响
        digitalWrite(TrigPin, LOW);//发送一个低电平
        delayMicroseconds(2); //延时
        digitalWrite(TrigPin, HIGH); //在发送高电平
        delayMicroseconds(10);//延时
        digitalWrite(TrigPin, LOW); //发送低电平 
       cm= pulseIn(EchoPin, HIGH) / 58.00; //将返回声波时间换算成厘米，pulseIn()单位为微秒，声速344m/s，所以距离cm=344*100/1000000*pulseIn()/2（往返时间）约等于pulseIn()/58.08；pulseIn()读取脉搏即从高电频开始读取数据
       //通过串口监视器输出
       Serial.print("距离");
        Serial.println(cm);//此句用于测试，可注释掉
        Serial.println("cm");
        cm=(int(cm*100))/100;//先取整，然后保留两位小数
        if (cm>=0&&cm<=20) //设定一个范围，如果在这个范围内指示灯就亮
     {  
      digitalWrite(led_pin,HIGH);//满足条件就输出高电平，指示灯亮
     digitalWrite(buzzer_pin,HIGH);//满足条件就输出高电平，蜂鸣器响
     delay(100); //延时200毫秒
      digitalWrite(led_pin,LOW);//熄灭LED
     digitalWrite(buzzer_pin,LOW);//关闭蜂鸣器
     }

if (net_time1 > millis())  net_time1 = millis();
  
  if (millis() - net_time1 > INTERVAL_NET)                  //发送数据时间间隔
  {                
    updateSensorData();                                     //将数据上传到服务器的函数
    net_time1 = millis();
  }
  
}
void updateSensorData() {
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");

String jsonToSend;
jsonToSend="{\"distance\":";
    jsonToSend+="\"";
    jsonToSend += 4534634;
    jsonToSend+="\",";
    
    jsonToSend+="\"Humidity\":";
    jsonToSend+="\"";
    jsonToSend += 34525;
    jsonToSend+="\",";
    
    jsonToSend+="\"cm\":";
    jsonToSend+="\"";
    jsonToSend += cm;
    jsonToSend+="\",";
    
    //jsonToSend+="\"Light\":";
    //jsonToSend+="\"";
    //jsonToSend += 1232;
    //jsonToSend+="\"";
    
    jsonToSend+="}";
Serial.println(jsonToSend);
    String postString;                                //用于存储发送数据的字符串
    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
  
}
