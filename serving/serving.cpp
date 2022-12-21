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
        Serial.println(count);

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

void main_move(Object *object, int i, Position *robot_pos, double *destination) {
    distance(object, i, destination, robot_pos)
    rotate(destination[0]);
    straight(destination[1]);
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

void measure_thermo(Object * object, int i) {
    delay(500);
    object[i].thermal = analogread(THERMO);
}

void measure_light(Object * object, int i) {
    object[i].light = analogread(LIGHT);
}