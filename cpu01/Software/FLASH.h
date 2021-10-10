/*
 * Flash.h
 *
 *  Created on: 19.11.2017
 *      Author: Mr.Tea
 */

#ifndef HARDWARE_FLASH_H_
#define HARDWARE_FLASH_H_

#include "F28x_Project.h"
#include "FLASH_API/F021_F2837xD_C28x.h"

enum Sector_enum
{
    SectorA,
    SectorB,
    SectorC,
    SectorD,
    SectorE,
    SectorF,
    SectorG,
    SectorH,
    SectorI,
    SectorJ,
    SectorK,
    SectorL,
    SectorM,
    SectorN
};

class FLASH_class
{
    public:
    Uint16 *address[10];
    Uint16 size16_each[10];
    enum Sector_enum sector;
    Uint16 *sector_address;
    Uint16 *next_address;

    void erase();
    void save();
    Uint16 retrieve(Uint16 offset_from_last = 0);
    Uint16 *find(Uint16 offset_from_last = 0);
};


#endif /* HARDWARE_FLASH_H_ */
