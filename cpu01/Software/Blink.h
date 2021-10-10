/*
 * Blink.h
 *
 *  Created on: 18 wrz 2020
 *      Author: Mr.Tea
 */

#ifndef BLINK_H_
#define BLINK_H_

#include <stdafx.h>
#include <float.h>

class Blink_class
{
    public:
    bool output_state;
    bool zero_crossing;
    bool task();
    bool task_simple();
    void reset_counter();
    Blink_class(float period, float pattern[])
    {
        reset_counter();
        update_pattern(period, pattern);
    }
    Blink_class(bool val)
    {
        reset_counter();
        update_pattern(val);
    }
    Blink_class(float period)
    {
        reset_counter();
        update_pattern(period);
    }
    Blink_class(float period, float duty)
    {
        reset_counter();
        update_pattern(period, duty);
    }
    void update_pattern(float period, float pattern[]);
    void update_pattern(bool val);
    void update_pattern(float period, float duty);
    void update_pattern(float period);

    private:
    static float cpu_freq;
    static float div_cpu_freq;
    float *blink_pattern;
    float internal_blink_pattern[15];
    Uint16 pattern_cursor;
    Uint32 integer_period;
    Uint32 external_counter_old;
    Uint32 internal_counter;
};

#endif /* BLINK_H_ */
