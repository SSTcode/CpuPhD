//Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_STATE_H_
#define SOFTWARE_STATE_H_

#define WBUF_SIZE 100
#define CT_CHARACTERISTIC_POINTS 60

struct calibration_struct
{
    Uint16 available;
    struct Measurements_gain_offset_struct Meas_gain;
    struct Measurements_gain_offset_struct Meas_offset;
};

struct settings_struct
{
    Uint16 available;
    union CONTROL control;
    float Baudrate;
};

enum Machine_state_enum
{
    Machine_init,
    Machine_idle,
    Machine_start,
    Machine_operational,
    Machine_cleanup,
    __dummybig_Machine = 300000
};

struct Machine_struct
{
    Uint16 save_to_RTC;
    enum Machine_state_enum state, state_last;
};


extern struct Machine_struct Machine;

void SMachine_Main();
void SMachine_Background();

#endif /* SOFTWARE_STATE_H_ */
