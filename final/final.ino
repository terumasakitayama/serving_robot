
#define PI 3.141592653589793 
// Rotary Encoder
int valA, valB;
int stateA, stateB;
int edgeA, edgeB;
int countA, countB;

//サーボモータ
#include <Servo.h>
Servo servo1;
Servo servo2;
int dir=10;//サーボモータが一度で動く角度

//ピン番号

const int PHASE_A = 5; // Encoder phase Aロータリーエンコーダ
const int PHASE_B = 4; // Encoder phase B
const int AIN1 = 9;
const int AIN2 = 10;
const int PWMA = 11;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMB =6;
#define TRIG2 1
#define ECHO2 3
#define TRIG1 0
#define ECHO1 13
const int therm = A0;//温度センサ
const int light1 = A1; // 光センサ１
const int light2 = A2; // 光センサ２
const int light3 = A3;// 光センサ３
const int light4 = A4;// 光センサ4


// Motor Control

int tirer=14;//タイヤの半径
int lshaf=90;//シャフトの長さ
const int cpr = 24; //count per rotation
float Kp1 = 2.5;//目的距離による比例ゲイン
float Kp1A = 0.1;
float Kp1B = 0.1;
float Kp2 =10;//差分による比例ゲイン
float Kp3 =10;// 目標角度による比例ゲイン
int constant = 140 ; //モータが動く最低限の出力
int Max = 230; //モータに与える出力の上限
int maxd =60;//差分項の上限
int Max2 = 200; //曲がるときのモータに与える出力の上限
int A,B;//回転数の加減



//位置決め
int n;

//光センサ

int black = 1000; //黒色の時のセンサの値（精度のいい方）////////////////////////////////////////////////////////////////////////////
int black34 = 1000; //黒色の時のセンサの値（精度の悪い方）

//超音波センサ
#include "SR04.h"
SR04 sr041 = SR04(ECHO1,TRIG1);
int distance1 = 50;//ヒーターとの距離（mm)

SR04 sr042 = SR04(ECHO2,TRIG2);

//温度センサ

int T;//測った温度から得られる値

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
// Motor Driver
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  pinMode(TRIG1,OUTPUT);
  pinMode(ECHO1,INPUT);
  pinMode(TRIG2,OUTPUT);
  pinMode(ECHO2,INPUT);
  pinMode(PHASE_A,INPUT);
  pinMode(PHASE_B,INPUT);
  servo1.attach(2);
servo2.attach(12);
  servo1.write(90);
  servo2.write(90);

  
  delay(2000);
}


//関数の説明
//直進　void gostraight(int dref)
//後退　void backstraight(int dref)
//左折　void turnleft(int deg)
//右折　void turnright(int deg)
//光センサ１、２で止まる（左回り）　void turnleftlight(int deg){//degは大まかな角度の値
// 光センサ１，２で止まる（右回り）　void turnrightlight(int deg){//degは大まかな角度
//黒線に沿って前進、横の超音波センサで止まる　void gostraightsr1()
//黒線にそって後退、横の超音波センサで止まる　void backstraightsr1()
//前の超音波センサで左回転が止まる　void turnleftsr2(int deg,int d){// 大まかな角度と止まる時の距離
//前の超音波センサで右回転をとめる　void turnrightsr2(int deg,int d){//degは大まかな角度, dは物体との距離
//前の超音波センサで直進が止まる　void gostraightsr2(int d){//dは物体をつかむときの物体との距離
//超音波センサで後退が止まる void backstraightsr2(int d){//dは物体をつかむときの物体との距離
//逆まわり左回転　void backturnleft(int deg)
//黒線に垂直にする関数　void vert(int dref){ //dは大まかな距離
//光センサ３で直進が止まる　void gostraightlight(int dref){//dは大まかな距離
//アームを閉じる　void closearm()
//アームを開ける　void openarm()
//アームをおろす　void downarm(int angleref)
//アームをあげる　void liftarm(int angleref)
//温度を測る void temperature()
int angle=0;
void loop() {
  openarm();
  delay(2000);
  downarm(0);
  delay(2000);
  gostraight(600);
  delay(2000);
  closearm();
  delay(2000);
  liftarm(160);
  delay(2000);
  turnright(40);
  delay(2000);
  gostraight(20);
  delay(2000);
  downarm(0);
  delay(2000);
  openarm();
  delay(2000);
  //一つ目クリア
  liftarm(160);
  delay(2000);
  backstraight(100);
  delay(2000);
  turnleft(190);
  delay(2000);
  openarm();
  delay(2000);
  downarm(0);
  delay(2000);
  gostraight(100);
  delay(2000);
  closearm();
  delay(2000);
  liftarm(160);
  delay(2000);
  turnright(90);
  delay(2000);
  gostraight(200);
  delay(2000);
  downarm(0);
  delay(2000);
  openarm();
  delay(2000);
  backstraight(400);
  delay(2000);
  turnright(30);
  delay(2000);
  
}


