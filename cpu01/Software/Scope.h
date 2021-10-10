/*
 * Scope.h
 *
 *  Created on: 19 cze 2019
 *      Author: Mr.Tea
 */

#ifndef CLA_FILES_SCOPE_H_
#define CLA_FILES_SCOPE_H_

#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCOPE_BUFFER 1250
#define SCOPE_CHANNEL 2
typedef float data_type;

struct Scope_v1{
    data_type data[SCOPE_CHANNEL][SCOPE_BUFFER];
    data_type *data_in[SCOPE_CHANNEL];
    Uint32 index;

    int32 acquire_counter;
    Uint32 acquire_before_counter;

    Uint32 finished_sorting;
    Uint32 acquire_before_trigger;
};

extern struct Scope_v1 Scope;

void Scope_start();
void Scope_trigger(float input, float *input_last, float trigger_val, Uint16 edge);
void Scope_trigger_unc();
void Scope_task();

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CLA_FILES_SCOPE_H_ */
