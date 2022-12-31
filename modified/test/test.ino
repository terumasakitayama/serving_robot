#include <Servo.h>

const int LED = 4;
const int AIN1 = 9;
const int AIN2 = 10;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMA = 11;
const int PWMB = 6;
const int PHASE_A = A0;
const int PHASE_B = A1;
const int THERMO = A2;
const int LIGHT = A3;
const double ROBOT_LENGTH = 50.0;
const double RADIUS = 17.0;
const int OBJECT_NUM = 4;
const int TABLE_L = 4;
const int TABLE_R = 5;
#define PI 3.14159265358979

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

void straight(float angle_REF);
void rotate(float angle_REF);
void distance(Object *object, int i, double *p_destination, POSITION *p_robot_pos);
void main_move(Object *object, int i, POSITION *p_robot_pos, double *p_destination);
void pick();
void divide();
void measure_thermo(Object *p_object, int i);
void measure_light(Object *p_object, int i);

int g_i = 0;

double g_destination[2];
POSITION g_robot_pos;
boolean table_hot = false;
boolean table_cool = false;
boolean sign = true;
boolean tableLR[2] = {false, false};
Object g_object[7];

void setup() {
    hunger.attach(3);
    bringer.attach(5);
    hunger.write(90);
    bringer.write(90);

    g_object[0].position = {300, 500};


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

void loop(){
    g_object[0].position = {500, 500};
    g_robot_pos = {500, 200};
    
    // 直径×5だけ進む
    // straight(1800);

    // 60°回転
    // rotate(60);

    // 持ち上げ
    // pickup();

    // 設置
    // divide();
}

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

