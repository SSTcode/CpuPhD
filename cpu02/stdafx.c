/*
 * stdafx.c
 *
 *  Created on: 13 paz 2019
 *      Author: Mr.Tea
 */

#include "stdafx.h"

#pragma SET_DATA_SECTION("CLAData")

struct CLA1toCLA2_struct CLA1toCLA2;

struct Timer_PWM_struct Timer_PWM;

struct Measurements_struct Meas;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

