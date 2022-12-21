#include "serving.h"        // ヘッダファイルの読み込み
#include <Servo.h>          // サーボモータのライブラリ

//----------------------------------
// グローバル変数の宣言
//----------------------------------

Servo hunger; // 物体を把持するためのサーボモータの設定
Servo bringer; // 物体を持ち上げるためのサーボモータの設定


// ロータリーエンコーダのための定数
const int threshold_ON = 600;
const int threshold_OFF = 450;

// ロータリーエンコーダのフィードバック制御のための定数
const int cpr = 48; // count per rotation (1回転当たりのカウント数「スリット数×４」)
int angle_REF = 180; // 角度の目標値
const float Kp = 0.3 // 比例ゲイン

int i = 0; // ループの回数を記憶する定数

double destination[2]; // 機体から目的地までの距離と角度を格納する配列
Position robot_pos = {
    ,
}; // ロボットの座標を格納する配列
boolean table_hot = false; // あったかいテーブルにドリンクがあるかを判別する定数
boolean table_cool = false; // 冷たいテーブルにドリンクがあるかを判別する定数
boolean sign = true; // モーターの回転方向を示す。trueで前進
boolean tableLR[2] = {
    false,
    false
}; // 左右のテーブルにドリンクをおいてるかの情報を格納する配列



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
Object[4].position.x = 100; // テーブル
Object[4].position.y = 1000;
Object[5].position.x = 700; // テーブル
Object[5].position.y = 1000;
Object[6].position.y = 1000; // カウンターのy座業
//----------------------------------
// 初期化に関する設定
//----------------------------------

void setup() {
    hunger.attach(3); // サーボモータのピンは3と5
    bringer.attach(5);
    hunger.write(90); // サーボモータの初期角度の設定
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
    delay(500); // サーボモータを適切に回転させるためのdelay
}

//----------------------------------
// メイン処理
//----------------------------------

void loop() {
    switch (task_state) {
        case START:
            task_state = APPROACH;
            break;

        case APPROACH:
            main_move(Object, i, robot_pos, destination);
            taskestate = PICKUP;
            break;

        case PICKUP:
            pick();
            measure_light(Object, i);
            if (Object[i].light > 800) { // 白いほう
                if (table_cool == false) { // テーブルが空のとき、テーブルに送る
                    taskstate = TABLE_WHITE;
                } else {
                    taskstate = COUNTER; // テーブルに既にドリンクを置いてるとき、カウンターに送る
                }
            } else {
                if (table_hot == false) {
                    taskstate = TABLE_BLACK;
                } else {
                    taskstate = COUNTER;
                }
            }
            break;

        case TABLE_WHITE:
            if (tableLR[0] == true) {
                main_move(Object, TABLE_R, robot_pos, destination);
                table_cool = true;
                tableLR[1] = true;
                taskstate = PLACE;
                break;
            } else if (tableLR[1] == true) {
                main_move(Object, TABLE_L, robot_pos, destination);
                table_cool = true;
                tabelLR[0] = true;
                taskstate = PLACE;
                break;
            } else {
                main_move(Object, TABLE_R, robot_pos, destination);
                measure_thermo(Object, i);
                if (Object[i].tempreture > 40) {
                    main_move(Object, TABLE_L, robot_pos, destination);
                    tableLR[0] = true;
                    table_cool = true;
                    task_state = PLACE;
                } else {
                    table_cool = true;
                    tableLR[1] = true;
                    task_state = PLACE;
                }
                break;
            }
            break;

        case TABLE_BLACK:
            if (tableLR[0] == true) {
                main_move(Object, TABLE_R, robot_pos, destination);
                table_hot = true;
                tableLR[1] = true;
                taskstate = PLACE;
                break;
            } else if (tableLR[1] == true) {
                main_move(Object, TABLE_L, robot_pos, destination);
                table_hot = true;
                tabelLR[0] = true;
                taskstate = PLACE;
                break;
            } else {
                main_move(Object, TABLE_R, robot_pos, destination);
                measure_thermo(Object, i);
                if (Object[i].tempreture < 40) {
                    main_move(Object, TABLE_L, robot_pos, destination);
                    tableLR[0] = true;
                    table_hot = true;
                    task_state = PLACE;
                } else {
                    table_hot = true;
                    tableLR[1] = true;
                    task_state = PLACE;
                }
                break;
            }
            break;


        case COUNTER:
            Object[6].position.x = robot_pos.x;
            main_move(Object, 6, robot_pos, destination);
            taskstate = PLACE;
            break;

        case PLACE:
            divide();
            if (i == OBJECT_NUM - 1) {
                taskstate = FINISH;
            } else {
                taskstate = APPROACH;
                i++;
            }
            break;

        case FINISH:
            digitalWrite(LED, HIGH);
            break;
    }
}