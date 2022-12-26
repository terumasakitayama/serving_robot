#include <Servo.h>          // サーボモータのライブラリ
#include <EEPROM.h>

//----------------------------------
// マクロ定義
//----------------------------------

const int LED = 4;
const int AIN1 = 9;
const int AIN2 = 10;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMA = 11;
const int PWMB = 6;
const int PHASE_A = A0; // ロータリーエンコーダ
const int PHASE_B = A1;
const int THERMO = A2;
const int LIGHT = A3;
const double ROBOT_LENGTH = 50.0;       // ロボットの長さ
const double RADIUS = 17.0;         // 車輪の半径
const int OBJECT_NUM = 4;       // ドリンクの数
const int TABLE_L = 4;          // 左側のテーブル
const int TABLE_R = 5;          // 右側のテーブル
#define PI 3.14159265358979

// ロータリーエンコーダのための定数
const int threshold_ON = 600;
const int threshold_OFF = 450;
const int cpr = 48; // count per rotation (1回転当たりのカウント数「スリット数×４」)
const float Kp = 0.3; // 比例ゲイン

Servo hunger; // 物体を把持するためのサーボモータの設定
Servo bringer; // 物体を持ち上げるためのサーボモータの設定

//----------------------------------
// Objectの構造体の定義
//----------------------------------


typedef struct Position // πを使うためdouble型にしている
{
    double x;
    double y;
}POSITION;


typedef struct {
    int index;
    POSITION position;
    int tempreture;
    int light;
}Object;

typedef enum {
    START,
    APPROACH,
    PICKUP,
    TABLE_WHITE,
    TABLE_BLACK,
    COUNTER,
    PLACE,
    FINISH
}
TaskState;
TaskState task_state = START;

//----------------------------------
// 関数のプロトタイプ宣言
//----------------------------------

void straight(float angle_REF); // まっすぐ進むための関数(main_moveで使用)
void rotate(float angle_REF); // 回転するための関数(main_moveで使用)
void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos); // 目的地までの距離と角度を測定する関数(main_moveで使用)
void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination); // 目的地へ動く関数
void pick(); // 物体を把持する関数
void divide(); // 物体を設置する関数
void measure_thermo(Object *p_object, int i); // 温度を測る関数
void measure_light(Object *p_object, int i); // ドリンクの色を測る関数


//----------------------------------
// グローバル変数の宣言
//----------------------------------

int g_i = 0; // ループの回数を記憶する定数

double g_destination[2]; // 機体から目的地までの距離と角度を格納する配列
POSITION g_robot_pos; // ロボットの座標を格納する配列
boolean table_hot = false; // あったかいテーブルにドリンクがあるかを判別する定数
boolean table_cool = false; // 冷たいテーブルにドリンクがあるかを判別する定数
boolean sign = true; // モーターの回転方向を示す。trueで前進
boolean tableLR[2] = {false, false}; // 左右のテーブルにドリンクをおいてるかの情報を格納する配列
Object g_object[7];       // 各座標や温度などを格納する構造体の定義



//----------------------------------
// 初期化に関する設定
//----------------------------------

