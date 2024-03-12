/**
 *
 *	@file		sonar.cpp
 *
 *	@brief		超音波距離センサー処理
 *
 *	@author		株式会社エムスクエア・ラボ　山形
 *
 *	@date		create : 2023/11/09
 *
 */
#include <Arduino.h>
#include "timer.hpp"
/*==========================================================================================
	外部関数定義
==========================================================================================*/


/*==========================================================================================
	内部定数定義
==========================================================================================*/

/*==========================================================================================
	内部関数定義
==========================================================================================*/
int readDistance(uint8_t addr);
int readN(uint8_t *buffer, size_t len);
unsigned int CRC16_2(unsigned char *buf, int len);
/*==========================================================================================
	外部変数定義
==========================================================================================*/
uint16_t sonarData[4] = {0,0,0,0};
/*==========================================================================================
	内部変数定義
==========================================================================================*/

/*==========================================================================================
	関数
==========================================================================================*/

/**
 *	@brief		超音波距離センサー初期化
 *	@param		無し
 *	@retval		無し
 */
void initSonar(){
  Serial2.begin(115200); 
  delay(1000);
  unsigned char Com[8] = {0x01,0x06,0x02,0x00,0x00,0x05,0x48,0x71}; //アドレス変更コマンド
  // unsigned char Com[8] = {0x05,0x06,0x02,0x08,0x00,0x01,0x48,0x71}; //検出角度変更
  // uint16_t crc = CRC16_2(Com,6);
  // Com[6] = (crc>>8) & 0xFF;
  // Com[7] = crc & 0xFF;
  // // for(int i = 0; i < 8; i++){
  // //   Serial.print(Com[i],HEX);
  // //   Serial.print(" ");
  // // }
  // Serial2.write(Com, 8);
  // readN(Com, sizeof(Com));
}

/**
 *	@brief		超音波距離センサーデータ取得
 *	@param		無し
 *	@retval		無し
 */
void getSonarData(){
  static uint32_t backTime = millis();

  sonarData[0] =readDistance(0x01);
  Serial.print("Distance1 = ");
  Serial.print(sonarData[0]);
  Serial.println(" mm");
  delay(5);
  sonarData[1] =readDistance(0x05);
  Serial.print("Distance2 = ");
  Serial.print(sonarData[1]);
  Serial.println(" mm");
  delay(5);
  
  Serial.println(millis() - backTime);
  backTime = millis();
  Serial.println();
}

/**
 *	@brief		超音波距離センサーデータ取得
 *	@param		addr:取得アドレス
 *	@retval		Distance:取得距離
 */
int readDistance(uint8_t addr){
  unsigned char Com[8] = {addr,0x03,0x01,0x01,0x00,0x01,0x00,0x00}; //距離取得コマンド
  unsigned char Data[16] = {0};
  int Distance = 0;
  int dataCount = 0;
  uint16_t crc;
  crc = CRC16_2(Com,6);
  Com[6] = (crc>>8) & 0xFF;
  Com[7] = crc & 0xFF;
  
  
  Serial2.write(Com, 8);
  dataCount = readN(Data, sizeof(Data));
  if(dataCount){  //データ取得したらデータ換算開始
    //CRCチェック
    if (CRC16_2(Data, dataCount-1) == ((Data[dataCount-1] << 8) | Data[dataCount])) {
      //コマンド内容確認
      for(int i = 0; i < 2; i++){
        if (Data[i] != Com[i]) { 
          Serial.println("Command Error");
          return Distance;
        }
      }
      //データ取得
      for(int i = 0; i < Data[2]; i++){
        Distance <<= 8;
        Distance |= Data[3+i];
      }
    }
  }
  return Distance;
}

/**
 *	@brief		超音波距離センサーデータ取得
 *	@param		buffer:格納アドレス len:データ長
 *	@retval		offset:データ取得数
 */
int readN(uint8_t *buffer, size_t len){
  int offset = 0;
  const uint32_t endCount = 305; //データ取得後3.5文字分待機して終了
  // uint8_t  buffer[16];
  uint32_t curr = micros();
  IntervalTimer timeOut(500);
  timeOut.init();
  while (offset < len) {
    if (Serial2.available()) {
      curr = micros();
      if(offset < len){
        buffer[offset] = Serial2.read();
        // Serial.print(buffer[offset],HEX);
        // Serial.print(" ");
        offset++;
      }
    }
    //3.5文字分の空白で取得終了
    if (((micros() - curr) > endCount)&&(offset)) {
      offset--;//文字数調整
      break;
    }
    //取得に時間がかかりすぎた場合は終了
    if(timeOut.isWait()){
      offset = 0;
      break;
    }
  }
  Serial.println();
  return offset;
}

/**
 *	@brief		CRC16 Modbus変換
 *	@param		buf:格納アドレス len:データ長
 *	@retval		crc:変換データ
 */
unsigned int CRC16_2(unsigned char *buf, int len){
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++){
    crc ^= (unsigned int)buf[pos];
    for (int i = 8; i != 0; i--){
      if ((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }else{
        crc >>= 1;
      }
    }
  }

  crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
  return crc;
}
