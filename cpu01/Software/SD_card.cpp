/*
 * SD_card.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */
#include <stdafx.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include "ff.h"
#include "diskio.h"
#include <Scope.h>
#include <SD_card.h>
#include <State.h>

class SD_card_class SD_card;

struct calibration_struct SD_card_class::calibration;
struct settings_struct SD_card_class::settings;
FRESULT SD_card_class::fresult = FR_OK;
char SD_card_class::working_buffer[WBUF_SIZE];
char SD_card_class::filename_buffer[WBUF_SIZE];

Uint16 *SD_card_class::byte_expand2word(Uint16 *word_array, Uint16 *byte_array, Uint16 byte_offset, Uint16 size)
{
    register Uint16 *destination_pointer = word_array;
    Uint16 *destination_end_pointer = destination_pointer + size;

    register int16 *source_pointer = (int16 *)byte_array;

    while(destination_pointer < destination_end_pointer)
    {
        *destination_pointer++ =  __byte(source_pointer, byte_offset++);
    }

    return destination_pointer;
}

void SD_card_class::word_truncate2byte(Uint16 *byte_array, Uint16 *word_array, Uint16 byte_offset, Uint16 size)
{
    register int16 *destination_pointer = (int16 *)byte_array;
    register Uint16 *source_pointer = word_array;
    Uint16 byte_offset_max = byte_offset + size;

    while(byte_offset < byte_offset_max)
    {
        __byte(destination_pointer, byte_offset++) = *source_pointer++;
    }
}

Uint16 *SD_card_class::byte_expand2word2(Uint16 *word_array, Uint16 *byte_array, Uint16 size)
{
    register Uint16 *destination_pointer = word_array;
    register Uint16 *source_pointer = byte_array;

    for(Uint16 i = 0;i<size;i++)
    {
        *destination_pointer++ = __byte((int *)source_pointer,0);
        *destination_pointer++ = __byte((int *)source_pointer++,1);
    }
    return source_pointer;
}

void SD_card_class::dump_memory(FIL* fp, Uint16 *source_address, Uint32 size)
{
    const Uint16 buffer_size = 100;
    static Uint16 buffer[buffer_size];
    Uint16 bw;

    while(size)
    {
        Uint32 btw = size > buffer_size/2 ? buffer_size/2 : size;
        size -= btw;
        source_address = byte_expand2word2(buffer, source_address, btw);
        fresult = f_write(fp, buffer, btw*2, &bw);
    }
}

void SD_card_class::save_float_table(FIL* fp, Uint16 max_columns, Uint16 max_rows, ...)
{
//    ESTOP0;
    va_list vl;
    va_start(vl, max_rows);

    float **columns;
    columns = (float **) malloc(max_columns * sizeof(float *));
    if (columns==NULL) abort();

    for (Uint16 i = 0;i<max_columns;i++)
    {
        columns[i] = va_arg(vl,float *);
    }
    va_end(vl);

    for(Uint16 i = 0; i < max_rows;i++)
    {
        for (Uint16 j = 0;j<max_columns;j++)
        {
            snprintf(working_buffer, WBUF_SIZE, "%g", *(columns[j]++));
            f_puts(working_buffer, fp);
            f_putc(';', fp);
        }
        f_lseek(fp, f_tell(fp) - 1);
        f_putc('\n', fp);
    }
    free(columns);
}


Uint16 SD_card_class::read_float_table(FIL* fp, Uint16 max_columns, Uint16 max_rows, ...)
{
    va_list vl;
    va_start(vl, max_rows);

    float **columns;
    columns = (float **) malloc(max_columns * sizeof(float *));
    if (columns==NULL) abort();

    for (Uint16 i = 0;i<max_columns;i++)
    {
        columns[i] = va_arg(vl,float *);
    }
    va_end(vl);
    Uint16 i;
    for(i = 0; i < max_rows;i++)
    {
        if(!f_gets(working_buffer, WBUF_SIZE, fp)) break;
        char *pch = working_buffer;
        for (Uint16 j = 0;j<max_columns;j++)
        {
            sscanf(pch,"%g", columns[j]++);
            pch = strchr(pch,';');
            if(pch == NULL) break;
            pch += 1;
        }
    }
    free(columns);
    return i;
}

Uint16 SD_card_class::read_settings()
{
    memset(&SD_card.settings, 0, sizeof(SD_card.settings));

    FIL fil;

    if(fresult = f_open(&fil, "settings.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    while(f_gets(working_buffer, WBUF_SIZE, &fil))
    {
        char * pch = working_buffer;
        while(*pch) *pch++ = toupper(*pch);

        pch = strchr(working_buffer,';') + 1;
        float value;
        sscanf(pch,"%g", &value);

        if(!strncmp(working_buffer, "BAUDRATE", sizeof("BAUDRATE")-1))
            settings.Baudrate = value;
    }
    if(fresult = f_close(&fil)) return fresult;

    settings.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_settings()
{
    FIL fil;
    if(fresult = f_open(&fil, "settings.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    f_puts("BAUDRATE;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.Baudrate);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    fresult = f_close(&fil);
    return fresult;
}

Uint16 SD_card_class::read_calibration_data()
{
    memset(&SD_card.calibration, 0, sizeof(SD_card.calibration));

    FIL fil;
    if(fresult = f_open(&fil, "calib.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    {
        Uint16 br;
        Uint16 buff = 0;
        while(buff != '\n')
        {
            if(fresult = f_read(&fil, &buff, 1, &br))
            {
                f_close(&fil);
                return fresult;
            }
        }
    }

    const Uint16 no_columns = 1;

    read_float_table(&fil, no_columns, sizeof(struct Measurements_gain_offset_struct)/sizeof(float), &calibration.Meas_gain);

    {
        Uint16 br;
        Uint16 buff = 0;
        while(buff != '\n')
        {
            if(fresult = f_read(&fil, &buff, 1, &br))
            {
                f_close(&fil);
                return fresult;
            }
        }
    }
    read_float_table(&fil, no_columns, sizeof(struct Measurements_gain_offset_struct)/sizeof(float), &calibration.Meas_offset);

    if(fresult = f_close(&fil)) return fresult;

    calibration.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_calibration_data()
{
    FIL fil;
    if(fresult = f_open(&fil, "calib.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    f_puts("struct Measurements Meas_gain\n", &fil);

    const Uint16 no_columns = 1;
    save_float_table(&fil, no_columns, sizeof(struct Measurements_gain_offset_struct)/sizeof(float), &calibration.Meas_gain);
    f_puts("struct Measurements Meas_offset\n", &fil);
    save_float_table(&fil, no_columns, sizeof(struct Measurements_gain_offset_struct)/sizeof(float), &calibration.Meas_offset);

    fresult = f_close(&fil);
    return fresult;
}
