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
    void clear_alarms();
    void GPIO();
    void CPUS();
    void CLA();
    void EMIF();
    void IPCBootCPU2_flash();
    void Variables();
    void EPwm_TZclear(volatile struct EPWM_REGS *EPwmReg);
    void EPwm(volatile struct EPWM_REGS *EPwmReg);
    void TZ_EN(volatile struct EPWM_REGS *EPwmReg);
    void PWM_timestamp(volatile struct EPWM_REGS *EPwmReg);
    void PWMs();
};

extern class Init_class Init;

#endif /* INIT_H_ */
