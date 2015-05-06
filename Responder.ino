//4responder Router
/*
 core hardware(RX0,TX1) =9600
 core software(D2,D3)=9600  must match with cc2530 setup.when change cc2530 baud  rate
 */
#include <SoftwareSerial.h>
#include <stdio.h>
SoftwareSerial mySerial(2, 3); // RX, TX
String myStringSoftCom="";
String myStringHardCom="";
String myString="Stu_Num=20140201,Stu_Answer=";
void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(9, INPUT);    //button1
  pinMode(10, INPUT);    //button2
  pinMode(11, INPUT);    //button3
  pinMode(12, INPUT);    //button4
  mySerial.begin(9600);//zigbee通信使用软串口
}
 
void loop() // run over and over
{
  int Stu_Answer=0;
  int Stu_Num=1;//学号
  int AnsPin=8;//led引脚
//  int CanAnsPin=7;
  char answer='a';//答案
  char reply;
//  boolean stu_answered=false;
  digitalWrite(AnsPin,LOW);//led灭，表示没有发送答案
//  delay(1000);
/*从按键获取答案的Int形式*/
  for(int i=9; i<=12; i++){
   if(digitalRead(i)==HIGH){
     Stu_Answer=i-8;
//     Serial.println(Stu_Answer,DEC);
//      break;
   }
 }
 /*将int形式的答案转换为char形式的答案*/
 if(Stu_Answer>=1&&Stu_Answer<=4){
   Serial.println(Stu_Answer,DEC);
 //  stu_answered=true;
   switch(Stu_Answer){
   case 1:answer='A';break;
   case 2:answer='B';break;
   case 3:answer='C';break;
   case 4:answer='D';break;
   }
   Serial.println(answer);
 }
/* if(stu_answered==true){
 digitalWrite(AnsPin,HIGH);//灯亮，开始发送答案
 delay(1000);
 }*/
  while (Serial.available() > 0)  
  {
    myStringHardCom += char(Serial.read());
    delay(2);
  }if(Stu_Answer>=1&&Stu_Answer<=4){
    myStringHardCom=myString+answer;}
  if(myStringHardCom!="")
    mySerial.println(myStringHardCom); 
  myStringHardCom="";
  while (mySerial.available() > 0)  
  {
    myStringSoftCom += char(mySerial.read());
    delay (2);
  }
  if (myStringSoftCom.length() > 0)
  {   
    Serial.print(myStringSoftCom);
    reply=myStringSoftCom[10];
    Serial.println(reply);
    //    Serial.print("1");
    if(reply=='Y')
  {
 digitalWrite(AnsPin,HIGH);
 delay(1000);
 }
// else if(reply=='y'){
// digitalWrite(CanAnsPin,HIGH);
// delay(1000);
// }
    myStringSoftCom="";
  }
  delay(100);
}
