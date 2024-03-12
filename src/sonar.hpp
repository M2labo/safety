/**
 *
 *	@file		sonar.hpp
 *
 *	@brief		超音波距離センサー処理
 *
 *	@author		株式会社エムスクエア・ラボ　山形
 *
 *	@date		create : 2023/11/09
 *
 */
#ifndef  __SONAR_HPP__
#define __SONAR_HPP__
#include <Arduino.h>
/*==========================================================================================
	定数定義
==========================================================================================*/


/*==========================================================================================
	変数定義
==========================================================================================*/
/**
 * @struct	
 * @brief	
 */
extern uint16_t sonarData[4];
/*==========================================================================================
	関数定義
==========================================================================================*/
extern void initSonar();
extern void getSonarData();
extern int readDistance(uint8_t addr);

#endif