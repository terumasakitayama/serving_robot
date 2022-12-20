const int threshold_ON = 600;
const int threshold_OFF = 450;
const int PHASE_A = A0;
const int AIN1 = 9;
const int AIN2 = 10;
const int PWMA = 11;
const int BIN1 = 8;
const int BIN2 = 7;
const int PWMB = 6;

const int cpr = 48;
float Kp = 0.3;

void straight(float angle_REF);
void curve(float angle_REF);

void setup() {

    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);

    Serial.begin(115200);

}

void loop() {
    Serial.println(analogRead(PHASE_A));
    float angle_REF = 180.0;
    //test(angle_REF);

}

void straight(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    while (1) {
        valA = analogRead(PHASE_A);

        if (abs(valA - valB) > 200) {
            count++;
        }
        valB = analogRead(PHASE_A);


        int angle = 360 * count / cpr;
        int u = (angle_REF - angle) * Kp;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, min(abs(u) + 10, 150));
        /*
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, min(abs(u) + 10, 150));
        */
        if (abs(u) < 2) {
            break;
        }
    }
}

void curve(float angle_REF) {
    int valA = 0;
    int valB = 0;
    int count = 0;
    while (1) {
        valA = analogRead(PHASE_A);

        if (abs(valA - valB) > 200) {
            count++;
        }
        valB = analogRead(PHASE_A);


        int angle = 360 * count / cpr;
        int u = (angle_REF - angle) * Kp;
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, min(abs(u) + 10, 150));
        /*
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, min(abs(u) + 10, 150));
        */
        if (abs(u) < 2) {
            break;
        }
    }
}
*