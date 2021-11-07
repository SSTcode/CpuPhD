
#include "stdafx.h"

void Inverter_calc()
{
   //if (!INV.enable ||alINV.all[0] | alINV.all[1])
   //{
     // GPIO_SET(RELAY_H1_DC_CPU);// r conect to dc bus
//  GPIO_SET(RELAY_H1_GRID_CPU);// desconect
   //
//    GPIO_CLEAR(RELAY_H1_DC_CPU);// LED_ON r deconect to dc bus same point ac
      //GPIO_CLEAR(RELAY_H1_GRID_CPU);// LED_on  conect phase
   //

   //}

   if (!INV.enable || (alarm.all[0] | alarm.all[1]))
   {
       INV.RDY = 0;
       INV.state = INV_first;
       INV.state_last = INV_active;

       Meas_alarm_L.U_dc = -5.0f;
       GPIO_CLEAR(PWM_H1_ON_CPU);
       GPIO_CLEAR(RELAY_H1_GRID_CPU);
       GPIO_CLEAR(RELAY_H1_DC_CPU);
   }
   else
   {
       switch (INV.state)
       {

       case INV_first:
       {
           static float counter;
           if (INV.state_last != INV.state)
           {
               INV.state_last = INV.state;
               counter = 0.0f;
               GPIO_SET(RELAY_H1_GRID_CPU);

               //Meas_alarm_L.U_dc = 5.0f;
           }
           counter += INV.Ts;
           if (counter > 20e-3)
               INV.state++;
       break;
       }

       case INV_active:
       {

           if (INV.state_last != INV.state)
           {
               INV.state_last = INV.state;
               //resonant init
               INV.PR_I_arm.x0 =
               INV.PR_I_arm.x1 = 0.0f;
               GPIO_SET(PWM_H1_ON_CPU);
               INV.RDY = 1.0f;
           }

           INV.theta += 50.0f * MATH_2PI * INV.Ts;
           INV.theta -= (float)((int32)(INV.theta * MATH_1_PI))*MATH_2PI;

            INV.I_arm_ref = 0.5f;// *(Rect.PI_U_dc.out * Rect.U_dc_filter2) / Rect.U_grid;
            register float duty = INV.I_arm_ref * cosf(INV.theta);

           register float input_error = INV.I_arm_ref - Meas.I_inv_0;
                PR_calc(&INV.PR_I_arm, input_error);

              //register float duty= INV.PR_I_arm.out / Meas.U_dc_0;

              INV.duty[0]=duty+0.5;
              INV.duty[1]=1-INV.duty[0];

           break;
       }
       default:
       {
           //alINV.bit.INV_ERR = 1;
           break;
       }
       }//end SW
   }
}
