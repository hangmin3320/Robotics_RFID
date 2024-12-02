#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>  // ArduinoJson 라이브러리
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10
#define ANGLE_OPEN 90
#define ANGLE_CLOSED 150

const int redPin = 4;    // 빨강 LED 핀
const int bluePin = 3;   // 녹색 LED 핀

MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo servo;

struct UIDMapping {
    String uid;
    String name;
};

UIDMapping allowedUsers[] = {
    {"832A6310", "hangmin"},  // UID와 이름 매핑
    {"33C3D5F", "Test"}
};

// 거부된 사용자 명단
UIDMapping deniedUsers[] = {
    {"444E9A9", "hoon"}  // UID와 이름 매핑
};

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    servo.attach(6);
    servo.write(ANGLE_CLOSED);
}

void loop() {
    // 카드가 없으면 대기
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
        return;

    // UID 읽기
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();  // UID를 대문자로 변환

    // 허가 및 거부 여부 확인
    String userName = findNameByUID(uid);
    String deniedUserName = findDeniedNameByUID(uid);
    bool isDenied = deniedUserName != "";
    bool isAllowed = (!isDenied && userName != "");

    // JSON 데이터 생성
    StaticJsonDocument<256> jsonDoc;  // JSON 문서 객체
    jsonDoc["uid"] = uid;

    // JSON user 필드 설정
    if (isDenied) {
        jsonDoc["user"] = deniedUserName;  // 거부된 사용자 이름
    } else if (isAllowed) {
        jsonDoc["user"] = userName;  // 허가된 사용자 이름
    } else {
        jsonDoc["user"] = "Unknown";  // 알 수 없는 사용자
    }

    // JSON access 필드 설정
    jsonDoc["access"] = isAllowed ? "Authorized" : "Denied";

    // JSON 문자열로 출력
    String jsonOutput;
    serializeJson(jsonDoc, jsonOutput);
    Serial.println(jsonOutput);

    if (isAllowed) {
        grantAccess();
    } else if (isDenied) {
        denyAccess("Denied User");  // 거부된 사용자 처리
    } else {
        denyAccess("Unknown User");  // Unknown 사용자 처리
    }

    mfrc522.PICC_HaltA(); // 태그 멈춤
    delay(1000);          // 딜레이
}

// 허가된 사용자 이름 찾기
String findNameByUID(String uid) {
    for (int i = 0; i < sizeof(allowedUsers) / sizeof(allowedUsers[0]); i++) {
        if (uid == allowedUsers[i].uid) {
            return allowedUsers[i].name;
        }
    }
    return ""; // UID가 매칭되지 않으면 빈 문자열 반환
}

// 거부된 사용자 이름 찾기
String findDeniedNameByUID(String uid) {
    for (int i = 0; i < sizeof(deniedUsers) / sizeof(deniedUsers[0]); i++) {
        if (uid == deniedUsers[i].uid) {
            return deniedUsers[i].name;
        }
    }
    return ""; // UID가 매칭되지 않으면 빈 문자열 반환
}

// 출입 허가 동작
void grantAccess() {
  digitalWrite(bluePin, HIGH);
  digitalWrite(redPin, LOW);  // 녹색은 끔
  servo.write(ANGLE_OPEN);
  delay(2000);                 // 2초 대기
  servo.write(ANGLE_CLOSED);
  digitalWrite(bluePin, LOW);   
}

// 출입 거부 동작
void denyAccess(String reason) {
  digitalWrite(redPin, HIGH);
  digitalWrite(bluePin, LOW);  // 파란색은 끔
  delay(2000);                 // 2초 대기

  digitalWrite(redPin, LOW);    
}