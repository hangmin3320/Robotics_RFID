#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>  // ArduinoJson 라이브러리

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

// UID와 이름 매핑
struct UIDMapping {
    String uid;
    String name;
};

UIDMapping allowedUsers[] = {
    {"832A6310", "hangmin"},  // UID와 이름 매핑
    {"33C3D5F", "Test"}
};

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
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

    // 허가 여부 확인
    String userName = findNameByUID(uid);
    bool isAllowed = userName != "";

    // JSON 데이터 생성
    StaticJsonDocument<256> jsonDoc;  // JSON 문서 객체
    jsonDoc["uid"] = uid;
    jsonDoc["user"] = isAllowed ? userName : "Unknown";
    jsonDoc["access"] = isAllowed ? "Authorized" : "Denied";

    // JSON 문자열로 출력
    String jsonOutput;
    serializeJson(jsonDoc, jsonOutput);
    Serial.println(jsonOutput);

    if (isAllowed) {
        grantAccess();  // 출입 허가 동작 (예: 문 열기)
    } else {
        denyAccess();  // 출입 거부 동작 (예: 경고음)
    }

    mfrc522.PICC_HaltA(); // 태그 멈춤
    delay(1000);          // 딜레이
}

// UID로 이름 찾기
String findNameByUID(String uid) {
    for (int i = 0; i < sizeof(allowedUsers) / sizeof(allowedUsers[0]); i++) {
        if (uid == allowedUsers[i].uid) {
            return allowedUsers[i].name;
        }
    }
    return ""; // UID가 매칭되지 않으면 빈 문자열 반환
}

void grantAccess() {
    // 여기서 릴레이, 서보 모터 등을 제어
}

void denyAccess() {
    // 여기서 부저 제어
}
