/*
 * FLASH.c
 *
 *  Created on: 18 wrz 2019
 *      Author: Mr.Tea
 */

#include "FLASH.h"

#define SectorA_start        0x80000
#define SectorA_end          0x81FFF

#define SectorB_start        0x82000
#define SectorB_end          0x83FFF

#define SectorC_start        0x84000
#define SectorC_end          0x85FFF

#define SectorD_start        0x86000
#define SectorD_end          0x87FFF

#define SectorE_start        0x88000
#define SectorE_end          0x8FFFF

#define SectorF_start        0x90000
#define SectorF_end          0x97FFF

#define SectorG_start        0x98000
#define SectorG_end          0x9FFFF

#define SectorH_start        0xA0000
#define SectorH_end          0xA7FFF

#define SectorI_start        0xA8000
#define SectorI_end          0xAFFFF

#define SectorJ_start        0xB0000
#define SectorJ_end          0xB7FFF

#define SectorK_start        0xB8000
#define SectorK_end          0xB9FFF

#define SectorL_start        0xBA000
#define SectorL_end          0xBBFFF

#define SectorM_start        0xBC000
#define SectorM_end          0xBDFFF

#define SectorN_start        0xBE000
#define SectorN_end          0xBFFFF

Uint32 Sector_start_address[] =
{
    SectorA_start,
    SectorB_start,
    SectorC_start,
    SectorD_start,
    SectorE_start,
    SectorF_start,
    SectorG_start,
    SectorH_start,
    SectorI_start,
    SectorJ_start,
    SectorK_start,
    SectorL_start,
    SectorM_start,
    SectorN_start
};

Uint32 Sector_end_address[] =
{
    SectorA_end,
    SectorB_end,
    SectorC_end,
    SectorD_end,
    SectorE_end,
    SectorF_end,
    SectorG_end,
    SectorH_end,
    SectorI_end,
    SectorJ_end,
    SectorK_end,
    SectorL_end,
    SectorM_end,
    SectorN_end
};

Uint32 Sector_length16[] =
{
    SectorA_end - SectorA_start + 1,
    SectorB_end - SectorB_start + 1,
    SectorC_end - SectorC_start + 1,
    SectorD_end - SectorD_start + 1,
    SectorE_end - SectorE_start + 1,
    SectorF_end - SectorF_start + 1,
    SectorG_end - SectorG_start + 1,
    SectorH_end - SectorH_start + 1,
    SectorI_end - SectorI_start + 1,
    SectorJ_end - SectorJ_start + 1,
    SectorK_end - SectorK_start + 1,
    SectorL_end - SectorL_start + 1,
    SectorM_end - SectorM_start + 1,
    SectorN_end - SectorN_start + 1
};

Uint32 Sector_length32[] =
{
    (SectorA_end - SectorA_start + 1)/2,
    (SectorB_end - SectorB_start + 1)/2,
    (SectorC_end - SectorC_start + 1)/2,
    (SectorD_end - SectorD_start + 1)/2,
    (SectorE_end - SectorE_start + 1)/2,
    (SectorF_end - SectorF_start + 1)/2,
    (SectorG_end - SectorG_start + 1)/2,
    (SectorH_end - SectorH_start + 1)/2,
    (SectorI_end - SectorI_start + 1)/2,
    (SectorJ_end - SectorJ_start + 1)/2,
    (SectorK_end - SectorK_start + 1)/2,
    (SectorL_end - SectorL_start + 1)/2,
    (SectorM_end - SectorM_start + 1)/2,
    (SectorN_end - SectorN_start + 1)/2
};

union Bufor {
    Uint64 bit64[2];
    Uint32 bit32[4];
    Uint16 bit16[8];
    struct List_elements_struct
    {
        Uint32 magic;
        union Bufor *next;
        union Bufor *previous;
        Uint32 size16;
    }list;
};

const Uint32 magic_value32 = 0xAAAA5555;

#pragma CODE_SECTION(".TI.ramfunc");
void FLASH_class::erase()
{
    SeizeFlashPump();

    EALLOW;
    Fapi_initializeAPI(F021_CPU0_W0_BASE_ADDRESS, 200);
    Fapi_setActiveFlashBank(Fapi_FlashBank0);
    EDIS;

    Uint16 erase_count = 0;
    Fapi_FlashStatusWordType fapi_statusword;
    while(1)
    {
        Fapi_StatusType fapi_status = Fapi_doBlankCheck((Uint32 *)Sector_start_address[sector], Sector_length32[sector], &fapi_statusword);

        if(fapi_status == Fapi_Status_Success || erase_count++ > 2)
        {
        ReleaseFlashPump();
            return;
        }

        EALLOW;
        Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,(Uint32 *)Sector_start_address[sector]);
        while(Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
        EDIS;
    }
}

