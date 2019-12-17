#include <Wire.h>
#include <SoftwareSerial.h>
#include <RTClib.h>

RTC_DS1307 RTC;
SoftwareSerial BT(2, 3);

int start_h = 7; //시작시간
int start_m = 30;
int end_h = 23; // 종료시간
int end_m = 29;

int pin_DC_A = 4;
int pin_DC_B = 5;

int LED_RED = 11;

int SOUND_SENSOR = A0;
int threshold = 500; // 감도 소리값이 이보다 클 시 동작
int Sensor_val; // 소리센서 값

bool state = false; //true = open

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(SOUND_SENSOR, INPUT);
  
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);

  pinMode(pin_DC_A, OUTPUT);
  pinMode(pin_DC_B, OUTPUT);

  if (!RTC.isrunning()) { // RTC 설정되지 않았을 시 현재시간으로 초기화.
    Serial.println("RTC is NOT running");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
}

void open() {
  digitalWrite(pin_DC_A, LOW);
  analogWrite(pin_DC_B, 150);

  delay(5000); // 5초동안 모터 돌리기

  digitalWrite(pin_DC_A, LOW); //모터 종료
  digitalWrite(pin_DC_B, LOW);

  digitalWrite(LED_RED, HIGH);

  state = true;
}

void close() {
  digitalWrite(pin_DC_B, LOW);
  analogWrite(pin_DC_A, 150);

  delay(5000); // 5초동안 모터 돌리기

  digitalWrite(pin_DC_A, LOW);
  digitalWrite(pin_DC_B, LOW);

  digitalWrite(LED_RED, LOW);

  state = false;
}

void loop() {
  if (BT.available()) {
    char c = BT.read();

    if (c == '1' && !state) open();
    else if (c == '0' && state) close();
  }

  DateTime now = RTC.now();

  if ( now.hour() >= start_h && now.hour() <= end_h) { // 시간값 비교
    if ((now.hour() == start_h && now.minute() < start_m) || now.hour() == end_h && now.minute() > end_m) { //분단위 비교
      return; // 설정시간안에 포함되지 않으면 아무동작 x
    }
    
    Sensor_val = analogRead(SOUND_SENSOR);

    if (Sensor_val > threshold && !state) open(); // 일정값 이상 && 닫혀있는 상태일 떄 동작
  }
}
