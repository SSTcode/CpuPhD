/*
 * SD_card.h
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#ifndef SD_CARD_H_
#define SD_CARD_H_

#include <stdafx.h>
#include "ff.h"
#include "State.h"

class SD_card_class
{
    public:

    static struct calibration_struct calibration;
    static struct settings_struct settings;

    static Uint16 save_settings();
    static Uint16 read_settings();
    static Uint16 save_calibration_data();
    static Uint16 read_calibration_data();

    private:

    static FRESULT fresult;
    static char working_buffer[WBUF_SIZE];
    static char filename_buffer[WBUF_SIZE];

    static Uint16 *byte_expand2word(Uint16 *destination_addres, Uint16 *source_address, Uint16 byte_offset, Uint16 size);
    static void word_truncate2byte(Uint16 *destination_addres, Uint16 *source_address, Uint16 byte_offset, Uint16 size);
    static Uint16 *byte_expand2word2(Uint16 *destination_addres, Uint16 *source_address, Uint16 size);
    static void dump_memory(FIL* fp, Uint16 *source_address, Uint32 size);
    static void save_float_table(FIL* fp, Uint16 max_columns, Uint16 max_rows, ...);
    static Uint16 read_float_table(FIL* fp, Uint16 max_columns, Uint16 max_rows, ...);
};

extern class SD_card_class SD_card;

#endif /* SD_CARD_H_ */
