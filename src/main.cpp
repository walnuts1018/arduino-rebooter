#include <Arduino.h>
int Mode_FLAG = 0;
int alice_status = 1;
int error_count = 0;
unsigned long timer;
unsigned long prev;
unsigned long diff;
int rasp_status = 0;
int rasp_sign = 0;
int rasp_count = 0;
int switch_status = 0;
int network = 0;
int switch_con(int push_length);
int rasp_no_sign = 0;

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  pinMode(8, OUTPUT);
  pinMode(10, INPUT);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  timer = millis();
  
}

void loop() {
  alice_status = digitalRead(10);
  rasp_sign = digitalRead(12);

  // 既定の8回（40秒）を超えてから、5回以上連続でラズパイからの信号がhighの時、ラズパイが固まったと判断
  if (5 <= rasp_status){
    Mode_FLAG = 1;
  }

  if (9 <= rasp_count){
    rasp_status += 1;
  }

  if (rasp_sign == 1){
    Mode_FLAG = 0;
    rasp_count += 1;
    rasp_no_sign = 0;
  }else if (rasp_sign == 0){
    rasp_no_sign += 1;
    if (rasp_no_sign >= 62){
      rasp_status += 1;
      rasp_no_sign = 0;
    }
    if (1 <= rasp_count and rasp_count < 4){
      rasp_status += 1;
    }else if (4 <= rasp_count){
      rasp_status = 0;
      delay(1);
    }
    rasp_count = 0;
  }

  if (Mode_FLAG == 1){
    digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(13,LOW);
    rasp_count = 0;
    rasp_status = 0;
    rasp_no_sign = 0;

    if (alice_status == 0){
      if (diff > 2*86400000){
        switch_status = switch_con(6000);
        if (switch_status == 1){
          if (error_count > 3){
            switch_con(12000);
            Mode_FLAG = 2;
            delay(120000);
          }else{
          error_count = error_count + 1;
          delay(60000);
          }
        }else{
          timer = millis();
        }
      }
    }else if (alice_status == 1){
      prev = millis();
      diff = prev - timer;
      if (diff < 0){
        diff = 4294967296 - timer + prev;
      }
      if (diff > 86400000){
        switch_status = switch_con(2000);
        if (switch_status == 0){
          if (error_count > 3){
            Mode_FLAG = 2;
            delay(120000);
          }else{
          error_count = error_count + 1;
          delay(60000);
          }
        }else{
          timer = millis();
        }
      }
    }
  }
  delay(5000);
}

int switch_con(int push_length) {
      digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    delay(100);
  digitalWrite(8,HIGH);
  delay(push_length);
  digitalWrite(8,LOW);
  delay(60000);

  // パソコンの電源が入ると10番がLOWになる
  alice_status = digitalRead(10);
  if (alice_status == 0){

    return 1;
  }else
  {
    return 0;
  }
}