#pragma CODE_SECTION(".TI.ramfunc");
void FLASH_class::save()
{
    union Bufor bufor_RAM;
    union Bufor *bufor_FLASH;

    Uint16 elements = 0;
    Uint16 size16 = 0;
    while(size16_each[elements]) size16 += size16_each[elements++];

    Uint16 size128 = size16>>3;
    if(size16 & 0x7) size128++;

    bufor_RAM.list.magic = magic_value32;
    bufor_RAM.list.size16 = size16;

    bufor_FLASH = (union Bufor *)find();
    if(bufor_FLASH != NULL)
    {
        bufor_RAM.list.previous = bufor_FLASH;
        bufor_FLASH = bufor_FLASH->list.next;
        bufor_RAM.list.next = bufor_FLASH + size128 + 1;
    }

    if(bufor_FLASH == NULL ||  bufor_RAM.list.next > (union Bufor *)Sector_end_address[sector])
    {
        erase();
        bufor_FLASH = (union Bufor *)Sector_start_address[sector];
        bufor_RAM.list.previous = bufor_FLASH;
        bufor_RAM.list.next = bufor_FLASH + size128 + 1;
    }

    SeizeFlashPump();
    EALLOW;
    Fapi_initializeAPI(F021_CPU0_W0_BASE_ADDRESS, 200);
    Fapi_setActiveFlashBank(Fapi_FlashBank0);

    Fapi_issueProgrammingCommand((Uint32 *)bufor_FLASH,bufor_RAM.bit16,8,0,0,Fapi_AutoEccGeneration);
    while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
    bufor_FLASH++;

    Uint32 size16_counter = 0;
    Uint16 element = 0;
    Uint16 *address_source = address[0];
    while(size16)
    {
        bufor_RAM.bit32[0] =
        bufor_RAM.bit32[1] =
        bufor_RAM.bit32[2] =
        bufor_RAM.bit32[3] = ~0UL;
        Uint16 i = 0;
        while(size16 && i < 8)
        {
            bufor_RAM.bit16[i++] = *address_source++;
            size16--;

            if(++size16_counter >= size16_each[element])
            {
                size16_counter = 0;
                address_source = address[++element];
            }
        }

        Fapi_issueProgrammingCommand((Uint32 *)bufor_FLASH,bufor_RAM.bit16,8,0,0,Fapi_AutoEccGeneration);
        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
        bufor_FLASH++;
    }
    EDIS;
    ReleaseFlashPump();
}

Uint16 FLASH_class::retrieve(Uint16 offset_from_last)
{
    Uint16 *bufor_FLASH = find(offset_from_last);

    if(bufor_FLASH == NULL) return 1;

    Uint16 element = 0;
    bufor_FLASH += 8;
    while(size16_each[element])
    {
        memcpy(address[element], bufor_FLASH, size16_each[element]);
        bufor_FLASH += size16_each[element++];
    }
    return 0;
}

Uint16 *FLASH_class::find(Uint16 offset_from_last)
{
    union Bufor *bufor_FLASH = (union Bufor *)(Sector_start_address[sector]);
    sector_address = (Uint16 *)bufor_FLASH;

    if(bufor_FLASH->list.magic != magic_value32) return NULL;

    Uint16 elements = 0;
    Uint16 size16 = 0;
    while(size16_each[elements]) size16 += size16_each[elements++];

    register union Bufor *sector_start = (union Bufor *)Sector_start_address[sector];
    register union Bufor *sector_end = (union Bufor *)Sector_end_address[sector];
    while(1)
    {
        register union Bufor *bufor_FLASH_temp;
        bufor_FLASH_temp = bufor_FLASH->list.next;
        if(bufor_FLASH_temp < sector_start) return NULL;
        if(bufor_FLASH_temp > sector_end) return NULL;
        if((bufor_FLASH_temp->bit32[0] == ~0UL) && (bufor_FLASH_temp->bit32[1] == ~0UL) &&
           (bufor_FLASH_temp->bit32[2] == ~0UL) && (bufor_FLASH_temp->bit32[3] == ~0UL)) break;
        if(bufor_FLASH_temp->list.magic != magic_value32) return NULL;
        bufor_FLASH = bufor_FLASH_temp;
    }

    next_address = (Uint16 *)bufor_FLASH->list.next;

    while(offset_from_last--)
    {
        bufor_FLASH = bufor_FLASH->list.previous;
    }

    if(bufor_FLASH->list.size16 != size16) return NULL;

    return (Uint16 *)(bufor_FLASH);
}
