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

#define SD_SPICLK_PIN 22
#define SD_SPISTE_PIN 23
#define SD_SPISIMO_PIN 24
#define SD_SPISOMI_PIN 25

#define PWM_SYNC 14

#endif

#endif /* IO_H_ */