//直進
void gostraight(int dref){
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB;
   int uA,uB;
   Serial.println("gostraight");
   while((dref>tirer*(360*countA/cpr)*(PI/180))&&(dref>tirer*(360*countB/cpr)*(PI/180))){ //-5は調整
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
      if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);
      
        uA = min((dref-dA)*Kp1A,maxd)+Kp2*(dB-dA);
      
        uB = min((dref-dB)*Kp1B,maxd)+Kp2*(dA-dB);
      
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));
 }
}


//後退
void backstraight(int dref){
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB;
   float Kp1=0.3;
   float Kp2=0.8;
   int uA,uB;
   Serial.println("backstraight");
   while((dref>tirer*(360*countA/cpr)*(PI/180))&&(dref>tirer*(360*countB/cpr)*(PI/180))){ //-5は調整
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
     if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

      uA = min((dref-dA)*Kp1A,maxd)+Kp2*(dB-dA);
      
      uB = min((dref-dB)*Kp1B,maxd)+Kp2*(dA-dB);
        
     
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));
 }
}

//左折
void turnleft(int deg){
   int edgeB;
   int valB;
   int stateB=0;
   int countB=0;
   Serial.println("turnright");
   while(lshaf*deg*(PI/180)+5>tirer*(360*countB/cpr)*(PI/180)){ //+5は調整
      valB = digitalRead(PHASE_B);
      edgeB=0;
      if ((valB ==1) && (stateB == 0)) {
         stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
         stateB = 0; edgeB = -1; // falling edge
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countB=");
      Serial.println(countB);
      int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countB/cpr)*(PI/180));
 
      if (u>0) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         
      } else {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         u=0;
      }
      analogWrite(PWMA, 0 );
      analogWrite(PWMB, min(u+constant, Max2));
   }
}

//右折
void turnright(int deg){
     int edgeA;
     int valA;
     int stateA=0;
     int countA=0;
     Serial.println("turnleft");
     while(lshaf*deg*(PI/180)+5>tirer*(360*countA/cpr)*(PI/180)){ //-5は調整
        valA = digitalRead(PHASE_A);
        edgeA=0;
     if ((valA ==1) && (stateA == 0)) {
        stateA = 1; edgeA = 1; // rising edge
     }
     if ((valA ==0) && (stateA == 1)) {
        stateA = 0; edgeA = -1; // falling edge
     }
     if(edgeA==1){
        countA ++;
        Serial.println(countA);
     }
     if(edgeA==-1){
        countA ++;
        Serial.println(countA);
     }
     int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countA/cpr)*(PI/180));
     if (u>0) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     } else {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        u=0;
     }
        analogWrite(PWMA, min(u+constant, Max2));
        analogWrite(PWMB, 0 );
     }
}


