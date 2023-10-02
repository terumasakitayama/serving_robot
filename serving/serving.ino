#include <Servo.h>          // サーボモータのライブラリ
#include <EEPROM.h>

//----------------------------------------------
// マクロ定義
//----------------------------------------------
const int LED = 4;
const int AIN1 = 9;
const int AIN2 = 10;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMA = 11;
const int PWMB = 6;
const int PHASE_A = A0;                         // ロータリーエンコーダ
const int PHASE_B = A1;
const int THERMO = A2;                          // 温度センサ
const int LIGHT = A3;                           // 光センサ
const double ROBOT_LENGTH = 50.0;               // ロボットの長さ
const double RADIUS = 17.0;                     // 車輪の半径
const int OBJECT_NUM = 4;                       // ドリンクの数
const int TABLE_L = 4;                          // 左側のテーブル
const int TABLE_R = 5;                          // 右側のテーブル
#define PI 3.14159265358979

// ロータリーエンコーダのための定数
const int threshold_ON = 600;                   // ON閾値
const int threshold_OFF = 450;                  // OFF閾値
const int cpr = 48;                             // count per rotation (1回転当たりのカウント数「スリット数×４」)
const float Kp = 0.3;                           // 比例ゲイン

Servo hunger;                                   // 物体を把持するためのサーボモータの設定
Servo bringer;                                  // 物体を持ち上げるためのサーボモータの設定

//----------------------------------------------
// Objectの構造体の定義
//----------------------------------------------
// POSITION（構造体）
typedef struct Position {                       // πを使うためdouble型にしている
    double x;                                   // x座標
    double y;                                   // y座標
}POSITION;

// 各座標や温度などを格納する構造体
typedef struct {
    int index;                                  // インデックス
    POSITION position;                          // 位置（xy座標）
    int tempreture;                             // 温度
    int light;                                  // ドリンクの色の測定値
}Object;

// タスクステータス一覧（列挙型）
typedef enum {
    START,                                      // 開始
    APPROACH,                                   // アプローチ
    PICKUP,                                     // ピックアップ
    TABLE_WHITE,                                // 白いテーブル
    TABLE_BLACK,                                // 黒いテーブル
    COUNTER,                                    // カウンタ
    PLACE,                                      // プレース
    FINISH                                      // 終了
}TaskState;

TaskState task_state = START;                   // タスクステータスを開始状態にする（初期化）

//--------------------------------------------------
// 関数のプロトタイプ宣言
//--------------------------------------------------
void straight(float angle_REF);                 // まっすぐ進むための関数(main_moveで使用)
void rotate(float angle_REF);                   // 回転するための関数(main_moveで使用)
                                                // 目的地までの距離と角度を測定する関数(main_moveで使用)
void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos);
                                                // 目的地へ動く関数
void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination);
void pick();                                    // 物体を把持する関数
void divide();                                  // 物体を設置する関数
void measure_thermo(Object *p_object, int i);   // 温度を測る関数
void measure_light(Object *p_object, int i);    // ドリンクの色を測る関数

//----------------------------------------------
// グローバル変数の宣言
//----------------------------------------------
int g_i = 0;                                    // 配膳回数を記憶する定数
double g_destination[2];                        // 機体から目的地までの距離と角度を格納する配列
POSITION g_robot_pos;                           // ロボットの座標を格納する配列
boolean table_hot = false;                      // あったかいテーブルにドリンクがあるかを判別する定数
boolean table_cool = false;                     // 冷たいテーブルにドリンクがあるかを判別する定数
boolean sign = true;                            // モーターの回転方向を示す。trueで前進
boolean tableLR[2] = {false, false};            // 左右側のテーブルにドリンクをおいてるかの情報を格納する配列
Object g_object[7];                             // 各座標や温度などを格納する構造体の定義

