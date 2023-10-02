#include <Servo.h>                                      // サーボモータのライブラリ

//------------------------------------------------------
// マクロ定義
//------------------------------------------------------
const int PHASE_A = A1;                                 // Encoder phase Aロータリーエンコーダ
const int PHASE_B = A2;                                 // Encoder phase B
const int AIN1 = 9;
const int AIN2 = 10;
const int PWMA = 11;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMB = 6;
const int THERMO = A0;                                  // 温度センサ
const double ROBOT_LENGTH = 50.0;                       // ロボットの長さ
const double RADIUS = 17.0;                             // 車輪の半径
const int TABLE_L = 4;                                  // 左側のテーブル
const int TABLE_R = 5;                                  // 右側のテーブル
const int COUNTER = 6;                                  // カウンタ
#define PI 3.14159265358979

// ロータリーエンコーダのための定数
const int threshold_ON = 600;                           // ON閾値
const int threshold_OFF = 450;                          // OFF閾値
const int cpr = 24;                                     // count per rotation (1回転当たりのカウント数「スリット数×2」)
const float Kp = 0.3;                                   // 比例ゲイン

Servo hunger;                                           // 物体を把持するためのサーボモータの設定
Servo bringer;                                          // 物体を持ち上げるためのサーボモータの設定

//------------------------------------------------------
// Objectの構造体の定義
//------------------------------------------------------
// POSITION（構造体）
typedef struct Position {                               // πを使うためdouble型にしている
    double x;                                           // x座標
    double y;                                           // y座標
}POSITION;

// 各座標や温度などを格納する構造体
typedef struct {
    int index;                                          // インデックス
    POSITION position;                                  // 位置（xy座標）
    boolean measured;                                   // 温度測定済みフラグ
    int tempreture;                                     // 温度
}Object;

//------------------------------------------------------
// 関数のプロトタイプ宣言
//------------------------------------------------------
void straight(float angle_REF);                         // まっすぐ進むための関数(main_moveで使用)
void rotate(float angle_REF);                           // 回転するための関数(main_moveで使用)
                                                        // 目的地までの距離と角度を測定する関数(main_moveで使用)
void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos);
void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination);
void pick();                                            // 物体を把持する関数
void divide();                                          // 物体を設置する関数
void measure_thermo(Object *p_object, int i);           // 温度を測る関数

//------------------------------------------------------
// グローバル変数の宣言
//------------------------------------------------------
int g_i = 0;                                            // ドリンク運搬回数を記憶する定数
double g_destination[2];                                // 機体から目的地までの距離と角度を格納する配列
POSITION g_robot_pos;                                   // ロボットの座標を格納する配列
boolean sign = true;                                    // モーターの回転方向を示す。trueで前進
boolean tableLR[2] = {                                  // 左右のテーブルにドリンクをおいてるかの情報を格納する配列
    false,                                              // 左側のテーブルにドリンクが置かれていない状態
    false                                               // 右側のテーブルにドリンクが置かれていない状態
};

Object g_object[8];                                     // 各座標や温度などを格納する構造体の定義
int angle = 0;

//------------------------------------------------------
// 初期化に関する設定
//------------------------------------------------------
void setup() {
    hunger.attach(2);                                   // サーボモータのピンは2と12
    bringer.attach(12);
    hunger.write(90);                                   // サーボモータの初期角度の設定
    bringer.write(90);

    //--------------------------------------------------
    // ドリンクやテーブルなどの座標の格納
    //--------------------------------------------------

    /*
    ドリンクは下記の 0～3座標レイアウトで配置されている.
        1(白)            2(黒)
            0(黒)   3(白)
    */

    g_object[0].position = {
        300,
        500
    };
    g_object[3].position = {
        500,
        500
    };
    g_object[1].position = {
        200,
        700
    };
    g_object[2].position = {
        600,
        700
    };
    g_object[4].position = {
        100,
        1000
    }; // テーブル
    g_object[5].position = {
        700,
        1000
    }; // テーブル
    g_object[6].position.y = 1000;                      // カウンターのy座標
    g_object[7].position = {
        300,
        900
    };

    g_object[TABLE_L].measured = false;                 // 左側のテーブルの温度未測定
    g_object[TABLE_R].measured = false;                 // 右側のテーブルの温度未測定

    //--------------------------------------------------
    // pinModeの設定
    //--------------------------------------------------
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);

    //--------------------------------------------------
    // モーターの初期設定
    //--------------------------------------------------
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
    pinMode(PHASE_A, INPUT);
    pinMode(PHASE_B, INPUT);
    hunger.write(90);
    bringer.write(90);

    Serial.begin(115200);                               // 115200 bpsでポートを開く
    delay(1000);                                        // サーボモータを適切に回転させるためのdelay
}

