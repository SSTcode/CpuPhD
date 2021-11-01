/*
 * stdafx.c
 *
 *  Created on: 13 paz 2019
 *      Author: Mr.Tea
 */

#include "stdafx.h"

#pragma SET_DATA_SECTION("CLAData")

union STATUS status;
union CONTROL control;
union ALARM alarm;
union ALARM alarm_snapshot;

struct Timer_PWM_struct Timer_PWM;

struct CIC2_struct CIC2_calibration;
CLA_FPTR CIC2_calibration_input;

struct INVERTER_struct INV;

struct Measurements_struct Meas;
struct Measurements_gain_offset_struct Meas_gain;
struct Measurements_gain_offset_struct Meas_offset;
struct Measurements_alarm_struct Meas_alarm_H;
struct Measurements_alarm_struct Meas_alarm_L;

struct EMIF_CLA_struct EMIF_CLA;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

#pragma SET_DATA_SECTION("EMIF_mem")

volatile union EMIF_union EMIF_mem;