//----------------------------------------------
// 初期化に関する設定
//----------------------------------------------
void setup() {
    hunger.attach(3);                           // サーボモータのピンは3と5
    bringer.attach(5);
    hunger.write(90);                           // サーボモータの初期角度の設定
    bringer.write(90);

    //------------------------------------------
    // ドリンクやテーブルなどの座標の格納
    //------------------------------------------
    g_object[0].position = {300, 500};          // 一番目のドリンクのxy座標
    g_object[1].position = {500, 500};          // 二番目のドリンクのxy座標
    g_object[2].position = {200, 700};          // 三番目のドリンクのxy座標
    g_object[3].position = {600, 700};          // 四番目のドリンクのxy座標
    g_object[4].position = {100, 1000};         // 左側のテーブルのxy座標
    g_object[5].position = {700, 1000};         // 右側のテーブルのxy座標
    g_object[6].position.y = 1000;              // カウンタのy座標

    //------------------------------------------
    // pinModeの設定
    //------------------------------------------
    pinMode(AIN1, OUTPUT);                      // デジタルピン AIN1を出力に設定
    pinMode(AIN2, OUTPUT);                      // デジタルピン AIN2を出力に設定
    pinMode(BIN1, OUTPUT);                      // デジタルピン BIN1を出力に設定
    pinMode(BIN2, OUTPUT);                      // デジタルピン BIN2を出力に設定
    pinMode(PWMA, OUTPUT);                      // デジタルピン PWMAを出力に設定
    pinMode(PWMB, OUTPUT);                      // デジタルピン PWMBを出力に設定

    //------------------------------------------
    // モーターの初期設定
    //------------------------------------------
    digitalWrite(AIN1, LOW);                    // sets the digital pin AIN1 off
    digitalWrite(AIN2, LOW);                    // sets the digital pin AIN2 off
    digitalWrite(BIN1, LOW);                    // sets the digital pin BIN1 off
    digitalWrite(BIN2, LOW);                    // sets the digital pin BIN2 off

    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);

    Serial.begin(115200);                       // 115200 bpsでポートを開く
    delay(500);                                 // サーボモータを適切に回転させるための delay(500ミリ秒)
}

