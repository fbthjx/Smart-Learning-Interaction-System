#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include <ctype.h>
#include <SoftwareSerial.h>
#include "utility/debug.h"
#include "utility/socket.h"
#define ADAFRUIT_CC3000_IRQ   2  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  9
#define ADAFRUIT_CC3000_CS    10
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed but DI
/* here  is  the wifi name and password */
#define WLAN_SSID       "YOUR_WIFI"        //cannot be longer than 32 characters!
#define WLAN_PASS       "PASSWORD"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2
/*这是定义超时连接的时间阈值*/
#define IDLE_TIMEOUT_MS  3000  
#define READ_ANSWER_TIMEOUT 5000 
/*这里是网站的地址和网页的接口*/
#define WEBSITE2      "218.249.255.24"
//webpage to get 
#define WEBPAGE2      "/open_info_api/quizzes/question/answers"

/****************************************************************************/

uint32_t ip2;//IPAddress server(218.249.255.24);
SoftwareSerial mySerial(5, 6);  //设定软串口
//const int buttonPin = 3;     // 定义按键输入引脚
const int ledPin =  8;     //定义led引脚
//int buttonState = 0;        //初始化按键值
int randomnum=0;
String Answer="";  //答题器的答案
String student=""; //学生号 
char answer='e';    //用于存放char形式的学生答案
void setup() {
  /*setup部分主要是建立wifi及HTTP的连接*/
  Serial.begin(9600);  /*定义波特率*/
  mySerial.begin(9600); 
//  pinMode(ledPin, OUTPUT);    //设置led引脚为输出状态   
//  pinMode(buttonPin, INPUT); //设置按键引脚为输入状态  
  
   Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())/*启动wifi*/
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  /*连接wifi,用上面给的用户名与密码*/
   if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }   
  Serial.println(F("\nConnected!\n"));
  Serial.println(F("-------------------------------------"));
    /*设置ip并ping下服务器*/
  ip2=cc3000.IP2U32(218,249,255,24);
  cc3000.printIPdotsRev(ip2); 
  Serial.println();
  Serial.println(ip2);
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip2); Serial.print("...");
  Serial.print(cc3000.ping(ip2, 5)); Serial.println(F(" replies"));
  Serial.println(F("-------------------------------------")); 
  delay(1000);
}
void loop(){
//  buttonState = digitalRead(buttonPin);//从指定的数字引脚读取数值，是高还是低。
//  if (buttonState == HIGH) {     
    digitalWrite(ledPin, HIGH); //如果按键信号是高，则led点亮（led接法阳极接控制脚，阴极接GND）
    delay(1000);
    getAnswer();//获取学生答案

    Serial.println(student);
    Serial.println(Answer);
    if(answer>='A'&&answer<='D'){ //如果答案为A,B,C,D则发送答案，answer是通过getAnswer（）赋值的
    sendData(student,Answer);}//发送学号和答案至服务器
    student="";
    Answer="";
    digitalWrite(ledPin, LOW);
    delay(1000);
//    digitalWrite(ledPin, LOW);
//    delay(100);
    } 
//  else {
//    digitalWrite(ledPin, LOW); //否则熄灭
//  }
//}

String studentRandom(void){
 randomnum= randomnum+random(1,4);
  switch(randomnum%10){
    case 0:return "20140201";break;
    case 1:return "20140202";break;
    case 2:return "20140203";break;
    case 3:return "20140204";break;
    case 4:return "20140205";break;
    case 5:return "20140206";break;
    case 6:return "20140207";break;
    case 7:return "20140208";break;
    case 8:return "20140209";break;
    default:return "20140210";break;}
}

String answerRandom(void){
  randomnum++;
  switch(randomnum%4){
    case 0:return "A";break;
    case 1:return "B";break;
    case 2:return "C";break;
    default:return "D";break;}
}

void sendData(String student_id,String answer_id){
  Adafruit_CC3000_Client www2 = cc3000.connectTCP(ip2, 80);
    if (www2.connected()) {
    www2.fastrprint(F("POST "));    
    www2.fastrprint(WEBPAGE2);
    www2.fastrprint(F(" HTTP/1.1\r\n"));
    www2.fastrprint(F("Host: "));www2.fastrprint(WEBSITE2); 
    www2.fastrprint(F("\r\n")); 
    www2.fastrprint(F("Content-Type: application/x-www-form-urlencoded")); 
    www2.fastrprint(F("\r\n")); 
    www2.fastrprint(F("Content-Length: 31")); 
    www2.fastrprint(F("\r\n"));     
    www2.fastrprint(F("\r\n")); 
    www2.fastrprint(F("student_id=")); 
    char  strStu[10];
    student_id.toCharArray(strStu,student_id.length()+1);     
    www2.fastrprint(strStu); 
    www2.fastrprint(F("&answer_id="));     
    char  strAns[10];
    answer_id.toCharArray(strAns,answer_id.length()+1);     
    www2.fastrprint(strAns);     
    www2.fastrprint(F("\r\n"));
    www2.println();
    Serial.println(F("here is what"));
 } else {
    Serial.println(F("Connection failed"));    
    return;
 } 
 Serial.println(F("-------------------------------------"));
   unsigned long lastRead2 = millis();
   //String str,str1;
  while (www2.connected() && (millis() - lastRead2 < IDLE_TIMEOUT_MS)) {
    while (www2.available()) {
      char c = www2.read();         
      Serial.print(c);
      lastRead2 = millis();
    }
  }
  www2.close();       
  //delay(200);
}
/*getAnswer函数用于获取答题器发送的学生IDstudent和答案Answer*以及向答题器返回一个确认信息*/
/*这一函数是答题器网关和答题器通过zigbee进行通信，通信时需要传输特定格式的字符串，具体参看microduino,zigbee模块的部分*/
 void getAnswer(){
  String myStringSoftCom="";//从答题器收到的字符串
  String myString="";
  String myReply="";//返回给答题器的字符串
  String myReplyHead="+ZBD=";
  String myReplyEnd=",YES";
  String shortAdd="";
  String studentId="";//用于存放解析出的学号
//  char answer='e';
   while (mySerial.available() > 0)  
  {
    myStringSoftCom += char(mySerial.read());
    delay (2);
  }
  if (myStringSoftCom.length() > 0)
  {   
    for(int i=10;i<myStringSoftCom.length()-2;i++) 
      myString+=myStringSoftCom[i];
      answer=myString[28];
    Answer=Answer+myString[28];//学生答案
    for(int i=5;i<=8;i++){
    shortAdd+=myStringSoftCom[i];
    }
    for(int i=8;i<=15;i++){
    studentId+=myString[i];
    }
    student=studentId;//学号
    if(answer>='A'&&answer<='D'){
    myReply=myReplyHead+shortAdd+myReplyEnd;
    mySerial.println(myReply);
    }
    myReply="";
    shortAdd="";
    studentId="";
    myString="";
    myStringSoftCom="";
  }
}
