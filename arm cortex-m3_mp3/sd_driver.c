/********************************************************************************/
/* sd_driver.c                                                                  */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include "hwdefs.h"

#define SD_TYPE_MMC     		0
#define SD_TYPE_V1      		1
#define SD_TYPE_V2      		2
#define SD_TYPE_V2HC    		4

#define SPI_SPEED_LOW   		0
#define SPI_SPEED_HIGH  		1

#define NO_RELEASE      		0
#define RELEASE         		1

#define CMD0    			0
#define CMD9    			9
#define CMD10   			10
#define CMD12   			12
#define CMD16   			16
#define CMD17   			17
#define CMD18   			18
#define ACMD23  			23
#define CMD24   			24
#define CMD25   			25
#define ACMD41  			41
#define CMD55   			55
#define CMD58   			58
#define CMD59   			59


unsigned char  SD_type = 0;


void SPI_configuration (void)
{
      SPI_InitTypeDef SPI_InitStructure;
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
      GPIO_Init_Pin(GPIOC,GPIO_Pin_8,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      SD_PWR_ON();
      GPIO_Init_Pin(GPIOB,GPIO_Pin_13,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      GPIO_Init_Pin(GPIOB,GPIO_Pin_14,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      GPIO_Init_Pin(GPIOB,GPIO_Pin_15,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      GPIO_Init_Pin(GPIOB,GPIO_Pin_12,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      SD_CS_DISABLE();
      /* SPI2 configuration */
      SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
      SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
      SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
      SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
      SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
      SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
      SPI_InitStructure.SPI_CRCPolynomial = 7;
      SPI_Init(SPI2, &SPI_InitStructure);
      /* Enable SPI2  */
      SPI_Cmd(SPI2, ENABLE);
}


void SPI_set_speed (unsigned char speed_set)
{
      SPI_InitTypeDef SPI_InitStructure;
      SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
      SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
      SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
      SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
      if (speed_set == SPI_SPEED_LOW) {
         SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
      } else {
         SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
      }
      SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
      SPI_InitStructure.SPI_CRCPolynomial = 7;
      SPI_Init(SPI2,&SPI_InitStructure);
}


unsigned char SPI_read_write_byte (unsigned char tx_data)
{
      /* Loop while DR register in not emplty */
      while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
      /* Send byte through the SPI1 peripheral */
      SPI_I2S_SendData(SPI2, tx_data);
      /* Wait to receive a byte */
      while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
      /* Return the byte read from the SPI bus */
      return SPI_I2S_ReceiveData(SPI2);
}


unsigned char SD_wait_ready (void)
{
      unsigned char r1;
      unsigned short retry;
      retry = 0;
      do {
          r1 = SPI_read_write_byte(0xFF);
          retry++;
          if (retry == 0xfffe) {
             return 1;
          }
      } while (r1 != 0xFF);
      return 0;
}


unsigned char SD_send_command (unsigned char cmd, unsigned int arg, unsigned char crc)
{
      unsigned char r1;
      unsigned char retry = 0;
      SPI_read_write_byte(0xff);
      SD_CS_ENABLE();
      SPI_read_write_byte(cmd | 0x40);
      SPI_read_write_byte(arg >> 24);
      SPI_read_write_byte(arg >> 16);
      SPI_read_write_byte(arg >> 8);
      SPI_read_write_byte(arg);
      SPI_read_write_byte(crc);
      while ((r1 = SPI_read_write_byte(0xFF)) == 0xFF) {
            retry++;
            if (retry > 200) {
               break;
            }
      }
      SD_CS_DISABLE();
      SPI_read_write_byte(0xFF);
      return r1;
}


unsigned char SD_send_command_no_deassert (unsigned char cmd, unsigned int arg, unsigned char crc)
{
      unsigned char r1;
      unsigned char retry = 0;
      SPI_read_write_byte(0xff);
      SD_CS_ENABLE();
      SPI_read_write_byte(cmd | 0x40);
      SPI_read_write_byte(arg >> 24);
      SPI_read_write_byte(arg >> 16);
      SPI_read_write_byte(arg >> 8);
      SPI_read_write_byte(arg);
      SPI_read_write_byte(crc);
      while ((r1 = SPI_read_write_byte(0xFF)) == 0xFF) {
            retry++;
            if (retry > 200) {
               break;
            }
      }
      return r1;
}


unsigned char SD_init (void)
{
      unsigned short i;
      unsigned char r1;
      unsigned short retry;
      unsigned char buff[6];
      SPI_set_speed(0);
      for (i=0;i<10;i++) {
          SPI_read_write_byte(0xFF);
      }
      retry = 0;
      do {
          r1 = SD_send_command(CMD0, 0, 0x95);
          retry++;
      } while ((r1 != 0x01) && (retry<200));
      if (retry == 200) {
         return 1;
      }
      r1 = SD_send_command_no_deassert(8, 0x1aa, 0x87);
      if (r1 == 0x05) {
         SD_type = SD_TYPE_V1;
         SD_CS_DISABLE();
         SPI_read_write_byte(0xFF);
         retry = 0;
         do {
             r1 = SD_send_command(CMD55, 0, 0);
             if (r1 != 0x01) {
                return r1;
             }
             r1 = SD_send_command(ACMD41, 0, 0);
             retry++;
         } while ((r1 != 0x00) && (retry < 400));
         if (retry == 400) {
            retry = 0;
            do {
                r1 = SD_send_command(1, 0, 0);
                retry++;
            } while ((r1 != 0x00) && (retry < 400));
            if (retry == 400) {
               return 1;
            }
            SD_type = SD_TYPE_MMC;
         }
         SPI_set_speed(1);
	 SPI_read_write_byte(0xFF);
         r1 = SD_send_command(CMD16, 512, 0xff);
         if (r1 != 0x00) {
            return r1;
         }
      } else if (r1 == 0x01) {
         buff[0] = SPI_read_write_byte(0xFF);  //should be 0x00
         buff[1] = SPI_read_write_byte(0xFF);  //should be 0x00
         buff[2] = SPI_read_write_byte(0xFF);  //should be 0x01
         buff[3] = SPI_read_write_byte(0xFF);  //should be 0xAA
         SD_CS_DISABLE();
         SPI_read_write_byte(0xFF);
         if (buff[2] == 0x01 && buff[3] == 0xAA) {
            retry = 0;
    	    do {
    		r1 = SD_send_command(CMD55, 0, 0);
    		if (r1 != 0x01) {
   		   return r1;
    		}
    		r1 = SD_send_command(ACMD41, 0x40000000, 0);
    		retry++;
                if (retry > 200) {
                   return r1;
                }
            } while (r1 != 0);
            r1 = SD_send_command_no_deassert(CMD58, 0, 0);
            if (r1 != 0x00) {
               return r1;
            }
            buff[0] = SPI_read_write_byte(0xFF);
            buff[1] = SPI_read_write_byte(0xFF);
            buff[2] = SPI_read_write_byte(0xFF);
            buff[3] = SPI_read_write_byte(0xFF);
            SD_CS_DISABLE();
            SPI_read_write_byte(0xFF);
            if (buff[0] & 0x40) {
               SD_type = SD_TYPE_V2HC;
            } else {
               SD_type = SD_TYPE_V2;
            }
            SPI_set_speed(1);
         }
      }
      return r1;
}


unsigned char SD_receive_data (unsigned char *data, unsigned short len, unsigned char release)
{
      unsigned short retry;
      unsigned char r1;
      SD_CS_ENABLE();
      retry = 0;
      do {
          r1 = SPI_read_write_byte(0xFF);
          retry++;
          if (retry > 2000) {
             SD_CS_DISABLE();
             return 1;
          }
      } while (r1 != 0xFE);
      while (len--) {
            *data = SPI_read_write_byte(0xFF);
            data++;
      }
      SPI_read_write_byte(0xFF);
      SPI_read_write_byte(0xFF);
      if (release == RELEASE) {
         SD_CS_DISABLE();
         SPI_read_write_byte(0xFF);
      }
      return 0;
}


unsigned char SD_get_cid (unsigned char *cid_data)
{
      unsigned char r1;
      r1 = SD_send_command(CMD10, 0, 0xFF);
      if (r1 != 0x00) {
         return r1;
      }
      SD_receive_data(cid_data, 16, RELEASE);
      return 0;
}


unsigned char SD_get_csd (unsigned char *csd_data)
{
      unsigned char r1;
      r1 = SD_send_command(CMD9, 0, 0xFF);
      if (r1 != 0x00) {
         return r1;
      }
      SD_receive_data(csd_data, 16, RELEASE);
      return 0;
}


unsigned int SD_get_capacity (void)
{
      unsigned char csd[16];
      unsigned int capacity;
      unsigned char r1;
      unsigned short i;
      unsigned short temp;
      if (SD_get_csd(csd) != 0) {
         return 0;
      }
      if ((csd[0] & 0xC0) == 0x40) {
         capacity = ((((unsigned int)csd[8]) << 8) + (unsigned int)csd[9] + 1) * (unsigned int)1024;
      } else {
    	 i = csd[6] & 0x03;
    	 i <<= 8;
    	 i += csd[7];
    	 i <<= 2;
    	 i += ((csd[8] & 0xc0) >> 6);
    	 r1 = csd[9] & 0x03;
    	 r1 <<= 1;
    	 r1 += ((csd[10] & 0x80) >> 7);
    	 r1 += 2;
    	 temp = 1;
    	 while (r1) {
    	       temp *= 2;
    	       r1--;
    	 }
    	 capacity = ((unsigned int)(i + 1)) * ((unsigned int)temp);
    	 i = csd[5] & 0x0f;
    	 temp = 1;
    	 while (i) {
    	       temp *= 2;
    	       i--;
    	 }
    	 capacity *= (unsigned int)temp;
      }
      return (unsigned int)capacity;
}


unsigned char SD_read_single_block (unsigned int sector, unsigned char *buffer)
{
      unsigned char r1;
      SPI_set_speed(SPI_SPEED_HIGH);
	  if (SD_type != SD_TYPE_V2HC) {
         sector = sector << 9;
	  }
      r1 = SD_send_command(CMD17, sector, 0);
      if (r1 != 0x00) {
         return r1;
      }
      r1 = SD_receive_data(buffer, 512, RELEASE);
      if (r1 != 0) {
         return r1;
      } else {
         return 0;
      }
}


unsigned char SD_write_single_block (unsigned int sector, const unsigned char *data)
{
      unsigned char r1;
      unsigned short i;
      unsigned short retry;
      SPI_set_speed(SPI_SPEED_HIGH);
      if (SD_type != SD_TYPE_V2HC) {
         sector = sector << 9;
      }
      r1 = SD_send_command(CMD24, sector, 0x00);
      if (r1 != 0x00) {
         return r1;
      }
      SD_CS_ENABLE();
      SPI_read_write_byte(0xff);
      SPI_read_write_byte(0xff);
      SPI_read_write_byte(0xff);
      SPI_read_write_byte(0xFE);
      for (i=0;i<512;i++) {
          SPI_read_write_byte(*data++);
      }
      SPI_read_write_byte(0xff);
      SPI_read_write_byte(0xff);
      r1 = SPI_read_write_byte(0xff);
      if ((r1 & 0x1F) != 0x05) {
         SD_CS_DISABLE();
         return r1;
      }
      retry = 0;
      while (!SPI_read_write_byte(0xff)) {
            retry++;
            if (retry > 0xfffe) {
               SD_CS_DISABLE();
               return 1;
            }
      }
      SD_CS_DISABLE();
      SPI_read_write_byte(0xff);
      return 0;
}


unsigned char SD_read_multi_block (unsigned int sector, unsigned char *buffer, unsigned char count)
{
      unsigned char r1;
      SPI_set_speed(SPI_SPEED_HIGH);
	  if (SD_type != SD_TYPE_V2HC) {
         sector = sector << 9;
	  }
      r1 = SD_send_command(CMD18, sector, 0);
      if (r1 != 0x00) {
         return r1;
      }
      do {
          if (SD_receive_data(buffer, 512, NO_RELEASE) != 0x00) {
             break;
          }
          buffer += 512;
      } while (--count);
      SD_send_command(CMD12, 0, 0);
      SD_CS_DISABLE();
      SPI_read_write_byte(0xFF);
      if (count != 0) {
         return count;
      } else {
         return 0;
      }
}


unsigned char SD_write_multi_block (unsigned int sector, const unsigned char *data, unsigned char count)
{
      unsigned char r1;
      unsigned short i;
      SPI_set_speed(SPI_SPEED_HIGH);
      if (SD_type != SD_TYPE_V2HC) {
         sector = sector << 9;
      }
      if (SD_type != SD_TYPE_MMC) {
         r1 = SD_send_command(ACMD23, count, 0x00);
      }
      r1 = SD_send_command(CMD25, sector, 0x00);
      if (r1 != 0x00) {
         return r1;
      }
      SD_CS_ENABLE();
      SPI_read_write_byte(0xff);
      SPI_read_write_byte(0xff);
      do {
          SPI_read_write_byte(0xFC);
          for (i=0;i<512;i++) {
              SPI_read_write_byte(*data++);
          }
          SPI_read_write_byte(0xff);
          SPI_read_write_byte(0xff);
          r1 = SPI_read_write_byte(0xff);
          if ((r1 & 0x1F) != 0x05) {
             SD_CS_DISABLE();
             return r1;
          }
          if (SD_wait_ready() == 1) {
             SD_CS_DISABLE();
             return 1;
          }
      } while (--count);
      r1 = SPI_read_write_byte(0xFD);
      if (r1 == 0x00) {
         count =  0xfe;
      }
      if (SD_wait_ready()) {
         while (1) {
               //
         }
      }
      SD_CS_DISABLE();
      SPI_read_write_byte(0xff);
      return count;
}