//----------------------------------------------
// メイン処理
//----------------------------------------------
void loop() {
    switch (task_state) {
        case START:                             // 開始
            task_state = APPROACH;              // タスクステータスをアプローチ状態にする
            break;

        case APPROACH:                          // アプローチ
                                                // ドリンク（目的地）の位置へ行く
            main_move(g_object, g_i, &g_robot_pos, g_destination);
            task_state = PICKUP;                // タスクステータスをピックアップ状態に変更する
            break;

        case PICKUP:                            // ピックアップ
            pick();                             // 物体を把持する
            measure_light(g_object, g_i);       // ドリンクの色を測る
            if (g_object[g_i].light > 800) {    // ドリンクの色が白いほうだったら（８００より大きい場合）
                if (table_cool == false) {      // 冷たいテーブルが空のとき、白いテーブルに送る
                    task_state = TABLE_WHITE;   // タスクステータスを白いテーブル状態に変更する
                } else {                        // 冷たいテーブルに既にドリンクを置いてるとき、カウンタに送る
                    task_state = COUNTER;       // タスクステータスをカウンタ状態に変更する
                }
            } else {                            // ドリンクの色が白いほう以外だったら（８００以下の場合）
                if (table_hot == false) {       // あったかいテーブルが空のとき、黒いテーブルに送る
                    task_state = TABLE_BLACK;   // タスクステータスを黒いテーブル状態に変更する
                } else {                        // 黒いテーブルに既にドリンクを置いてるとき、カウンタに送る
                    task_state = COUNTER;       // タスクステータスをカウンタ状態に変更する
                }
            }
            break;

        case TABLE_WHITE:                       // 白いテーブル
            if (tableLR[0] == true) {           // 左側のテーブルにドリンクを置いてある場合
                                                // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                table_cool = true;              // 冷たいテーブルにドリンクがある状態に設定する
                tableLR[1] = true;              // 右側のテーブルにドリンクを置く
                task_state = PLACE;             // タスクステータスをプレース状態に変更する
                break;
            } else if (tableLR[1] == true) {    // 右側のテーブルにドリンクを置いてある場合
                                                // 左側のテーブルへ移動する.
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                table_cool = true;              // 冷たいテーブルにドリンクがある状態に設定する
                tableLR[0] = true;              // 左側のテーブルにドリンクを置く
                task_state = PLACE;             // タスクステータスをプレース状態に変更する
                break;
            } else {                            // 左右側のテーブルにドリンクをある、または左右両方にない場合
                                                // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, g_i);
                if (g_object[g_i].tempreture > 40) {
                    // 温度が40度より高かったら
                                                // 左側のテーブルへ移動する.
                    main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                    table_cool = true;          // 冷たいテーブルにドリンクがある状態に設定する
                    tableLR[0] = true;          // 左側のテーブルにドリンクを置く
                    task_state = PLACE;         // タスクステータスをプレース状態に変更する
                } else {                        // 温度が40度以下だったら
                    table_cool = true;          // 冷たいテーブルにドリンクがある状態に設定する
                    tableLR[1] = true;          // 右側のテーブルにドリンクを置く
                    task_state = PLACE;         // タスクステータスをプレース状態に変更する
                }
                break;
            }
            break;

        case TABLE_BLACK:                       // 黒いテーブル
            if (tableLR[0] == true) {           // 左側のテーブルにドリンクをおいてるある場合
                                                // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                table_hot = true;               // あったかいテーブルにドリンクがある状態に設定する
                tableLR[1] = true;              // 右側のテーブルにドリンクを置く
                task_state = PLACE;             // タスクステータスをプレース状態に変更する
                break;
            } else if (tableLR[1] == true) {    // 右側のテーブルにドリンクをおいてるある場合
                                                // 左側のテーブルへ移動する.
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                table_hot = true;               // あったかいテーブルにドリンクがある状態に設定するに設定する
                tableLR[0] = true;              // 左側のテーブルにドリンクを置く
                task_state = PLACE;             // タスクステータスをプレース状態に変更する
                break;
            } else {                            // 左右側のテーブルにドリンクをある、または左右両方にない場合
                                                // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, g_i);  // 温度を測る
                if (g_object[g_i].tempreture < 40) {
                    // 温度が40度より低かったら
                                                // 左側のテーブルへ移動する.
                    main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                    table_hot = true;           // あったかいテーブルにドリンクがある状態に設定する
                    tableLR[0] = true;          // 左側のテーブルにドリンクを置く
                    task_state = PLACE;         // タスクステータスをプレース状態に変更する
                } else {                        // 温度が40度以上だったら
                    table_hot = true;           // あったかいテーブルにドリンクがある状態に設定する
                    tableLR[1] = true;          // 右側のテーブルにドリンクを置く
                    task_state = PLACE;         // タスクステータスをプレース状態に変更する
                }
                break;
            }
            break;

        case COUNTER:                           // カウンタ
            g_object[6].position.x = g_robot_pos.x;
                                                // カウンタへ移動する.
            main_move(g_object, 6, &g_robot_pos, g_destination);
            task_state = PLACE;                 // タスクステータスをプレース状態に変更する
            break;

        case PLACE:                             // プレース
            divide();                           // 物体を設置する
            if (g_i == OBJECT_NUM - 1) {        // ループの回数がドリンク数-1に達した場合
                task_state = FINISH;            // タスクステータスを終了状態に変更する
            } else {                            // ループの回数がドリンク数-1に達していない場合
                task_state = APPROACH;          // タスクステータスをアプローチ状態に変更する
                g_i++;                          // ループの回数カウントアップする.
            }
            break;

        case FINISH:                            // 終了
            digitalWrite(LED, HIGH);            // sets the digital pin LED = 4 on
            break;
    }
}

//----------------------------------------------
// 関数定義
//----------------------------------------------
/**
 * まっすぐ進むための関数(main_moveで使用)
 * @param angle_REF
 */