//光センサで止まる（左回り）
void turnleftlight(int deg){//degは大まかな角度の値
   int edgeB;
   int valB;
   int stateB=0;
   int countB=0;
   Serial.println("turnrightlight");
   while(analogRead(light1)<black){
      analogRead(light1);
      valB = digitalRead(PHASE_B);
      edgeB=0;
      if ((valB ==1) && (stateB == 0)) {
         stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
         stateB = 0; edgeB = -1; // falling edge
      }
      if(edgeB==1){
         countB ++;
      }
      if(edgeB==-1){
         countB ++;
      }
      Serial.print("countB=");
      Serial.println(countB);
      int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countB/cpr)*(PI/180));
 
      if (u>0) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } else {
        u=0;
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      }
      analogWrite(PWMA, 0);
      analogWrite(PWMB, min(u+constant, Max2));
   }
}

// 光センサで止まる（右回り）
void turnrightlight(int deg){//degは大まかな角度
     int edgeA;
     int valA;
     int stateA=0;
     int countA=0;
     Serial.println("turnleftlight");
     while(analogRead(light2)<black){ 
        analogRead(light2);
        valA = digitalRead(PHASE_A);
        edgeA=0;
     if ((valA ==1) && (stateA == 0)) {
        stateA = 1; edgeA = 1; // rising edge
     }
     if ((valA ==0) && (stateA == 1)) {
        stateA = 0; edgeA = -1; // falling edge
     }
     if(edgeA==1){
        countA ++;
        Serial.println(countA);
     }
     if(edgeA==-1){
        countA ++;
        Serial.println(countA);
     }
     int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countA/cpr)*(PI/180));
     if (u>0) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     } else {
        u=0;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     }
        analogWrite(PWMA, min(u+constant, Max2));
        analogWrite(PWMB,0);
     }
}

//黒線に沿って前進、横の超音波センサで止まる。
void gostraightsr1(){
   int uA, uB;
   long a;
   int lef, ri;
   Serial.println("gostraightsr1");
   while(a>distance1){ //distance1は大体ヒーターと直線との距離
      a = sr041.Distance()*10;//mmに直した
      analogRead(light1);
      analogRead(light2);

      if(analogRead(light1)>black&&analogRead(light2)>black){//両方出てない
        uA = constant;
        uB = constant+20; 
        lef=0;
        ri=0;
      }
       if(analogRead(light1)<=black&&analogRead(light2)>black){//左だけ出ている
        uA =constant+40 ;
        uB =constant ; 
        lef++;
      }
       if(analogRead(light1)>black&&analogRead(light2)<=black){//右だけ出ている
        uA =constant;
        uB =constant+40 ; 
        ri++;
      }
       if(analogRead(light1)<=black&&analogRead(light2)<=black&&lef>ri){//左右どちらとも黒線の左側に出ている
        uA =constant+50;
        uB =constant ; 
       }
      if(analogRead(light1)<=black&&analogRead(light2)<=black&&lef>ri){//左右どちらとも黒線の右側に出ている
        uA =constant ;
        uB =constant+50 ; 
       }
      
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
    
      analogWrite(PWMA, min(uA, Max));
      analogWrite(PWMB, min(uB, Max));
 }
}

//黒線にそって後退、横の超音波センサで止まる
void backstraightsr1(){
   int uA, uB;
   long a;
   int lef, ri;
   Serial.println("backstraightsr1");
   while(a>distance1){ 
      a = sr041.Distance()*10;//mmに直した
      analogRead(light1);
      analogRead(light2);

      if(light1>black&&light2>black){//両方出てない
        uA = constant;
        uB = constant; 
        lef=0;
        ri=0;
      }
       if(light1<=black&&light2>black){//左だけ出ている
        uA =constant ;
        uB =constant-70 ; 
        lef++;
      }
      
       if(light1>black&&light2<=black){//右だけ出ている
        uA =constant-70 ;
        uB =constant ; 
        ri++;
      }
       if(light1<=black&&light2<=black&&lef>ri){//左右どちらとも黒線の左側に出ている
        uA =constant+20 ;
        uB =constant-70 ; 
       }
      if(light1<=black&&light2<=black&&lef>ri){//左右どちらとも黒線の右側に出ている
        uA =constant-70 ;
        uB =constant+20 ; 
       }
      
      
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
    
      analogWrite(PWMA, min(uA, 150));
      analogWrite(PWMB, min(uB, 150));
 }
}

