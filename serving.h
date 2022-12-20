
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
const int PHASER_A = A0;     // ロータリーエンコーダ
const int PHASER_B = A1;
const int PHASEL_A = A2;
const int PHASEL_B = A3;
const int THERMO = A4;
const int LIGHT = A5;
const int ROBOT_LENGTH = ;      // ロボットの長さ
const int RADIUS = 17;      // 車輪の半径
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

// ロータリーエンコーダの処理を行うための関数
void encoderA(int angle_REF);
void encoderB(int angle_REF);