void encoderA(int angle_REF) {      // A側のモーターの制御
    while (1) {
        int valA, valB;
        int conditionA, conditionB;
        int count = 0;
        valA = analogRead(PHASEL_A);
        valB = analogRead(PHASEL_B);
        // edge detection
        int edge = 0;
        if ((valA > threshold_ON) && (conditionA == 0)) { // rising edge
            conditionA = 1;
            edge = 1
        }
        if ((valA < threshold_OFF) && (conditionA == 1)) { // falling edge
            conditionA = 0;
            edge = -1
        }

        // pulse & direction count
        if (edge == 1) { // A rising
            if (conditionB) {
                count--;
            } else {
                count++;
            }
            Serial.println(count);
        }
        if (edge == -1) { // A falling
            if (conditionB) {
                count++;
            } else {
                count--;
            }
            Serial.println(count);
        }
        if (edgeB == 1) { // B rising
            if (conditionA) {
                count++;
            } else {
                count--;
            }
            Serial.println(count);
        }
        if (edgeB == -1) { // B falling
            if (conditionA) {
                count--;
            } else {
                count++;
            }
            Serial.println(count);
        }

        /*ここから先は自分で設定する*/

        double angle = 360 * count / cpr; // 角度に変換
        double u = (angle_REF - angle) * Kp; // 比例制御則
        if (u > 0) { // uの符号によって回転方向を反転
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
        } else {
            digitalWrite(AIN1, HIGH);
            digitalWrite(AIN2, LOW);
        }
        analogWrite(PWMA, min(abs(u) + 10, 150));
        // ↑PWM出力　　　　飽和 絶対値    バイアス(不感帯補償)
        if(abs(u)<2){       //ここやばいかもしれん
            break;
        }
    }
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(PWMA, 0);
}

void encoderB(int angle_REF) {      // B側のモーターの制御
    while (1) {
        int valA, valB;
        int conditionA, conditionB;
        int count = 0;
        valA = analogRead(PHASER_A);
        valB = analogRead(PHASER_B);
        // edge detection
        edge = 0;
        if ((valA > threshold_ON) && (conditionA == 0)) { // rising edge
            conditionA = 1;
            edge = 1;
        }
        if ((valA < threshold_OFF) && (conditionA == 1)) { // falling edge
            conditionA = 0;
            edge = -1;
        }
        edgeB = 0;
        if ((valB > threshold_ON) && (conditionB == 0)) { // rising edge
            conditionB = 1;
            edge = 1;
        }
        if ((valB < threshold_OFF) && (conditionB == 1)) { // falling edge
            conditionA = 0;
            edge = -1;
        }

        // pulse & direction count
        if (edge == 1) { // A rising
            if (conditionB) {
                count--;
            } else {
                count++;
            }
            Serial.println(count);
        }
        if (edge == -1) { // A falling
            if (conditionB) {
                count++;
            } else {
                count--;
            }
            Serial.println(count);
        }
        if (edgeB == 1) { // B rising
            if (conditionA) {
                count++;
            } else {
                count--;
            }
            Serial.println(count);
        }
        if (edgeB == -1) { // B falling
            if (conditionA) {
                count--;
            } else {
                count++;
            }
            Serial.println(count);
        }

        /*ここから先は自分で設定する*/

        double angle = 360 * count / cpr; // 角度に変換
        double u = (angle_REF - angle) * Kp; // 比例制御則
        if (u > 0) { // uの符号によって回転方向を反転
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, HIGH);
        } else {
            digitalWrite(BIN1, HIGH);
            digitalWrite(BIN2, LOW);
        }
        analogWrite(PWMB, min(abs(u) + 10, 150));
        // ↑PWM出力　　　　飽和 絶対値    バイアス(不感帯補償)
        if(abs(u)<2){       //ここやばいかもしれん
            break;
        }
    }
}

void move(Object *object, int i, Position *robot_pos, double *destination) {
    distance(object, i, destination, robot_pos)
    encoderA(destination[0]);
    encoderB(destination[0] * -1);
    encoderA(destination[1]);
    encoderB(destination[1]);
    robot_pos.x = object[i].position.x - (ROBOT_LENGTH * sin(distination[0]));
    robot_pos.y = object[i].position.y - (ROBOT_LENGTH * cos(distination[0]));
}

void distance(Object object, int i, double *destination, Position robot_pos) {
    double X = robot_pos.x - object[i].position.x;
    double Y = robot_pos.y - object[i].position.y;
    double angle = 90 - atan(abs(Y) / abs(X)) / PI * 180;
    double dis = sqrt(pow(X, 2) + pow(Y, 2)) / (2 * PI * RADIUS) * 360;
    destination[0] = angle;
    destination[1] = dis;
}

void pick() {
    bringer.write();
    hunger.write();
    bringer.write();
}

void divide() {
    bringer.write();
    hunger.write();
    bringer.write();
}

void measure_thermo(Object *object, int i){
    delay(500);
    object[i].thermal = analogread(THERMO);
}

void measure_light(Object *object, int i){
    object[i].light = analogread(LIGHT);
}