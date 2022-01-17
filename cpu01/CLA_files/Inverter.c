
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
    INV.Cross0 =  0.75f * Meas.I_inv_0 - 0.25f * Meas.I_inv_1 - 0.25f * Meas.I_inv_2 - 0.25f * Meas.I_inv_3;
    INV.Cross1 = -0.25f * Meas.I_inv_0 + 0.75f * Meas.I_inv_1 - 0.25f * Meas.I_inv_2 - 0.25f * Meas.I_inv_3;
    INV.Cross2 = -0.25f * Meas.I_inv_0 - 0.25f * Meas.I_inv_1 + 0.75f * Meas.I_inv_2 - 0.25f * Meas.I_inv_3;
    INV.Cross3 = -0.25f * Meas.I_inv_0 - 0.25f * Meas.I_inv_1 - 0.25f * Meas.I_inv_2 + 0.75f * Meas.I_inv_3;


   if (!INV.enable || (alarm.all[0] | alarm.all[1]))
   {
       INV.RDY = 0;
       INV.state = INV_first;
       INV.state_last = INV_active;


       GPIO_CLEAR(PWM_H1_ON_CPU);
       GPIO_SET(RELAY_H1_GRID_CPU);
       GPIO_CLEAR(RELAY_H1_DC_CPU);

       GPIO_CLEAR(PWM_H2_ON_CPU);
       GPIO_SET(RELAY_H2_GRID_CPU);
       GPIO_CLEAR(RELAY_H2_DC_CPU);

       GPIO_CLEAR(PWM_H3_ON_CPU);
       GPIO_SET(RELAY_H3_GRID_CPU);
       GPIO_CLEAR(RELAY_H3_DC_CPU);

       GPIO_CLEAR(PWM_H4_ON_CPU);
       GPIO_SET(RELAY_H4_GRID_CPU);
       GPIO_CLEAR(RELAY_H4_DC_CPU);
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
               GPIO_CLEAR(RELAY_H3_DC_CPU);
               GPIO_CLEAR(RELAY_H4_DC_CPU);
               GPIO_SET(RELAY_H1_GRID_CPU);
               GPIO_SET(RELAY_H2_GRID_CPU);
               GPIO_SET(RELAY_H3_GRID_CPU);
               GPIO_SET(RELAY_H4_GRID_CPU);
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
               INV.PR_I_arm0.x0 =
               INV.PR_I_arm0.x1 =
               INV.PR_I_arm1.x0 =
               INV.PR_I_arm1.x1 =
               INV.PR_I_arm2.x0 =
               INV.PR_I_arm2.x1 =
               INV.PR_I_arm3.x0 =
               INV.PR_I_arm3.x1 = 0.0f;

               INV.Resonant_I_a_odd0[0].x0 = INV.Resonant_U_grid.y0;
               INV.Resonant_I_a_odd0[0].x1 = INV.Resonant_U_grid.y1;

               INV.Resonant_I_a_odd1[0].x0 = INV.Resonant_U_grid.y0;
               INV.Resonant_I_a_odd1[0].x1 = INV.Resonant_U_grid.y1;

               INV.Resonant_I_a_odd2[0].x0 = INV.Resonant_U_grid.y0;
               INV.Resonant_I_a_odd2[0].x1 = INV.Resonant_U_grid.y1;

               INV.Resonant_I_a_odd3[0].x0 = INV.Resonant_U_grid.y0;
               INV.Resonant_I_a_odd3[0].x1 = INV.Resonant_U_grid.y1;

               INV.state_last = INV.state;

               GPIO_SET(PWM_H1_ON_CPU);
               GPIO_CLEAR(RELAY_H1_GRID_CPU);
               GPIO_SET(PWM_H2_ON_CPU);
               GPIO_CLEAR(RELAY_H2_GRID_CPU);
               GPIO_SET(PWM_H3_ON_CPU);
               GPIO_CLEAR(RELAY_H3_GRID_CPU);
               GPIO_SET(PWM_H4_ON_CPU);
               GPIO_CLEAR(RELAY_H4_GRID_CPU);
               INV.RDY = 1.0f;

           }


           INV.zero_error[3] =
           INV.zero_error[2] =
           INV.zero_error[1] =
           INV.zero_error[0] = 1.0f;

           if(fabs(INV.duty[0] - 0.5f) > 0.5f) INV.zero_error[0] = 0.0f;
           if(fabs(INV.duty[2] - 0.5f) > 0.5f) INV.zero_error[1] = 0.0f;
           if(fabs(INV.duty[4] - 0.5f) > 0.5f) INV.zero_error[2] = 0.0f;
           if(fabs(INV.duty[6] - 0.5f) > 0.5f) INV.zero_error[3] = 0.0f;

           INV.theta += 50.0f * MATH_2PI * INV.Ts;
           INV.theta -= (float)((int32)(INV.theta * MATH_1_PI)) * MATH_2PI;

           register float I_arm_ref = INV.I_arm_dc_ref + INV.I_arm_ac_ref * cosf(INV.theta);// *(Rect.PI_U_dc.out * Rect.U_dc_filter2) / Rect.U_grid;
           register float I_avg = 0.25f * Meas.I_grid;

           register float input_error3 = (I_arm_ref - I_avg);// (I_arm_ref - Meas.I_inv_3);//
           PI_antiwindup(&INV.PI_I_arm_dc3, input_error3);
           //PR_calc(&INV.PR_I_arm3, input_error3);
           INV.U_ref3 = INV.Kp_I * input_error3;
           INV.U_ref3 += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd3, input_error3 * INV.zero_error[3], INV.resonant_odd_number);

           register float input_error2 = (I_arm_ref - I_avg);// (I_arm_ref - Meas.I_inv_2);//
           PI_antiwindup(&INV.PI_I_arm_dc2, input_error2);
           // PR_calc(&INV.PR_I_arm2, input_error2);
           INV.U_ref2 = INV.Kp_I * input_error2;
           INV.U_ref2 += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd2, input_error2 * INV.zero_error[2], INV.resonant_odd_number);

           register float input_error1 = (I_arm_ref - I_avg);//(I_arm_ref - Meas.I_inv_1);//
           PI_antiwindup(&INV.PI_I_arm_dc1, input_error1);
           //PR_calc(&INV.PR_I_arm1, input_error1);
           INV.U_ref1 = INV.Kp_I * input_error1;
           INV.U_ref1 += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd1, input_error1 * INV.zero_error[1], INV.resonant_odd_number);

           register float input_error0 = (I_arm_ref - I_avg);//(I_arm_ref - Meas.I_inv_0);//
           PI_antiwindup(&INV.PI_I_arm_dc0, input_error0);
           //PR_calc(&INV.PR_I_arm0, input_error0);
           INV.U_ref0 = INV.Kp_I * input_error0;
           INV.U_ref0 += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd0, input_error0 * INV.zero_error[0], INV.resonant_odd_number);