void setup() {
    hunger.attach(3); // サーボモータのピンは3と5
    bringer.attach(5);
    hunger.write(90); // サーボモータの初期角度の設定
    bringer.write(90);

    //----------------------------------
    // ドリンクやテーブルなどの座標の格納
    //----------------------------------


    g_object[0].position = {300, 500};
    g_object[1].position = {500, 500};
    g_object[2].position = {200, 700};
    g_object[3].position = {600, 700};
    g_object[4].position = {100, 1000}; // テーブル
    g_object[5].position = {700, 1000}; // テーブル
    g_object[6].position.y = 1000; // カウンターのy座業


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
            main_move(g_object, g_i, &g_robot_pos, g_destination);
            task_state = PICKUP;
            break;

        case PICKUP:
            pick();
            measure_light(g_object, g_i);
            if (g_object[g_i].light > 800) { // 白いほう
                if (table_cool == false) { // テーブルが空のとき、テーブルに送る
                    task_state = TABLE_WHITE;
                } else {
                    task_state = COUNTER; // テーブルに既にドリンクを置いてるとき、カウンターに送る
                }
            } else {
                if (table_hot == false) {
                    task_state = TABLE_BLACK;
                } else {
                    task_state = COUNTER;
                }
            }
            break;

        case TABLE_WHITE:
            if (tableLR[0] == true) {
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                table_cool = true;
                tableLR[1] = true;
                task_state = PLACE;
                break;
            } else if (tableLR[1] == true) {
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                table_cool = true;
                tableLR[0] = true;
                task_state = PLACE;
                break;
            } else {
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, g_i);
                if (g_object[g_i].tempreture > 40) {
                    main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
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
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                table_hot = true;
                tableLR[1] = true;
                task_state = PLACE;
                break;
            } else if (tableLR[1] == true) {
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                table_hot = true;
                tableLR[0] = true;
                task_state = PLACE;
                break;
            } else {
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, g_i);
                if (g_object[g_i].tempreture < 40) {
                    main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
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
            g_object[5].position.x = g_robot_pos.x;
            main_move(g_object, 5, &g_robot_pos, g_destination);
            task_state = PLACE;
            break;

        case PLACE:
            divide();
            if (g_i == OBJECT_NUM - 1) {
                task_state = FINISH;
            } else {
                task_state = APPROACH;
                g_i++;
            }
            break;

        case FINISH:
            digitalWrite(LED, HIGH);
            break;
    }
}


//----------------------------------
// 関数定義
//----------------------------------

void straight(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    int state = 0;
    int edge = 0;
    while (1) {
        valA = analogRead(PHASE_A);
        edge = 0;
        if ((valA > 550) && (state == 0)) {
            state = 1;
            edge = 1;
        }
        if ((valA < 450) && (state == 1)) {
            state = 0;
            edge = -1;
        }
        if (edge == -1) {
            count++;
        }
        if (edge == 1) {
            count++;
        }

        int angle = 360 * count / cpr;
        int u = (angle_REF - angle) * Kp;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, min(abs(u) + 10, 200));
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, min(abs(u) + 10, 200));
        if (abs(u) < 2) {
            break;
        }
    }
}

void rotate(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    int state = 0;
    int edge = 0;
    while (1) {
        valA = analogRead(PHASE_A);
        edge = 0;
        if ((valA > 550) && (state == 0)) {
            state = 1;
            edge = 1;
        }
        if ((valA < 450) && (state == 1)) {
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

        int angle = 360 * count / cpr;
        int u = (angle_REF - angle) * Kp;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, min(abs(u) + 10, 200));
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, min(abs(u) + 10, 200));
        if (abs(u) < 2) {
            break;
        }
    }
}

void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination) {
    distance(object, i, p_destination, p_robot_pos);
    rotate(p_destination[0]);
    straight(p_destination[1]);
    p_robot_pos->x = object[i].position.x - (ROBOT_LENGTH * sin(p_destination[0]));
    p_robot_pos->y = object[i].position.y - (ROBOT_LENGTH * cos(p_destination[0]));
}

void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos) {
    double X = p_robot_pos->x - object[i].position.x;
    double Y = p_robot_pos->y - object[i].position.y;
    double angle = 90 - atan(abs(Y) / abs(X)) / PI * 180;
    double dis = sqrt(pow(X, 2) + pow(Y, 2)) / (2 * PI * RADIUS) * 360;
    p_destination[0] = angle;
    p_destination[1] = dis;
}

void pick() {
    bringer.write(90);
    hunger.write(90);
    bringer.write(90);
}

void divide() {
    bringer.write(90);
    hunger.write(90);
    bringer.write(90);
}

void measure_thermo(Object *p_object, int i) {
    delay(500);
    p_object[i].tempreture = analogRead(THERMO);
}

void measure_light(Object *p_object, int i) {
    p_object[i].light = analogRead(LIGHT);
}