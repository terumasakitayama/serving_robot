
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
const int PHASER_A = A0;        // ロータリーエンコーダ
const int PHASER_B = A1;
const int PHASEL_A = A2;
const int PHASEL_B = A3;
const int THERMO = A4;
const int LIGHT = A5;
const int ROBOT_LENGTH = ;      // ロボットの長さ
const int RADIUS = 17;          // 車輪の半径
const int OBJECT_NUM 4          // ドリンクの数
const int TABLE_L 4             // 左側のテーブル
const int TABLE_R 5             // 右側のテーブル
#define PI 3.14159265358979


//----------------------------------
// Objectの構造体の定義
//----------------------------------

struct Position         // πを使うためdouble型にしている
{
    double x;
    double y;
};

typedef struct
{
    int index;
    Position position;
    int tempreture;
    int light;
}
Object;

typedef enum
{
    START,
    APPROACH,
    PICKUP,
    TABLE_WHITE,
    TABLE_BLACK,
    COUNTER,
    PLACE,
    FINISH
} TaskState;
TaskState task_state = START;

//----------------------------------
// 関数のプロトタイプ宣言
//----------------------------------

void straight(float angle_REF);         // まっすぐ進むための関数(main_moveで使用)
void rotate(float angle_REF);           // 回転するための関数(main_moveで使用)
void distance(Object object, int i, double *destination, Position robot_pos);       // 目的地までの距離と角度を測定する関数(main_moveで使用)
void main_move(Object *object, int i, Position *robot_pos, double *destination);    // 目的地へ動く関数
void pick();                                            // 物体を把持する関数
void divide();                                          // 物体を設置する関数
void measure_thermo(Object *object, int i);             // 温度を測る関数
void measure_light(Object *object, int i);              // ドリンクの色を測る関数