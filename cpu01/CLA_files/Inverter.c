
#include "stdafx.h"

void Inverter_calc()
{
     INV.counter ++;
     if (INV.counter > 100000.0f)
     {
         GPIO_TOGGLE(RELAY_H1_DC_CPU);
         INV.counter = 0;
     }
}
