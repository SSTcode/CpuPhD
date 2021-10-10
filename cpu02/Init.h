/*
 * Hardware.h
 *
 *  Created on: 17.11.2017
 *      Author: Mr.Tea
 */

#ifndef INIT_H_
#define INIT_H_

class Init_class
{
public:
    void PWM_timestamp(volatile struct EPWM_REGS *EPwmReg);
    void PWMs();
    void CLA();
    void Variables();
};

extern class Init_class Init;

#endif /* INIT_H_ */
