#include <Arduino.h>
#include "timer.hpp"
#include "sonar.hpp"
#include "can.hpp"
IntervalTimer it(500);
void setup(){
  Serial.begin(115200);  
  delay(1000);
  canBegin();
  initSonar();
  

}
void loop(){
  getSonarData();
  if(it.isWait()){
    canIsSend();
  }
}