void straight(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    int state = 0;
    int edge = 0;
    while (1) {
        valA = analogRead(PHASE_A);             // エッジの検出
        edge = 0;
        if ((valA > 550) && (state == 0)) {     // rising edge
            state = 1;
            edge = 1;
        }
        if ((valA < 450) && (state == 1)) {     // falling edge
            state = 0;
            edge = -1;
        }
        if (edge == -1) {
            count++;
        }
        if (edge == 1) {
            count++;
        }

        int angle = 360 * count / cpr;          // カウント値を角度に戻す
        int u = (angle_REF - angle) * Kp;       // フィードバック制御
        digitalWrite(AIN1, LOW);                // sets the digital pin AIN1 off
        digitalWrite(AIN2, HIGH);               // sets the digital pin AIN2 on
        analogWrite(PWMA, min(abs(u) + 10, 200));
        digitalWrite(BIN1, LOW);                // sets the digital pin BIN1 off
        digitalWrite(BIN2, HIGH);               // sets the digital pin BIN2 on
        analogWrite(PWMB, min(abs(u) + 10, 200));
        if (abs(u) < 2) {
            break;
        }
    }
}

/**
 * 回転するための関数(main_moveで使用)
 * @param angle_REF
 */
void rotate(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    int state = 0;
    int edge = 0;
    while (1) {
        valA = analogRead(PHASE_A);             // エッジの検出
        edge = 0;
        if ((valA > 550) && (state == 0)) {     // rasing edge
            state = 1;
            edge = 1;
        }
        if ((valA < 450) && (state == 1)) {     // falling edge
            state = 0;
            edge = -1;
        }
        if (edge == -1) {
            count++;
        }
        if (edge == 1) {
            count++;
        }
        Serial.println(count);

        int angle = 360 * count / cpr;          // カウント値を角度に戻す
        int u = (angle_REF - angle) * Kp;       // フィードバック制御
        digitalWrite(AIN1, LOW);                // sets the digital pin AIN1 off
        digitalWrite(AIN2, HIGH);               // sets the digital pin AIN2 on
        analogWrite(PWMA, min(abs(u) + 10, 200));
        digitalWrite(BIN1, HIGH);               // sets the digital pin BIN1 on
        digitalWrite(BIN2, LOW);                // sets the digital pin BIN2 off
        analogWrite(PWMB, min(abs(u) + 10, 200));
        if (abs(u) < 2) {
            break;
        }
    }
}

/**
 * 目的地へ動く関数
 * @param object        オブジェクトのインスタンス
 * @param i             インデックス
 * @param p_robot_pos   ロボットの座標
 * @param p_destination 機体から目的地までの距離と角度
 */
void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination) {
    distance(object, i, p_destination, p_robot_pos);
    rotate(p_destination[0]);
    straight(p_destination[1]);
    p_robot_pos->x = object[i].position.x - (ROBOT_LENGTH * sin(p_destination[0]));
    p_robot_pos->y = object[i].position.y - (ROBOT_LENGTH * cos(p_destination[0]));
}

/**
 * 目的地までの距離と角度を測定する関数(main_moveで使用)
 * @param object        オブジェクトのインスタンス
 * @param i             インデックス
 * @param p_robot_pos   ロボットの座標
 * @param p_destination 機体から目的地までの距離と角度
 */
void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos) {
    double X = p_robot_pos->x - object[i].position.x;
    double Y = p_robot_pos->y - object[i].position.y;
    double angle = 90 - atan(abs(Y) / abs(X)) / PI * 180;
    double dis = sqrt(pow(X, 2) + pow(Y, 2)) / (2 * PI * RADIUS) * 360;
    p_destination[0] = angle;
    p_destination[1] = dis;
}

/**
 * 物体を把持する関数
 */
void pick() {
    bringer.write(90);
    hunger.write(90);
    bringer.write(90);
}

/**
 * 物体を設置する関数
 */
void divide() {
    bringer.write(90);
    hunger.write(90);
    bringer.write(90);
}

/**
 * 温度を測る関数
 * @param p_object
 * @param i
 */
void measure_thermo(Object *p_object, int i) {
    delay(500);
    p_object[i].tempreture = analogRead(THERMO);// 温度センサで温度を測定
}
/**
 * ドリンクの色を測る関数
 * @param p_object  オブジェクトのインスタンス
 * @param i         インデックス
 */
void measure_light(Object *p_object, int i) {
    p_object[i].light = analogRead(LIGHT);      // ドリンクの色を測定
}
