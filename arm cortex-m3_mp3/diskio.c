/********************************************************************************/
/* diskio.c                                                                     */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include "hwdefs.h"
#include "diskio.h"


DSTATUS disk_initialize (unsigned char drv)
{
      unsigned char state;
      if (drv) {
         return STA_NOINIT;
      }
      state = SD_init();
      if (state == STA_NODISK) {
         return STA_NODISK;
      } else if (state != 0) {
         return STA_NOINIT;
      } else {
         return 0;
      }
}


DSTATUS disk_status (unsigned char drv)
{
      if (drv) {
         return STA_NOINIT;
      }
      return 0;
}


DRESULT disk_read (unsigned char drv,unsigned char *buff,unsigned int sector,unsigned char count)
{
      unsigned char res = 0;
      if (drv || !count) {
         return RES_PARERR;
    }
    if (count == 1) {
       res = SD_read_single_block(sector, buff);
    } else {
       res = SD_read_multi_block(sector, buff, count);
    }
    if (res == 0x00) {
       return RES_OK;
    } else {
       return RES_ERROR;
    }
}


#if _READONLY == 0
DRESULT disk_write (unsigned char drv,const unsigned char *buff,unsigned int sector,unsigned char count)
{
      unsigned char res;
      if (drv || !count) {
         return RES_PARERR;
      }
      if (count == 1) {
         res = SD_write_single_block(sector, buff);
      } else {
         res = SD_write_multi_block(sector, buff, count);
      }
      if (res == 0) {
         return RES_OK;
      } else {
         return RES_ERROR;
      }
}
#endif


DRESULT disk_ioctl (unsigned char drv,unsigned char ctrl,void *buff)
{
      DRESULT res;
      if (drv) {
         return RES_PARERR;
      }
      switch (ctrl) {
         case CTRL_SYNC:
           SD_CS_ENABLE();
           if (SD_wait_ready() == 0) {
              res = RES_OK;
           } else {
              res = RES_ERROR;
           }
           SD_CS_DISABLE();
           break;
         case GET_BLOCK_SIZE:
           *(WORD*)buff = 512;
           res = RES_OK;
           break;
         case GET_SECTOR_COUNT:
           *(DWORD*)buff = SD_get_capacity();
           res = RES_OK;
           break;
         default:
           res = RES_PARERR;
           break;
      }
      return res;
}


DWORD get_fattime (void)
{
      return 0;
}