//超音波センサで左回転が止まる
void turnleftsr2(int deg,int d){// 大まかな角度と止まる時の距離
   long a;
   int edgeB;
   int valB;
   int stateB=0;
   int countB=0;
   Serial.println("turnrightsr");
   while(a>d){
      a=sr042.Distance()*10;
      valB = digitalRead(PHASE_B);
      edgeB=0;
      if ((valB ==1) && (stateB == 0)) {
         stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
         stateB = 0; edgeB = -1; // falling edge
      }
      if(edgeB==1){
         countB ++;
      }
      if(edgeB==-1){
         countB ++;
      }
      Serial.print("countB=");
      Serial.println(countB);
      int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countB/cpr)*(PI/180));
 
      if (u>0) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } else {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW); 
      }
      analogWrite(PWMA, 0 );
      analogWrite(PWMB,min(abs(u)+constant, Max2));
   }
}

//超音波センサで右回転をとめる
void turnrightsr2(int deg,int d){//degは大まかな角度, dは物体との距離
     long a;
     int edgeA;
     int valA;
     int stateA=0;
     int countA=0;

     Serial.println("turnleftsr");
     while(a>d){ 
        a=sr042.Distance()*10;
        valA = digitalRead(PHASE_A);
        edgeA=0;
     if ((valA ==1) && (stateA == 0)) {
        stateA = 1; edgeA = 1; // rising edge
     }
     if ((valA ==0) && (stateA == 1)) {
        stateA = 0; edgeA = -1; // falling edge
     }
     if(edgeA==1){
        countA ++;
        Serial.println(countA);
     }
     if(edgeA==-1){
        countA ++;
        Serial.println(countA);
     }
     int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countA/cpr)*(PI/180));
     if (u>0) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     } else {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
     }
        analogWrite(PWMA, min(abs(u)+constant, Max));
        analogWrite(PWMB,0);
     }
}

//超音波センサで直進が止まる
void gostraightsr2(int d){//dは物体をつかむときの物体との距離
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB;
   long a;
   int uA,uB;
   Serial.println("gostraightsr");
   while(a>d){ //-5は調整
      a = sr042.Distance()*10;
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
      if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

      
        uA = abs(a-d)*Kp1+Kp2*(dB-dA);
        uB = abs(a-d)*Kp1+Kp2*(dA-dB);
      
   
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
       
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));
 }
}

//超音波センサで後退が止まる
void backstraightsr2(int d){//dは物体をつかむときの物体との距離
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB;
   long a;
   int uA,uB;
   Serial.println("gostraightsr");
   while(a>d){ //-5は調整
      a = sr042.Distance()*10;
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
      if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

      
        uA = abs(a-d)*Kp1+Kp2*(dB-dA);
      
        uB = abs(a-d)*Kp1+Kp2*(dA-dB);
      
      
   
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));
 }
}


//逆まわり左回転
void backturnleft(int deg){
     int edgeA;
     int valA;
     int stateA=0;
     int countA=0;

     Serial.println("backturnleft");
     while(lshaf*deg*(PI/180)+5>tirer*(360*countA/cpr)*(PI/180)){ //-5は調整
        valA = digitalRead(PHASE_A);
        edgeA=0;
     if ((valA ==1) && (stateA == 0)) {
        stateA = 1; edgeA = 1; // rising edge
     }
     if ((valA ==0) && (stateA == 1)) {
        stateA = 0; edgeA = -1; // falling edge
     }
     if(edgeA==1){
        countA ++;
        Serial.println(countA);
     }
     if(edgeA==-1){
        countA ++;
        Serial.println(countA);
     }
     int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countA/cpr)*(PI/180));
     if (u>0) {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
     } else {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
     }
        analogWrite(PWMA, min(abs(u)+10, Max2));
        analogWrite(PWMB, 0 );
     }
}

