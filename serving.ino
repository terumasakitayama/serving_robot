#include "serving.h"        // ヘッダファイルの読み込み
#include <Servo.h>          // サーボモータのライブラリ
#define OBJECT_NUM 4        // ドリンクの数


//----------------------------------
// グローバル変数の宣言
//----------------------------------

Servo hunger;       // 物体を把持するためのサーボモータの設定
Servo bringer;      // 物体を持ち上げるためのサーボモータの設定


// ロータリーエンコーダのための定数
const int threshold_ON = 600;
const int threshold_OFF = 450;

// ロータリーエンコーダのフィードバック制御のための定数
const int cpr = 48;  // count per rotation (1回転当たりのカウント数「スリット数×４」)
int angle_REF = 180; // 角度の目標値
const float Kp = 0.3 // 比例ゲイン

int i = 0; // ループの回数を記憶する定数

double destination[2];          // 機体から目的地までの距離と角度を格納する配列
Position robot_pos ={, };       // ロボットの座標を格納する配列
boolean table_hot = false;         // あったかいテーブルにドリンクがあるかを判別する定数
boolean table_cool = false;        // 冷たいテーブルにドリンクがあるかを判別する定数
boolean sign = true;               // モーターの回転方向を示す。trueで前進



//----------------------------------
// ドリンクやテーブルなどの座標の格納
//----------------------------------

Object[0].position.x = 300;
Object[0].position.y = 500;
Object[1].position.x = 500;
Object[1].position.y = 500;
Object[2].position.x = 200;
Object[2].position.y = 700;
Object[3].position.x = 600;
Object[3].position.y = 700;
Object[4].position.x = 100;
Object[4].position.y = 1000;
Object[5].position.x = 700;
Object[5].position.y = 1000;
Object[6].position.x = ;
Object[6].position.y = ;

//----------------------------------
// 初期化に関する設定
//----------------------------------

void setup()
{
    hunger.attach(3);       // サーボモータのピンは3と5
    bringer.attach(5);
    hunger.write(90);       // サーボモータの初期角度の設定
    bringer.write(90);


    // pinModeの設定
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);

    // モーターの初期設定
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);

    Serial.begin(115200);
    delay(500);         // サーボモータを適切に回転させるためのdelay
}

//----------------------------------
// メイン処理
//----------------------------------

void loop()
{
    switch (task_state)
    {
    case START:
        task_state = APPROACH;
        break;

    case APPROACH:
        move(object, i, robot_pos, destination);
        taskestate = PICKUP;
        break;

    case PICKUP:
        pick();
        measure_light(object, i);
        if(object[i].light > 800){       // 白いほう
            if(table_cool == false){        // テーブルが空のとき、テーブルに送る
                taskstate = TABLE;
            }else{
                taskstate = COUNTER;        // テーブルに既にドリンクを置いてるとき、カウンターに送る
            }
        }else{
            if(table_hot == false){
                taskstate = TABLE;
            }else{
                taskstate = COUNTER;
            }
        }
        break;

    case TABLE_COLD:
        move()
        measure_thermo(object, i);
        if(object[i].tempreture > 40){

        }

    case TABLE_HOT:


    case COUNTER:
        move(Object *object, int i, Position *robot_pos, double *destination)
        taskstate = PLACE;
        break;

    case PLACE:
        divide();
        measure_thermo(object, i);
        if()
        if(i == OBJECT_NUM - 1){
            taskstate = FINISH;
        }else{
            taskstate = APPROACH;
            i++;
        }
        break;

    case FINISH:
        digitalWrite(LED, HIGH);
        break;
  }
}



