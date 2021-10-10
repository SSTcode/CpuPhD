/*
 * IO.h
 *
 *  Created on: 22 mar 2020
 *      Author: MrTea
 */

#ifndef IO_H_
#define IO_H_

#ifndef __ASM_HEADER__

enum GPIO_state_enum
{
    LOW,
    HIGH,
};

enum GPIO_mux_enum
{
    MUX0,
    MUX1,
    MUX2,
    MUX3,
    MUX4,
    MUX5,
    MUX6,
    MUX7,
    MUX8,
    MUX9,
    MUX11,
    MUX12,
    MUX13,
    MUX14,
    MUX15
};

enum GPIO_cpucla_enum
{
    CPU1_IO,
    CPU1CLA_IO,
    CPU2_IO,
    CPU2CLA_IO
};

enum GPIO_dir_enum
{
    INPUT,
    OUTPUT
};

enum GPIO_options_enum
{
    PUSHPULL = (0 << 0),
    PULLUP = (1 << 0),
    INVERT = (1 << 1),
    SYNC  = (0 << 4),
    QUAL3 = (1 << 4),
    QUAL6 = (2 << 4),
    ASYNC = (3 << 4),
};

struct GPIO_struct
{
    enum GPIO_state_enum defval;
    enum GPIO_mux_enum mux;
    enum GPIO_cpucla_enum cpucla;
    enum GPIO_dir_enum dir;
    Uint16 options;
};

extern const struct GPIO_struct GPIOreg[169];

#define GPIO_Setup(i)                                  \
GPIO_WritePin(i, GPIOreg[i].defval);                      \
GPIO_SetupPinMux(i, GPIOreg[i].cpucla, GPIOreg[i].mux);      \
GPIO_SetupPinOptions(i, GPIOreg[i].dir, GPIOreg[i].options)

#define GPIO_SET(pin) *(&GpioDataRegs.GPASET.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_CLEAR(pin) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_TOGGLE(pin) *(&GpioDataRegs.GPATOGGLE.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_READ(pin) ((*(&GpioDataRegs.GPADAT.all + (pin / 32)*GPY_DATA_OFFSET) >> (pin % 32) & 1UL))
#define GPIO_WRITE(pin, val) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET - val) = 1UL << (pin % 32)
#define GPIO_SET2(pin, val) *(&GpioDataRegs.GPASET.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)
#define GPIO_CLEAR2(pin, val) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)
#define GPIO_TOGGLE2(pin, val) *(&GpioDataRegs.GPATOGGLE.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)

#define EM1D0   85
#define EM1D1   83
#define EM1D2   82
#define EM1D3   81
#define EM1D4   80
#define EM1D5   79
#define EM1D6   78
#define EM1D7   77
#define EM1D8   76
#define EM1D9   75
#define EM1D10  74
#define EM1D11  73
#define EM1D12  72
#define EM1D13  71
#define EM1D14  70
#define EM1D15  69

#define EM1CS2  34
#define EM1WE   31
#define EM1OE   37

#define EM1BA1  92
#define EM1A0   38
#define EM1A1   39
#define EM1A2   40
#define EM1A3   41
#define EM1A4   44
#define EM1A5   45
#define EM1A6   46
#define EM1A7   47
#define EM1A8   48
#define EM1A9   49
#define EM1A10  50
#define EM1A11  51

#define PWM1A 145
#define PWM1B 146
#define PWM2A 147
#define PWM2B 148
#define PWM3A 149
#define PWM3B 150
#define PWM4A 151
#define PWM4B 152
#define PWM5A 153
#define PWM5B 154
#define PWM6A 155
#define PWM6B 156
#define PWM7A 157
#define PWM7B 158
#define PWM8A 159
#define PWM8B 160
#define PWM9A 161
#define PWM9B 162
#define PWM10A 163
#define PWM10B 164
#define PWM11A 165
#define PWM11B 166
#define PWM12A 167
#define PWM12B 168

//CPU IO

#define SD_SPICLK_PIN 102
#define SD_SPISTE_PIN 103
#define SD_SPISIMO_PIN 100
#define SD_SPISOMI_PIN 101

#define RELAY_H1_DC_CPU 0
#define RELAY_H2_DC_CPU 2
#define RELAY_H3_DC_CPU 4
#define RELAY_H4_DC_CPU 6

#define RELAY_H1_GRID_CPU 1
#define RELAY_H2_GRID_CPU 3
#define RELAY_H3_GRID_CPU 5
#define RELAY_H4_GRID_CPU 7

#define PWM_H1_ON_CPU 17
#define PWM_H2_ON_CPU 18
#define PWM_H3_ON_CPU 20
#define PWM_H4_ON_CPU 22

#define PWM_SYNC_CPU 21
#define TZ_EN_CPU PWM9A

#endif

#endif /* IO_H_ */
