#include <EEPROM.h>

int n;
int light;
int thermo;
boolean table;
boolean tableLR[2];
typedef struct {
    double x;
    double y;
}pos;

void interface(pos object, pos robot, double *degree);

void setup() {
    Serial.begin(115200);
    pos robot;
    pos object;
    double degree[2];
    int adress = 0x00;
    for (int i = 0; i < 4; i++) {
        EEPROM.get(adress, n);
        adress += sizeof(n);
        EEPROM.get(adress, object);
        adress += sizeof(object);
        EEPROM.get(adress, robot);
        adress += sizeof(robot);
        EEPROM.get(adress, degree);
        adress += sizeof(degree);
        EEPROM.get(adress, light);
        adress += sizeof(light);
        EEPROM.get(adress, table);
        adress += sizeof(table);
        EEPROM.get(adress, tableLR);
        adress += sizeof(tableLR);


        Serial.println("-------------------------------------");
        Serial.print(n+1);
        Serial.println(F(" 個目のドリンク"));
        Serial.println(F(" "));
        Serial.println(F("ドリンクへ移動するフェーズ"));

        interface(object, robot, degree);
        Serial.println(F(" "));
        Serial.println(F("ドリンクを把持するフェーズ"));
        if (light > 800) {
            Serial.println(F("ドリンクは白色"));
            if (table) {
                Serial.println(F("テーブルにすでに白いドリンクを置いているため、カウンターへ移行"));
                Serial.println(F(" "));
                Serial.println(F("白いドリンクを持ち、カウンターへ移行するフェーズ"));
                EEPROM.get(adress, object);
                adress += sizeof(object);
                EEPROM.get(adress, robot);
                adress += sizeof(robot);
                EEPROM.get(adress, degree);
                adress += sizeof(degree);
                Serial.println(F("y軸に平行に移動"));
                interface(object, robot, degree);
                Serial.println(F("カウンターにドリンクを設置したため、次のドリンクへ"));
                Serial.println(F(" "));

            } else {
                Serial.println(F("テーブルにまだ白いドリンクを置いてないため、テーブルへ移行"));
                Serial.println(F(" "));
                Serial.println(F("白いドリンクを持ち、テーブルへ移行するフェーズ"));
                if (tableLR[0] == true) {
                    Serial.println(F("テーブルの状況 : 左だけが空"));
                    Serial.println(F("右のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    interface(object, robot, degree);
                    Serial.println(F("テーブルにドリンクを設置したため、次のドリンクへ"));

                } else if (tableLR[1] == true) {
                    Serial.println(F("テーブルの状況 : 右だけが空"));
                    Serial.println(F("左のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    interface(object, robot, degree);
                    Serial.println(F("テーブルにドリンクを設置したため、次のドリンクへ"));
                    Serial.println(F(" "));
                } else {
                    Serial.println(F("テーブルの状況 : 両方とも空"));
                    Serial.println(F("左のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);
                    EEPROM.get(adress, thermo);
                    adress += sizeof(thermo);

                    interface(object, robot, degree);

                    if (thermo > 40) {
                        Serial.println(F("右のテーブルの温度が高いため、左のテーブルへ移行"));
                        EEPROM.get(adress, object);
                        adress += sizeof(object);
                        EEPROM.get(adress, robot);
                        adress += sizeof(robot);
                        EEPROM.get(adress, degree);
                        adress += sizeof(degree);
                        interface(object, robot, degree);
                        Serial.println(F("右のテーブルへ設置して次のドリンクへ"));
                        Serial.println(F(" "));
                    } else {
                        Serial.println(F("右のテーブルの温度が低いため、設置して次のドリンクへ"));
                        Serial.println(F(" "));
                    }
                }

            }
        } else {
            Serial.println(F("ドリンクは黒色"));
            if (table) {
                Serial.println(F("テーブルにすでに黒いドリンクを置いているため、カウンターへ移行"));
                Serial.println(F(" "));
                Serial.println(F("黒いドリンクを持ち、カウンターへ移行するフェーズ"));
                EEPROM.get(adress, object);
                adress += sizeof(object);
                EEPROM.get(adress, robot);
                adress += sizeof(robot);
                EEPROM.get(adress, degree);
                adress += sizeof(degree);
                Serial.println(F("y軸に平行に移動"));
                interface(object, robot, degree);
                Serial.println(F("カウンターにドリンクを設置したため、次のドリンクへ"));
                Serial.println(F(" "));

            } else {
                Serial.println(F("テーブルにまだ黒いドリンクを置いてないため、テーブルへ移行"));
                Serial.println(F(" "));
                Serial.println(F("黒いドリンクを持ち、テーブルへ移行するフェーズ"));
                if (tableLR[0] == true) {
                    Serial.println(F("テーブルの状況 : 左だけが空"));
                    Serial.println(F("右のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    interface(object, robot, degree);
                    Serial.println(F("テーブルにドリンクを設置したため、次のドリンクへ"));
                    Serial.println(F(" "));

                } else if (tableLR[1] == true) {
                    Serial.println(F("テーブルの状況 : 右だけが空"));
                    Serial.println(F("左のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    interface(object, robot, degree);
                    Serial.println(F("テーブルにドリンクを設置したため、次のドリンクへ"));
                    Serial.println(F(" "));
                } else {
                    Serial.println(F("テーブルの状況 : 両方とも空"));
                    Serial.println(F("左のテーブルに移行"));
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);
                    EEPROM.get(adress, thermo);
                    adress += sizeof(thermo);

                    interface(object, robot, degree);

                    if (thermo < 40) {
                        Serial.println(F("右のテーブルの温度が低いため、左のテーブルへ移行"));
                        EEPROM.get(adress, object);
                        adress += sizeof(object);
                        EEPROM.get(adress, robot);
                        adress += sizeof(robot);
                        EEPROM.get(adress, degree);
                        adress += sizeof(degree);
                        interface(object, robot, degree);
                        Serial.println(F("右のテーブルへ設置して次のドリンクへ"));
                        Serial.println(F(" "));
                    } else {
                        Serial.println(F("右のテーブルの温度が高いため、設置して次のドリンクへ"));
                        Serial.println(F(" "));
                    }
                }

            }
        }
    }
}

void loop() {

}


void interface(pos object, pos robot, double *degree){
    Serial.print("目的地のx座標 : ");
    Serial.println(object.x);
    Serial.print("目的地のy座標 : ");
    Serial.println(object.y);
    Serial.print("ロボットのx座標 : ");
    Serial.println(robot.x);
    Serial.print("ロボットのy座標 : ");
    Serial.println(robot.x);
    Serial.print("目的地までの回転角 : ");
    Serial.println(degree[0]);
    Serial.print("直進のための回転角 : ");
    Serial.println(degree[1]);
}