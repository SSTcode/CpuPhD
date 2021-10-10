/*
 * Blink.cpp
 *
 *  Created on: 19 wrz 2020
 *      Author: Mr.Tea
 */

#include <stdafx.h>
#include <Blink.h>
#include <stdlib.h>

float Blink_class::cpu_freq = 200e6;
float Blink_class::div_cpu_freq = 1.0f / cpu_freq;

void Blink_class::reset_counter()
{
    internal_counter =
    pattern_cursor = 0;
    external_counter_old = IpcRegs.IPCCOUNTERL;
}

bool Blink_class::task()
{
    register Uint32 counter_new = IpcRegs.IPCCOUNTERL;
    register Uint32 integer_period_temp = integer_period;
    register Uint32 internal_counter_temp = internal_counter + __lmin(counter_new - external_counter_old, integer_period_temp);
    external_counter_old = counter_new;

    register Uint32 pattern_cursor_temp = pattern_cursor;
    if((float)internal_counter_temp * div_cpu_freq > fabs(blink_pattern[pattern_cursor_temp])) pattern_cursor_temp++;

    register bool out = false;
    if(blink_pattern[pattern_cursor_temp] > 0.0f) out = true;

    if(internal_counter_temp >= integer_period_temp)
    {
        internal_counter_temp -= integer_period_temp;
        pattern_cursor_temp = 0;
        zero_crossing = true;
    }
    else zero_crossing = false;

    internal_counter = internal_counter_temp;
    pattern_cursor = pattern_cursor_temp;
    return output_state = out;
}

bool Blink_class::task_simple()
{
    register Uint32 counter_new = IpcRegs.IPCCOUNTERL;
    register Uint32 integer_period_temp = integer_period;
    register Uint32 internal_counter_temp = internal_counter + __lmin(counter_new - external_counter_old, integer_period_temp);
    external_counter_old = counter_new;

    register bool out = false;
    if(internal_counter_temp >= integer_period_temp)
    {
        internal_counter_temp -= integer_period_temp;
        out = true;
    }
    internal_counter = internal_counter_temp;
    return output_state = out;
}

void Blink_class::update_pattern(float period, float pattern[])
{
    integer_period = period * cpu_freq;
    blink_pattern = pattern;
}

void Blink_class::update_pattern(float period, float duty)
{
    internal_blink_pattern[0] = duty * period;
    internal_blink_pattern[1] =
    internal_blink_pattern[2] = -period;
    integer_period = fabs(period) * cpu_freq;
    blink_pattern = internal_blink_pattern;
}

void Blink_class::update_pattern(bool val)
{
    reset_counter();
    internal_blink_pattern[0] =
    internal_blink_pattern[1] = (val) ? 1.0f : -1.0f;
    integer_period = cpu_freq;
    blink_pattern = internal_blink_pattern;
}

void Blink_class::update_pattern(float period)
{
    internal_blink_pattern[0] = -period;
    internal_blink_pattern[1] = period ? FLT_MIN : -FLT_MIN;
    integer_period = fabs(period) * cpu_freq;
    blink_pattern = internal_blink_pattern;
}