//------------------------------------------------------
// メイン処理
//------------------------------------------------------
void loop() {
    switch (g_i) {
        case 0:                                         // 黒いドリンク（目的地=0）の位置の運搬
                                                        // ドリンク（目的地=0）の位置へ行く
            main_move(g_object, g_i, &g_robot_pos, g_destination);
            pick();                                     // 物体を把持する
            main_move(g_object, 7, &g_robot_pos, g_destination);
            //------------------------------------------
            // 左側のテーブルに移動して温度を測る
            //------------------------------------------
                                                        // 左側のテーブルへ移動する.
            main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
            measure_thermo(g_object, TABLE_L);          // 左側のテーブルの温度を測る
            g_object[TABLE_L].measured = true;          // 左側のテーブルの温度測定済みにする.

            if (tableLR[0] == false &&                  // 左側のテーブルにドリンクが置かれていなくてかつ
                g_object[TABLE_L].tempreture > 40) {    // 左側のテーブルの温度が40度より高い場合
                tableLR[0] = true;
            } else {
                //--------------------------------------
                // 右側のテーブルに移動して温度を測る
                //--------------------------------------
                                                        // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, TABLE_R);      // 右側のテーブルの温度を測る
                g_object[TABLE_R].measured = true;      // 右側のテーブルの温度測定済みにする.

                if (tableLR[1] == false &&              // 右側のテーブルにドリンクが置かれていなくてかつ
                    g_object[TABLE_R].tempreture > 40) {// 右側のテーブルの温度が40度より高い場合
                    tableLR[1] = true;
                } else {                                // 上記以外の条件の場合
                                                        // カウンタへ移動する.
                    g_object[COUNTER].position.x = g_robot_pos.x;
                    main_move(g_object, COUNTER, &g_robot_pos, g_destination);
                }
            }
            divide();                                   // 物体を設置する
            if (g_object[TABLE_R].measured == false){   // 右側のテーブルの温度未測定の場合.
                //--------------------------------------
                // 右側のテーブルに移動して温度を測る
                //--------------------------------------
                                                        // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                measure_thermo(g_object, TABLE_R);      // 右側のテーブルの温度を測る
                g_object[TABLE_R].measured = true;      // 右側のテーブルの温度測定済みにする.
            }
            g_i++;
            break;

        case 1:                                         // 白いドリンク（目的地=1）の位置の運搬
        case 3:                                         // 白いドリンク（目的地=3）の位置の運搬
                                                        // ドリンク（目的地=1,3）の位置へ行く
            main_move(g_object, g_i, &g_robot_pos, g_destination);
            pick();                                     // 物体を把持する
            if (tableLR[0] == false &&                  // 左側のテーブルにドリンクが置かれていなくてかつ
                g_object[TABLE_L].tempreture <= 40) {   // 左側のテーブルの温度が40度以下の場合
                                                        // 左側のテーブルへ移動する.
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
                tableLR[0] = true;
            } else if (tableLR[1] == false &&           // 右側のテーブルにドリンクが置かれていなくてかつ
                g_object[TABLE_R].tempreture <= 40) {   // 右側のテーブルの温度が40度以下の場合
                                                        // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                tableLR[1] = true;
            } else {                                    // 上記以外の条件の場合
                g_object[COUNTER].position.x = g_robot_pos.x;
                                                        // カウンタへ移動する.
                main_move(g_object, COUNTER, &g_robot_pos, g_destination);
            }
            divide();                                   // 物体を設置する
            g_i++;
            break;

        case 2:                                         // 黒いドリンク（目的地=2）の位置の運搬
                                                        // ドリンク（目的地=2）の位置へ行く
            main_move(g_object, g_i, &g_robot_pos, g_destination);
            pick();                                     // 物体を把持する
            if (tableLR[0] == false &&                  // 左側のテーブルにドリンクが置かれていなくてかつ
                g_object[TABLE_L].tempreture > 40) {    // 左側のテーブルの温度が40度より高い場合
                tableLR[0] = true;
                                                        // 左側のテーブルへ移動する.
                main_move(g_object, TABLE_L, &g_robot_pos, g_destination);
            } else if (tableLR[1] == false &&           // 右側のテーブルにドリンクが置かれていなくてかつ
                g_object[TABLE_R].tempreture > 40) {    // 右側のテーブルの温度が40度より高い場合
                                                        // 右側のテーブルへ移動する.
                main_move(g_object, TABLE_R, &g_robot_pos, g_destination);
                tableLR[1] = true;
            } else {                                    // 上記以外の条件の場合
                                                        // カウンタへ移動する.
                g_object[COUNTER].position.x = g_robot_pos.x;
                main_move(g_object, COUNTER, &g_robot_pos, g_destination);
            }
            divide();                                   // 物体を設置する
            g_i++;
            break;

        case 4:                                         // 終了処理（全ドリンク運搬完了）
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, LOW);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, LOW);
            analogWrite(PWMA, 0);
            analogWrite(PWMB, 0);
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
        valA = analogRead(PHASE_A);                     // エッジの検出
        edge = 0;
        if ((valA > 550) && (state == 0)) {             // raising edge
            state = 1;
            edge = 1;
        }
        if ((valA < 450) && (state == 1)) {             // falling edge
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
        if ((valA > 550) && (state == 0)) {     // raising edge
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

        int angle = 360 * count / cpr;
        int u = (angle_REF - angle) * Kp;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, min(abs(u) + 10, 150));
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, min(abs(u) + 10, 150));
        if (abs(u) < 2) {                       // 直進用に電圧を変更
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, HIGH);
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
    while (angle > 0) {
        bringer.write(angle);
        angle = angle - 10;
        delay(5);
    }
    while (angle > 0) {
        hunger.write(angle);
        angle = angle - 10;
        delay(100);
    }
    while (160 > angle) {
        bringer.write(angle);
        angle = angle + 10;
        delay(5);
    }
}

/**
 * 物体を設置する関数
 */
void divide() {
    while (angle > 0) {
        bringer.write(angle);
        angle = angle - 10;
        delay(5);
    }
    while (angle > 0) {
        hunger.write(angle);
        angle = angle - 10;
        delay(100);
    }
    while (160 > angle) {
        bringer.write(angle);
        angle = angle + 10;
        delay(5);
    }
}

/**
 * 温度を測る関数
 * @param p_object
 * @param i
 */
void measure_thermo(Object *p_object, int i) {
    delay(500);
    p_object[i].tempreture = analogRead(THERMO);        // 温度センサで温度を測定
}