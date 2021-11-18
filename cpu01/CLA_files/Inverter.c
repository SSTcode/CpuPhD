
#include "stdafx.h"

void Inverter_calc()
{
   //if (!INV.enable ||alINV.all[0] | alINV.all[1])
   //{
     // GPIO_SET(RELAY_H1_DC_CPU);// r conect to dc bus
//  GPIO_SET(RELAY_H1_GRID_CPU);// desconect
   //
//    GPIO_CLEAR(RELAY_H1_DC_CPU);// LED_ON r deconect to dc bus. Same point ac
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
       GPIO_CLEAR(PWM_H2_ON_CPU);
       GPIO_SET(RELAY_H1_DC_CPU);//R conect DC. LED OFF
       GPIO_SET(RELAY_H2_DC_CPU);
       GPIO_SET(RELAY_H1_GRID_CPU);// phase open. LED OFF
       GPIO_SET(RELAY_H2_GRID_CPU);
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
               GPIO_CLEAR(RELAY_H1_DC_CPU);//R conect phase. LED ON
               GPIO_CLEAR(RELAY_H2_DC_CPU);
               GPIO_CLEAR(RELAY_H1_GRID_CPU);// phase close. LED ON
               GPIO_CLEAR(RELAY_H2_GRID_CPU);
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
               INV.PR_I_arm2.x0 =
               INV.PR_I_arm2.x1 = 0.0f;
               INV.PR_I_arm_cross.x0 =
               INV.PR_I_arm_cross.x1 = 0.0f;
               INV.PR_I_arm2_cross.x0 =
               INV.PR_I_arm2_cross.x1 = 0.0f;

               INV.Resonant_I_a_odd[0].x0 = INV.Resonant_U_grid.y0;
               INV.Resonant_I_a_odd[0].x1 = INV.Resonant_U_grid.y1;

               GPIO_SET(PWM_H1_ON_CPU);
               GPIO_SET(PWM_H2_ON_CPU);
               INV.RDY = 1.0f;

           }

           INV.theta += 50.0f * MATH_2PI * INV.Ts;
           INV.theta -= (float)((int32)(INV.theta * MATH_1_PI))*MATH_2PI;

           register float I_arm_ref = INV.I_arm_ref * cosf(INV.theta)+ INV.I_arm_dc_ref;// *(Rect.PI_U_dc.out * Rect.U_dc_filter2) / Rect.U_grid;
           //register float duty = INV.I_arm_ref * cosf(INV.theta);

           INV.I_cross_0= 0.5f*Meas.I_inv_0-0.5f*Meas.I_inv_1;
           INV.I_cross_1= -0.5f*Meas.I_inv_0+0.5f*Meas.I_inv_1;

           register float input_error_cross = -0.0f + INV.I_cross_0;
           register float input_error2_cross = -0.0f + INV.I_cross_1;

           register float input_error = (-I_arm_ref + Meas.I_inv_0);
           PI_antiwindup(&INV.PI_I_arm_dc, input_error);
           float U_ref = INV.Kp_I * input_error;
           U_ref += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd, input_error * INV.zero_error, INV.resonant_odd_number);

           register float input_error2 = (-I_arm_ref + Meas.I_inv_1);
           PI_antiwindup(&INV.PI_I_arm2_dc, input_error2);
           float U_ref2 = INV.Kp_I * input_error2;
           U_ref2 += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd, input_error2 * INV.zero_error, INV.resonant_odd_number);

           //PR_calc(&INV.PR_I_arm, input_error);
           //PR_calc(&INV.PR_I_arm2, input_error2);
           //PI_antiwindup(&INV.PI_I_arm_dc_cross, input_error_cross);
           //PI_antiwindup(&INV.PI_I_arm2_dc_cross, input_error2_cross);
           PR_calc(&INV.PR_I_arm_cross, input_error_cross);
           PR_calc(&INV.PR_I_arm2_cross, input_error2_cross);


          // INV.duty[0]= (INV.PR_I_arm.out ) / Meas.U_dc_0 + 0.5f;
           //INV.duty[2]= (INV.PR_I_arm2.out) / Meas.U_dc_1 + 0.5f ;//+ INV.PR_I_arm_cross.out;



                  INV.duty[0]= (U_ref + INV.PI_I_arm_dc.out) / Meas.U_dc_0 + 0.5f  ;
                  INV.duty[2]= (U_ref2 + INV.PI_I_arm2_dc.out ) / Meas.U_dc_1 + 0.5f ;//+ INV.PR_I_arm_cross.out;
                  INV.duty[1]=1-INV.duty[0];
                  INV.duty[3]=1-INV.duty[2];



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
