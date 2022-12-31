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

void setup() {
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
        Serial.println((n + 1) + " 個目のドリンク");
        Serial.println(" ");
        Serial.println("ドリンクへ移動するフェーズ");
        Serial.println("ドリンクのx座標 : ");
        Serial.print(object.x);
        Serial.println("ドリンクのy座標 : ");
        Serial.print(object.y);
        Serial.println("ロボットのx座標 : ");
        Serial.print(robot.x);
        Serial.println("ロボットのy座標 : ");
        Serial.print(robot.x);
        Serial.println("目的地までの回転角 : ");
        Serial.print(degree[0]);
        Serial.println("直進のための回転角 : ");
        Serial.print(degree[1]);
        Serial.println(" ");
        Serial.println("ドリンクを把持するフェーズ");
        if (light > 800) {
            Serial.println("ドリンクは白色");
            if (table) {
                Serial.println("テーブルにすでに白いドリンクを置いているため、カウンターへ移行");
                Serial.println(" ");
                Serial.println("白いドリンクを持ち、カウンターへ移行するフェーズ");
                EEPROM.get(adress, object);
                adress += sizeof(object);
                EEPROM.get(adress, robot);
                adress += sizeof(robot);
                EEPROM.get(adress, degree);
                adress += sizeof(degree);
                Serial.println("y軸に平行に移動");
                Serial.println("カウンターのx座標 : ");
                Serial.print(object.x);
                Serial.println("カウンターのy座標 : ");
                Serial.print(object.y);
                Serial.println("ロボットのx座標 : ");
                Serial.print(robot.x);
                Serial.println("ロボットのy座標 : ");
                Serial.print(robot.x);
                Serial.println("目的地までの回転角 : ");
                Serial.print(degree[0]);
                Serial.println("直進のための回転角 : ");
                Serial.print(degree[1]);
                Serial.print("カウンターにドリンクを設置したため、次のドリンクへ");
                Serial.println(" ");

            } else {
                Serial.println("テーブルにまだ白いドリンクを置いてないため、テーブルへ移行");
                Serial.println(" ");
                Serial.println("白いドリンクを持ち、テーブルへ移行するフェーズ");
                if (tableLR[0] == true) {
                    Serial.println("テーブルの状況 : 左だけが空");
                    Serial.println("右のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);
                    Serial.print("テーブルにドリンクを設置したため、次のドリンクへ");

                } else if (tableLR[1] == true) {
                    Serial.println("テーブルの状況 : 右だけが空");
                    Serial.println("左のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);
                    Serial.print("テーブルにドリンクを設置したため、次のドリンクへ");
                    Serial.println(" ");
                } else {
                    Serial.println("テーブルの状況 : 両方とも空");
                    Serial.println("左のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);
                    EEPROM.get(adress, thermo);
                    adress += sizeof(thermo);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);

                    if (thermo > 40) {
                        Serial.println("右のテーブルの温度が高いため、左のテーブルへ移行");
                        EEPROM.get(adress, object);
                        adress += sizeof(object);
                        EEPROM.get(adress, robot);
                        adress += sizeof(robot);
                        EEPROM.get(adress, degree);
                        adress += sizeof(degree);
                        Serial.println("テーブルのx座標 : ");
                        Serial.print(object.x);
                        Serial.println("テーブルのy座標 : ");
                        Serial.print(object.y);
                        Serial.println("ロボットのx座標 : ");
                        Serial.print(robot.x);
                        Serial.println("ロボットのy座標 : ");
                        Serial.print(robot.x);
                        Serial.println("目的地までの回転角 : ");
                        Serial.print(degree[0]);
                        Serial.println("直進のための回転角 : ");
                        Serial.print(degree[1]);
                        Serial.println("右のテーブルへ設置して次のドリンクへ");
                        Serial.println(" ");
                    } else {
                        Serial.println("右のテーブルの温度が低いため、設置して次のドリンクへ");
                        Serial.println(" ");
                    }
                }

            }
        } else {
            Serial.println("ドリンクは黒色");
            if (table) {
                Serial.println("テーブルにすでに黒いドリンクを置いているため、カウンターへ移行");
                Serial.println(" ");
                Serial.println("黒いドリンクを持ち、カウンターへ移行するフェーズ");
                EEPROM.get(adress, object);
                adress += sizeof(object);
                EEPROM.get(adress, robot);
                adress += sizeof(robot);
                EEPROM.get(adress, degree);
                adress += sizeof(degree);
                Serial.println("y軸に平行に移動");
                Serial.println("カウンターのx座標 : ");
                Serial.print(object.x);
                Serial.println("カウンターのy座標 : ");
                Serial.print(object.y);
                Serial.println("ロボットのx座標 : ");
                Serial.print(robot.x);
                Serial.println("ロボットのy座標 : ");
                Serial.print(robot.x);
                Serial.println("目的地までの回転角 : ");
                Serial.print(degree[0]);
                Serial.println("直進のための回転角 : ");
                Serial.print(degree[1]);
                Serial.print("カウンターにドリンクを設置したため、次のドリンクへ");
                Serial.println(" ");

            } else {
                Serial.println("テーブルにまだ黒いドリンクを置いてないため、テーブルへ移行");
                Serial.println(" ");
                Serial.println("黒いドリンクを持ち、テーブルへ移行するフェーズ");
                if (tableLR[0] == true) {
                    Serial.println("テーブルの状況 : 左だけが空");
                    Serial.println("右のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);
                    Serial.print("テーブルにドリンクを設置したため、次のドリンクへ");
                    Serial.println(" ");

                } else if (tableLR[1] == true) {
                    Serial.println("テーブルの状況 : 右だけが空");
                    Serial.println("左のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);
                    Serial.print("テーブルにドリンクを設置したため、次のドリンクへ");
                    Serial.println(" ");
                } else {
                    Serial.println("テーブルの状況 : 両方とも空");
                    Serial.println("左のテーブルに移行");
                    EEPROM.get(adress, object);
                    adress += sizeof(object);
                    EEPROM.get(adress, robot);
                    adress += sizeof(robot);
                    EEPROM.get(adress, degree);
                    adress += sizeof(degree);
                    EEPROM.get(adress, thermo);
                    adress += sizeof(thermo);

                    Serial.println("テーブルのx座標 : ");
                    Serial.print(object.x);
                    Serial.println("テーブルのy座標 : ");
                    Serial.print(object.y);
                    Serial.println("ロボットのx座標 : ");
                    Serial.print(robot.x);
                    Serial.println("ロボットのy座標 : ");
                    Serial.print(robot.x);
                    Serial.println("目的地までの回転角 : ");
                    Serial.print(degree[0]);
                    Serial.println("直進のための回転角 : ");
                    Serial.print(degree[1]);

                    if (thermo < 40) {
                        Serial.println("右のテーブルの温度が低いため、左のテーブルへ移行");
                        EEPROM.get(adress, object);
                        adress += sizeof(object);
                        EEPROM.get(adress, robot);
                        adress += sizeof(robot);
                        EEPROM.get(adress, degree);
                        adress += sizeof(degree);
                        Serial.println("テーブルのx座標 : ");
                        Serial.print(object.x);
                        Serial.println("テーブルのy座標 : ");
                        Serial.print(object.y);
                        Serial.println("ロボットのx座標 : ");
                        Serial.print(robot.x);
                        Serial.println("ロボットのy座標 : ");
                        Serial.print(robot.x);
                        Serial.println("目的地までの回転角 : ");
                        Serial.print(degree[0]);
                        Serial.println("直進のための回転角 : ");
                        Serial.print(degree[1]);
                        Serial.println("右のテーブルへ設置して次のドリンクへ");
                        Serial.println(" ");
                    } else {
                        Serial.println("右のテーブルの温度が高いため、設置して次のドリンクへ");
                        Serial.println(" ");
                    }
                }

            }
        }
    }
}

void loop() {

}