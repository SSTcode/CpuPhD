/*-----------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2007                   */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

/*
 * This file was modified from a sample available from the FatFs
 * web site. It was modified to work with a F2837xD device.
 */

#include "F28x_Project.h"
#include "ff.h"
#include "diskio.h"
#include "IO.h"

/* Timer configurations */
static volatile
DWORD Timer1, Timer2;

struct FatFS_time_struct FatFS_time;

#define CYCLES_ms 200000UL
#define TIMESTAMP(x) x = IpcRegs.IPCCOUNTERL
#define TIMEOUT_ms(x, y) (IpcRegs.IPCCOUNTERL - x > (Uint32)y*CYCLES_ms)

/* SPI configurations */
enum spi_module_enum
{
    SPI_A,
    SPI_B,
    SPI_C
};
const enum spi_module_enum spi_module = SPI_C;

#define SpiRegs (*(&SpiaRegs+spi_module))

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD13    (0x40+13)    /* SD_STATUS (SDC) */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD32    (0x40+32)    /* ERASE_ER_BLK_START */
#define CMD33    (0x40+33)    /* ERASE_ER_BLK_END */
#define CMD38    (0x40+38)    /* ERASE */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

typedef enum { FALSE = 0, TRUE } BOOL;

static inline void wait_for_empty_txfifo(void){
    while( SpiRegs.SPIFFTX.bit.TXFFST != 0);//odczekanie na wyslanie FIFO (przy starcie sa wysylane FFy)
}

static inline void SELECT(void) {
    wait_for_empty_txfifo();
    *(&GpioDataRegs.GPACLEAR.all + (SD_SPISTE_PIN / 32)*GPY_DATA_OFFSET) = 1UL << (SD_SPISTE_PIN % 32);
}

static inline void DESELECT(void) {
    wait_for_empty_txfifo();
    *(&GpioDataRegs.GPASET.all + (SD_SPISTE_PIN / 32)*GPY_DATA_OFFSET) = 1UL << (SD_SPISTE_PIN % 32);

}

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile
DSTATUS Stat = STA_NOINIT;    /* Disk status */

static
BYTE CardType;            /* b0:MMC, b1:SDC, b2:Block addressing */

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