//光センサで直進が止まる
void gostraightlight(int dref){//dは大まかな距離
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB;
   float Kp1=0.3;
   float Kp2=10;
   float b;
   int uA,uB;
   Serial.println("gostraightlight");
   while(b>black34){ //-5は調整
      b = analogRead(light3);
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
      if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);
      
        uA = (dref-dA)*Kp1+Kp2*(dB-dA);
        uB = (dref-dB)*Kp1+Kp2*(dA-dB);
      
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
        uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, HIGH);
         digitalWrite(AIN2, LOW);
         digitalWrite(BIN1, HIGH);
         digitalWrite(BIN2, LOW);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));
 }
}

 //黒線に垂直にする関数
void vert(int dref){ //dは大まかな距離
   int state1,state2;
   int edgeA,edgeB;
   int valA,valB;
   int stateA=0;
   int stateB=0;
   int countA=0;
   int countB=0;
   int dA,dB,dA1,dB1,dA2,dB2;
   int s;//0なら無調整のまま、１なら左回転で調整済み、２なら右回転で調整済み
   int deg;
   int uA,uB;
   Serial.println("vert");
   
   //１と４の両方黒線にかかっていなければただまっすぐに進むだけ  
 while(analogRead(light1)<black&&analogRead(light4)<black34&&s==0){ //-5は調整
      analogRead(light1);
      analogRead(light4);
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
     if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

     
            uA = (dref-dA)*Kp1+Kp2*(dB-dA);
            uB = (dref-dB)*Kp1+Kp2*(dA-dB);
      
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));     
  }

  
  countA = 0 ;
  countB = 0 ;
  
 //１が先に黒線にかかったら右に曲がっている
if(analogRead(light1)>=black&&analogRead(light4)<black34&&s==0){
     valA=0;
     valB=0;
     stateA=0;
     stateB=0;
     countA=0;
     countB=0;
     s=1;
    while(analogRead(light4)>=black34){ //-5は調整
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
     if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

      dA1=dA;
      dB1=dB;//距離を保存しておく

        int uA = 50+Kp2*(dB-dA);
        int uB = 50+Kp2*(dA-dB);
     
      
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));     
 }
 }
 
 //4が先に黒線にかかったら左に曲がっている。