/////////////////////////////////////////////////////////////////////////////////////////////
           register float input_cross0 = (0.0f - INV.Cross0);//(I_arm_ref - Meas.I_inv_0);
           PI_antiwindup_fast(&INV.PI_I_arm_dc0_cross, input_cross0);
           //PR_calc(&INV.PR_I_arm0, input_cross0);
           INV.U_ref0_cross = INV.Kp_I * input_cross0;
           INV.U_ref0_cross += Resonant_mult_calc_CLAasm(INV.Resonant_I_a_odd0_cross, input_cross0 * INV.zero_error_cross[0], INV.resonant_odd_number);

           //register float duty = (U_ref + INV.PI_I_arm_dc.out) / Meas.U_dc_0 ;

           //INV.duty[0] = (U_ref + INV.PI_I_arm_dc.out) / Meas.U_dc_0 + 0.5f;
           //INV.duty[2] = (U_ref2 + INV.PI_I_arm2_dc.out) / Meas.U_dc_1 + 0.5f;//+ INV.PR_I_arm_cross.out;
           INV.duty[0] = (INV.U_ref0+ INV.PI_I_arm_dc0.out ) / fmaxf(Meas.U_dc_0, 1.0f) + 0.5f;
           INV.duty[1] = 1 - INV.duty[0];
           INV.duty[2] = (INV.U_ref1+ INV.PI_I_arm_dc1.out) / fmaxf(Meas.U_dc_1, 1.0f) + 0.5f;
           INV.duty[3] = 1 - INV.duty[2];
           INV.duty[4] = (INV.U_ref2+ INV.PI_I_arm_dc2.out) / fmaxf(Meas.U_dc_2, 1.0f) + 0.5f;
           INV.duty[5] = 1 - INV.duty[4];
           INV.duty[6] = (INV.U_ref3+ INV.PI_I_arm_dc3.out) / fmaxf(Meas.U_dc_3, 1.0f) + 0.5f;
           INV.duty[7] = 1 - INV.duty[6];


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
