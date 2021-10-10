/*
 * Scope.cla
 *
 *  Created on: 19 cze 2019
 *      Author: Mr.Tea
 */

#include "Software/Scope.h"

#pragma DATA_SECTION(Scope,"Scope")
struct Scope_v1 Scope;

#pragma CODE_SECTION(Scope_start, ".TI.ramfunc");
void Scope_start()
{
    Scope.acquire_counter = -1;
}

#pragma CODE_SECTION(Scope_trigger, ".TI.ramfunc");
void Scope_trigger(float input, float *input_last, float trigger_val, Uint16 edge)
{
    if(edge)
    {
        if((*input_last < trigger_val) && (input > trigger_val))
        {
            if(Scope.acquire_counter < 0) Scope.acquire_counter = SCOPE_BUFFER - Scope.acquire_before_trigger;
        }
    }
    else
    {
        if((*input_last > trigger_val) && (input < trigger_val))
        {
            if(Scope.acquire_counter < 0) Scope.acquire_counter = SCOPE_BUFFER - Scope.acquire_before_trigger;
        }
    }
    *input_last = input;
}

#pragma CODE_SECTION(Scope_trigger_unc, ".TI.ramfunc");
void Scope_trigger_unc()
{
    if(Scope.acquire_counter < 0) Scope.acquire_counter = SCOPE_BUFFER - Scope.acquire_before_trigger;
}

#pragma CODE_SECTION(Scope_task, ".TI.ramfunc");
void Scope_task()
{
    static Uint32 swap_counter;
    static Uint32 start_index;
    static int32 index_current;
    static Uint32 new_cycle;
    static data_type memory[SCOPE_CHANNEL];

    if(Scope.acquire_counter)
    {
        Scope.finished_sorting = 0;
        {
            register data_type *scope_data = ((data_type *)Scope.data + Scope.index);
            Uint32 index_temp = 0;
            #pragma UNROLL(SCOPE_CHANNEL)
            #pragma MUST_ITERATE(SCOPE_CHANNEL)
            while(index_temp<SCOPE_CHANNEL)
            {
                memory[index_temp] =
                *(scope_data+(index_temp*SCOPE_BUFFER)) = *Scope.data_in[index_temp];
                index_temp++;
            }
        }
        if(++Scope.index >= SCOPE_BUFFER) Scope.index = 0;
        if(Scope.acquire_counter > 0) Scope.acquire_counter--;
        if(Scope.acquire_before_counter < Scope.acquire_before_trigger)
        {
            Scope.acquire_before_counter++;
            if(Scope.acquire_counter>0)Scope.acquire_counter++;
        }
        swap_counter = 0, start_index = 0, index_current = 0;
        new_cycle = 1;
    }
    else
    {
        Scope.acquire_before_counter = 0;

        if(swap_counter < SCOPE_BUFFER)
        {
            if( (index_current != start_index) || (new_cycle) )
            {
                new_cycle = 0;
                {
                    register data_type * const scope_data = ((data_type *)Scope.data + index_current);
                    Uint32 index_temp = 0UL;

                    #pragma UNROLL(SCOPE_CHANNEL)
                    #pragma MUST_ITERATE(SCOPE_CHANNEL)
                    while(index_temp<SCOPE_CHANNEL)
                    {
                        register data_type temp;
                        temp = *(scope_data+(index_temp*SCOPE_BUFFER));
                        *(scope_data+(index_temp*SCOPE_BUFFER)) = memory[index_temp];
                        memory[index_temp] = temp;
                        index_temp++;
                    }
                }

                index_current -= Scope.index;
                if(index_current < 0) index_current += SCOPE_BUFFER;
                swap_counter++;
            }
            if(index_current == start_index)
            {
                register data_type * const scope_data = ((data_type *)Scope.data + index_current);
                Uint32 index_temp = 0UL;
                #pragma UNROLL(SCOPE_CHANNEL)
                #pragma MUST_ITERATE(SCOPE_CHANNEL)
                while(index_temp<SCOPE_CHANNEL)
                {
                    *(scope_data+(index_temp*SCOPE_BUFFER)) = memory[index_temp];
                    index_temp++;
                }
                start_index++;
                index_current = start_index;
                new_cycle = 1;
            }
        }
        else
        {
            Scope.finished_sorting = 1;
        }
    }
}


//void sort_data()
//{
//    Uint16 swap_counter = 0;
//    Uint16 start_index = 0;
//    int16 index_current;
//
//    float memory1_A;
//    float memory1_B;
//    float memory2_A;
//    float memory2_B;
//
//    while(swap_counter < SCOPE_BUFFER)
//    {
//        index_current = start_index;
//        do
//        {
//            memory1_A = Scope.data1[index_current];
//            Scope.data1[index_current] = memory1_B;
//            memory1_B = memory1_A;
//
//            memory2_A = Scope.data2[index_current];
//            Scope.data2[index_current] = memory2_B;
//            memory2_B = memory2_A;
//
//            index_current = index_current - Scope.index;
//            if(index_current < 0) index_current += SCOPE_BUFFER;
//            swap_counter++;
//        }while(index_current != start_index);
//
//        Scope.data1[index_current] = memory1_B;
//        Scope.data2[index_current] = memory2_B;
//        start_index++;
//    }
//}