if(analogRead(light1)<black&&analogRead(light4)>=black34&&s==0){
     valA=0;
     valB=0;
     stateA=0;
     stateB=0;
     countA=0;
     countB=0;
    s=2;
    while(analogRead(light1)>=black){ //-5は調整
      valA = digitalRead(PHASE_A);
      valB = digitalRead(PHASE_B);
      edgeA=0;
      edgeB=0;
      if ((valA ==1) && (stateA == 0)) {
          stateA = 1; edgeA = 1; // rising edge
      }
      if ((valA==0) && (stateA == 1)) {
          stateA = 0; edgeA = -1; // falling edge
      }
      if ((valB ==1) && (stateB == 0)) {
          stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
          stateB = 0; edgeB = -1; // falling edge
      }

/* pulse & direction count */
     if (edgeA==1){
         countA++;
      }
      if(edgeA==-1){
         countA++;
      }
      if(edgeB==1){
         countB++;
      }
      if(edgeB==-1){
         countB++;
      }
      Serial.print("countA=");
      Serial.println(countA);
      Serial.print("countB=");
      Serial.println(countB);

      dA=tirer*(360*countA/cpr)*(PI/180);
      dB=tirer*(360*countB/cpr)*(PI/180);

      dA2=dA;
      dB2=dB;//距離を保存しておく

      int uA = 50+Kp2*(dB-dA);
      int uB = 50+Kp2*(dA-dB);
      
      if ((uA>0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } 
      if ((uA>0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uB=0;
      } 
      if ((uA<=0)&&(uB>0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
      } 
      if((uA<=0)&&(uB<=0)) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         uA=0;
         uB=0;
      }
      analogWrite(PWMA, min(uA+constant, Max));
      analogWrite(PWMB, min(uB+constant, Max));     
 }
 }


//s=1（右に曲がっている時の調整）
 if(s==1){
   edgeB=0;
   valB=0;
   stateB=0;
   countB=0;

   Serial.println("adjustment1");
   deg=atan2(dA1,lshaf)*(180/PI);
   while(lshaf*deg*(PI/180)+5>tirer*(360*countB/cpr)*(PI/180)){ //+5は調整
      valB = digitalRead(PHASE_B);
      edgeB=0;
      if ((valB ==1) && (stateB == 0)) {
         stateB = 1; edgeB = 1; // rising edge
      }
      if ((valB ==0) && (stateB == 1)) {
         stateB = 0; edgeB = -1; // falling edge
      }
      if(edgeB==1){
         countB ++;
      }
      if(edgeB==-1){
         countB ++;
      }
      Serial.print("countB=");
      Serial.println(countB);
      int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countB/cpr)*(PI/180));
 
      if (u>0) {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
      } else {
         digitalWrite(AIN1, LOW);
         digitalWrite(AIN2, HIGH);
         digitalWrite(BIN1, LOW);
         digitalWrite(BIN2, HIGH);
         u=0;
      }
      analogWrite(PWMA, 0 );
      analogWrite(PWMB, min(u+constant, Max2));
   }
 }
 
//s=2（左に曲がっている時の調整）
 if(s==2){
   edgeA=0;
   valA=0;
   stateA=0;
   countA=0;
   Serial.println("adjustment2");
   deg=atan2(dA1,lshaf)*(180/PI);
     while(lshaf*deg*(PI/180)+5>tirer*(360*countA/cpr)*(PI/180)){ //-5は調整
        valA = digitalRead(PHASE_A);
        edgeA=0;
     if ((valA ==1) && (stateA == 0)) {
        stateA = 1; edgeA = 1; // rising edge
     }
     if ((valA ==0) && (stateA == 1)) {
        stateA = 0; edgeA = -1; // falling edge
     }
     if(edgeA==1){
        countA ++;
        Serial.println(countA);
     }
     if(edgeA==-1){
        countA ++;
        Serial.println(countA);
     }
     int u=Kp3*(lshaf*deg*(PI/180)-tirer*(360*countA/cpr)*(PI/180));
     if (u>0) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     } else {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
     }
        analogWrite(PWMA, min(u+constant, Max2));
        analogWrite(PWMB, 0 );
     }
  }
}

// アームに関する関数
//アームを閉じる
void closearm(){
  int angle;
  while(angle>0){
    servo1.write(angle);
    angle=angle-dir;
    delay(100);
  }
  Serial.println("closearm");
}

//アームを開ける
void openarm(){
  int angle;
  while(angle<90){
    servo1.write(angle);
    angle=angle+dir;
    delay(100);
  }
  Serial.println("openarm");
}

//アームを上げる
void liftarm(int angleref){
  int angle;
  while(angleref>angle){
    servo2.write(angle);
    angle=angle+dir;
    delay(5);
  }
  Serial.println("liftarm");
}

//アームをおろす
void downarm(int angleref){
  int angle;
  while(angle>angleref){
    servo2.write(angle);
    angle=angle-dir;
    delay(5);
  }
  Serial.println("downarm");
}

//温度を測る
void temperature(){
  delay(5000);
  T = analogRead(therm);
  Serial.print("temprature=");
  Serial.println(T);
  
}