static
void xmit_spi (BYTE dat)    //EDITED
{
    volatile DWORD rcvdat;                      //included file like integer.h for DWORD definition

    /* Write the data to the tx fifo */
    while(SpiRegs.SPISTS.bit.BUFFULL_FLAG);    //Wait for room to write data
    SpiRegs.SPITXBUF = ((DWORD)dat)<<8;        //Write data

    /* flush data read during the write */
    while(SpiRegs.SPISTS.bit.INT_FLAG !=1);    //May be possible to switch to '!SpiRegs.SPISTS.bit.INT_FLAG'
    rcvdat = (SpiRegs.SPIRXBUF && 0xFF);       //Clear Receive Buffer
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi (void)        //EDITED
{
    volatile DWORD rcvdat;

    //Disable transmission channel
    //SpiRegs.SPICTL.bit.TALK = 0;

    /* write dummy data */
    while(SpiRegs.SPISTS.bit.BUFFULL_FLAG);    //Wait for space to write
    SpiRegs.SPITXBUF = 0xFF00;                 //Write dummy data

    /* read data from RX fifo */
    while(SpiRegs.SPISTS.bit.INT_FLAG !=1);    //May be possible to switch to '!SpiRegs.SPISTS.bit.INT_FLAG'
    rcvdat = (SpiRegs.SPIRXBUF & 0xFF);        //Read Transferred data

    return (BYTE)rcvdat;
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
    BYTE res;

    TIMESTAMP(Timer2);
    rcvr_spi();
    do{
        res = rcvr_spi();
    }   while ((res != 0xFF) && !TIMEOUT_ms(Timer2, 500));

    return res;
}

/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
void power_on (void)
{
    EALLOW;
    CpuSysRegs.PCLKCR8.all |= (1<<spi_module);
    EDIS;

    GPIO_Setup(SD_SPISIMO_PIN);
    GPIO_Setup(SD_SPISOMI_PIN);
    GPIO_Setup(SD_SPICLK_PIN);
    GPIO_Setup(SD_SPISTE_PIN);

    /* Configure the SPI C port */
    SpiRegs.SPICCR.bit.SPISWRESET = 0;         //Set reset bit low
    SpiRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiRegs.SPICTL.bit.MASTER_SLAVE = 1;       //Master mode
    SpiRegs.SPIBRR.all = 127;  //390kHz                     //update value to proper setting for correct bitrate ( current: ~500kHz)
    SpiRegs.SPICCR.bit.SPICHAR = 0x7;          //Set char length to 8 bits
    SpiRegs.SPICTL.bit.TALK = 1;
    SpiRegs.SPICCR.bit.SPISWRESET = 1;         //Release SPI from reset
    SpiRegs.SPIPRI.bit.FREE = 1;
    SpiRegs.SPIPRI.bit.SOFT = 1;
}

static
void set_max_speed(void)            //EDIT
{
    SpiRegs.SPIBRR.all = 3; // <- (No enforced limit)
}

static
void power_off (void)
{

}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BOOL rcvr_datablock (
    BYTE *buff,            /* Data buffer to store received data */
    UINT btr            /* Byte count (must be even number) */
)
{
    BYTE token;

    TIMESTAMP(Timer2);
    do token = rcvr_spi();
    while ((token == 0xFF) && !TIMEOUT_ms(Timer2, 100));

    if(token != 0xFE) return FALSE;    /* If not valid data token, return with error */

    do                              /* Receive the data block into buffer */
    {
        *buff++ = rcvr_spi();
        *buff++ = rcvr_spi();
    }
    while (btr -= 2);
    rcvr_spi();                        /* Discard CRC */
    rcvr_spi();

    return TRUE;                    /* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

static
BOOL xmit_datablock (
    const BYTE *buff,    /* 512 byte data block to be transmitted */
    BYTE token            /* Data/Stop token */
)
{
    BYTE resp, wc;

    xmit_spi(token);                    /* Xmit data token */
    if (token != 0xFD)      /* Is data token */
    {
        wc = 256;
        do                              /* Xmit the 512 byte data block to MMC */
        {
            xmit_spi(*buff++);
            xmit_spi(*buff++);
        }
        while (--wc);
        xmit_spi(0xFF);                    /* CRC (Dummy) */
        xmit_spi(0xFF);
        resp = rcvr_spi();                /* Reveive data response */
        if ((resp & 0x1F) != 0x05)        /* If not accepted, return with error */
            return FALSE;
    }

    return TRUE;
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
    BYTE cmd,        /* Command byte */
    DWORD arg        /* Argument */
)
{
    BYTE n, res;
    if (wait_ready() != 0xFF) return 0xFF;
    /* Send command packet */
    xmit_spi(cmd);                        /* Command */
    xmit_spi((BYTE)(arg >> 24));        /* Argument[31..24] */
    xmit_spi((BYTE)(arg >> 16));        /* Argument[23..16] */
    xmit_spi((BYTE)(arg >> 8));            /* Argument[15..8] */
    xmit_spi((BYTE)arg);                /* Argument[7..0] */
    n = 0;
    if (cmd == CMD0) n = 0x95;            /* CRC for CMD0(0) */
    if (cmd == CMD8) n = 0x87;            /* CRC for CMD8(0x1AA) */
    xmit_spi(n);

    /* Receive command response */
    if (cmd == CMD12) rcvr_spi();        /* Skip a stuff byte when stop reading */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    do
        res = rcvr_spi();
    while ((res & 0x80) && --n);

    return res;            /* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE drv        /* Physical drive nmuber (0) */
)
{
    BYTE n, ty, ocr[4];


    if (drv) return STA_NOINIT;            /* Supports only single drive */
    if (Stat & STA_NODISK) return Stat;    /* No card in the socket */

    power_on();                            /* Force socket power on */
    for (n = 10; n; n--) xmit_spi(0xFF);   /* Send 80 dummy clocks */


    SELECT();
    if (wait_ready() != 0xFF)
    {
        DESELECT();
        rcvr_spi();            /* Idle (Release DO) */
        Stat = STA_NOINIT;
        return Stat;
    }


    ty = 0;
    if (send_cmd(CMD0, 0) == 1)              /* Enter Idle state */
    {
        TIMESTAMP(Timer1);
        if (send_cmd(CMD8, 0x1AA) == 1)      /* SDC Ver2+ */
        {
            for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
            if (ocr[2] == 0x01 && ocr[3] == 0xAA)      /* The card can work at vdd range of 2.7-3.6V */
            {
                do
                {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 1UL << 30) == 0)    break;    /* ACMD41 with HCS bit */
                }
                while (!TIMEOUT_ms(Timer1, 1000));
                if (!TIMEOUT_ms(Timer1, 1000) && send_cmd(CMD58, 0) == 0)      /* Check CCS bit */
                {
                    for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
                    ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
                }
            }
        }
        else                                /* SDC Ver1 or MMC */
        {
            ty = (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) <= 1) ? CT_SD1 : CT_MMC;    /* SDC : MMC */
            do
            {
                if (ty == CT_SD1)
                {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) == 0) break;    /* ACMD41 */
                }
                else
                {
                    if (send_cmd(CMD1, 0) == 0) break;                                /* CMD1 */
                }
            }
            while (!TIMEOUT_ms(Timer1, 1000));
            if (TIMEOUT_ms(Timer1, 1000) || send_cmd(CMD16, 512) != 0)    /* Select R/W block length */
                ty = 0;
        }
    }
    CardType = ty;
    DESELECT();
    rcvr_spi();            /* Idle (Release DO) */
    wait_for_empty_txfifo();

    if (ty)              /* Initialization succeded */
    {
        Stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */
        set_max_speed();
    }
    else                /* Initialization failed */
    {
        power_off();
        Stat = STA_NOINIT;
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv        /* Physical drive number (0) */
)
{
    if (drv) return STA_NOINIT;        /* Supports only single drive */
    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..255) */
)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert to byte address if needed */

    SELECT();            /* CS = L */
    if (wait_ready() != 0xFF)
    {
        DESELECT();
        rcvr_spi();            /* Idle (Release DO) */
        return RES_NOTRDY;
    }

    if (count == 1)      /* Single block read */
    {
        if ((send_cmd(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
                && rcvr_datablock(buff, 512))
            count = 0;
    }
    else                  /* Multiple block read */
    {
        if (send_cmd(CMD18, sector) == 0)      /* READ_MULTIPLE_BLOCK */
        {
            do
            {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            }
            while (--count);
            send_cmd(CMD12, 0);
        }
    }

    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count            /* Sector count (1..255) */
)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

    if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert to byte address if needed */

    SELECT();            /* CS = L */
    if (wait_ready() != 0xFF)
    {
        DESELECT();
        rcvr_spi();            /* Idle (Release DO) */
        return RES_NOTRDY;
    }

    if (count == 1)      /* Single block write */
    {
        if ((send_cmd(CMD24, sector) == 0)    /* WRITE_BLOCK */
                && xmit_datablock(buff, 0xFE))
            count = 0;
    }
    else                  /* Multiple block write */
    {
        if (CardType & CT_SDC)
        {
            send_cmd(CMD55, 0);
            send_cmd(CMD23, count);    /* ACMD23 */
        }
        if (send_cmd(CMD25, sector) == 0)      /* WRITE_MULTIPLE_BLOCK */
        {
            do
            {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            }
            while (--count);
            if (!xmit_datablock(0, 0xFD))    /* STOP_TRAN token */
                count = 1;
        }
    }

    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,        /* Physical drive nmuber (0) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    DWORD st, ed, csz;
    LBA_t *dp;


    if (pdrv) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    res = RES_ERROR;

    SELECT();        /* CS = L */
    if (wait_ready() != 0xFF)
    {
        DESELECT();
        rcvr_spi();            /* Idle (Release DO) */
        return RES_NOTRDY;
    }

    switch (cmd) {
    case CTRL_SYNC :        /* Wait for end of internal write process of the drive */
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT : /* Get drive capacity in unit of sector (DWORD) */
        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
            if ((csd[0] >> 6) == 1) {   /* SDC ver 2.00 */
                csz = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
                *(LBA_t*)buff = csz << 10;
            } else {                    /* SDC ver 1.XX or MMC ver 3 */
                n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                csz = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                *(LBA_t*)buff = csz << (n - 9);
            }
            res = RES_OK;
        }
        break;

    case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
        if (CardType & CT_SD2) {    /* SDC ver 2.00 */
            if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD13, 0) == 0) { /* Read SD status */
                rcvr_spi(); /* Discard 2nd byte of R2 resp. */
                if (rcvr_datablock(csd, 16)) {                  /* Read partial block */
                    for (n = 64 - 16; n; n--) rcvr_spi();   /* Purge trailing data */
                    *(DWORD*)buff = 16UL << (csd[10] >> 4);
                    res = RES_OK;
                }
            }
        } else {                    /* SDC ver 1.XX or MMC */
            if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {  /* Read CSD */
                if (CardType & CT_SD1) {    /* SDC ver 1.XX */
                    *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                } else {                    /* MMC */
                    *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                }
                res = RES_OK;
            }
        }
        break;
    case CTRL_TRIM :    /* Erase a block of sectors (used when FF_USE_TRIM == 1) */
        if (!(CardType & CT_SDC)) break;                /* Check if the card is SDC */
        if (disk_ioctl(pdrv, MMC_GET_CSD, csd)) break;   /* Get CSD */
        if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break; /* Check if sector erase can be applied to the card */
        dp = buff; st = (DWORD)dp[0]; ed = (DWORD)dp[1];    /* Load sector block */
        if (!(CardType & CT_BLOCK)) {
            st *= 512; ed *= 512;
        }
        if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0) { /* Erase sector block */
            for (n = 60; n; n--)
            {
                if (wait_ready() == 0xFF)
                {
                    res = RES_OK;
                    break;
                }
            }
        }
        break;
    /* Following command are not used by FatFs module */

    case MMC_GET_TYPE :     /* Get MMC/SDC type (BYTE) */
        *ptr = CardType;
        res = RES_OK;
        break;

    case MMC_GET_CSD :      /* Read CSD (16 bytes) */
        if (send_cmd(CMD9, 0) == 0 && rcvr_datablock(ptr, 16)) {    /* READ_CSD */
            res = RES_OK;
        }
        break;

    case MMC_GET_CID :      /* Read CID (16 bytes) */
        if (send_cmd(CMD10, 0) == 0 && rcvr_datablock(ptr, 16)) {   /* READ_CID */
            res = RES_OK;
        }
        break;

    case MMC_GET_OCR :      /* Read OCR (4 bytes) */
        if (send_cmd(CMD58, 0) == 0) {  /* READ_OCR */
            for (n = 4; n; n--) *ptr++ = rcvr_spi();
            res = RES_OK;
        }
        break;

    case MMC_GET_SDSTAT :   /* Read SD status (64 bytes) */
        if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD13, 0) == 0) { /* SD_STATUS */
            rcvr_spi();
            if (rcvr_datablock(ptr, 64)) res = RES_OK;
        }
        break;

    default:
        res = RES_PARERR;
    }

    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */


    return res;
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

DWORD get_fattime (void)
{
    return *(Uint32 *)&FatFS_time;
//    return    ((2008UL-1980) << 25)    // Year = 2008
//              | (2UL << 21)            // Month = February
//              | (26UL << 16)            // Day = 26
//              | (14U << 11)            // Hour = 14
//              | (0U << 5)            // Min = 0
//              | (0U >> 1)                // Sec = 0
//              ;

}
