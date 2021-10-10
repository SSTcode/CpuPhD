/*
 * CPU_shared.h
 *
 *  Created on: 16 sie 2020
 *      Author: MrTea
 */

#ifndef CPU_SHARED_H_
#define CPU_SHARED_H_

#include "IO.h"

struct Measurements_struct
{
    float temperature_0A;
    float temperature_0B;
    float temperature_1A;
    float temperature_1B;
    float temperature_2A;
    float temperature_2B;
    float temperature_3A;
    float temperature_3B;
    float I_conv_0;
    float U_dc_0;
    float I_conv_1;
    float U_dc_1;
    float I_conv_2;
    float U_dc_2;
    float I_conv_3;
    float U_dc_3;
    float I_grid;
    float U_grid;
};

struct CPU1toCPU2_struct
{

};

struct CPU2toCPU1_struct
{

};

struct CLA1toCLA2_struct
{

};

struct CLA2toCLA1_struct
{

};

#endif /* CPU_SHARED_H_ */
