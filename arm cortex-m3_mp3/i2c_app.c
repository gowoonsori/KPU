/********************************************************************************/
/* i2c_app.c                                                                    */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_test								*/
/********************************************************************************/
#include "hwdefs.h"


#define ClockSpeed			400000

#define OwnAddress1			0x58
#define OwnAddress2			0x60
#define SEEP_PAGE0			0xA0
#define SEEP_PAGE1			0xA2


unsigned char I2C1_PORT = 0,I2C_CHANNEL = 0;


void i2c_write (I2C_TypeDef *I2Cx, unsigned char device, unsigned char address, unsigned char buffer)
{
#if defined (GD32_CPU)
      while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY));
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      I2C_Send7bitAddress(I2Cx,device, I2C_Direction_Transmitter);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      I2C_SendData(I2Cx,address);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_SendData(I2Cx,buffer);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_GenerateSTOP(I2Cx,ENABLE);
      while(I2Cx->CR1 & 0x0200);
#else
      while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY));
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      device = device & 0xFFFE;
      I2C_Send7bitAddress(I2Cx,device, I2C_Direction_Transmitter);
      while ((I2Cx->SR1 & 0x0002) != 0x0002);
      device = I2Cx->SR2;
      I2C_SendData(I2Cx,address);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_SendData(I2Cx,buffer);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_GenerateSTOP(I2Cx,ENABLE);
      while(I2Cx->CR1 & 0x0200);
#endif
}


unsigned char i2c_read (I2C_TypeDef *I2Cx, unsigned char device, unsigned char address)
{
      unsigned char data;
#if defined (GD32_CPU)
      while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY));
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      I2C_Send7bitAddress(I2Cx,device,I2C_Direction_Transmitter);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      I2C_SendData(I2Cx,address);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_AcknowledgeConfig(I2Cx,DISABLE);
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      I2C_Send7bitAddress(I2Cx,device,I2C_Direction_Receiver);
      while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR));
      I2C_AcknowledgeConfig(I2Cx,DISABLE);
      while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
      I2C_GenerateSTOP(I2Cx,ENABLE);
      while ((I2C_GetLastEvent(I2Cx) & 0x0040) != 0x0040);
      data = I2C_ReceiveData(I2Cx);
      while(I2Cx->CR1 & 0x0200);
      I2C_AcknowledgeConfig(I2Cx,ENABLE);
#else
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      device = device & 0xFFFE;
      I2C_Send7bitAddress(I2Cx,device, I2C_Direction_Transmitter);
      while ((I2Cx->SR1 & 0x0002) != 0x0002);
      data = I2Cx->SR2;
      I2C_SendData(I2Cx,address);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_AcknowledgeConfig(I2Cx,DISABLE);
      I2C_GenerateSTART(I2Cx,ENABLE);
      while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
      I2C_Send7bitAddress(I2Cx,device,I2C_Direction_Receiver);
      while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR));
      I2C_AcknowledgeConfig(I2Cx,DISABLE);
      __disable_irq();
      device = I2Cx->SR2;
      I2C_GenerateSTOP(I2Cx,ENABLE);
      __enable_irq();
      while ((I2C_GetLastEvent(I2Cx) & 0x0040) != 0x0040);
      data = I2C_ReceiveData(I2Cx);
      while ((I2Cx->CR1 & 0x200) == 0x200);
      I2C_AcknowledgeConfig(I2Cx,ENABLE);
#endif
      return data;
}


void I2C_LowLevel_Init (void)
{
      I2C_InitTypeDef I2C_InitStructure;
      if (I2C_CHANNEL == 0) {
         /* Enable I2C1 clock */
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
         if (I2C1_PORT == 0) {
            /* I2C1 SDA and SCL configuration */
            GPIO_Init_Pin(GPIOB,GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
            GPIO_Init_Pin(GPIOB,GPIO_Pin_7,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
         } else {
            GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
            /* I2C1 SDA and SCL configuration */
            GPIO_Init_Pin(GPIOB,GPIO_Pin_8,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
            GPIO_Init_Pin(GPIOB,GPIO_Pin_9,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
         }
         I2C_DeInit(I2C1);
         /* I2C1 configuration */
         I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
         I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
         I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
         I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
         I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
         I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
         I2C_Init(I2C1, &I2C_InitStructure);
      } else {
         /* Enable I2C2 clock */
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
         /* I2C2 SDA and SCL configuration */
         GPIO_Init_Pin(GPIOB,GPIO_Pin_10,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
         GPIO_Init_Pin(GPIOB,GPIO_Pin_11,GPIO_Speed_50MHz,GPIO_Mode_AF_OD);
         I2C_DeInit(I2C2);
         /* I2C2 configuration */
         I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
         I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
         I2C_InitStructure.I2C_OwnAddress1 = OwnAddress2;
         I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
         I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
         I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
         I2C_Init(I2C2, &I2C_InitStructure);
      }
}


void u8_write (unsigned char device,unsigned char address,unsigned char data)
{
      /* Reload IWDG counter */
      IWDG_ReloadCounter();
      if (I2C_CHANNEL == 0) {
         i2c_write(I2C1,device,address,data);
      } else {
         i2c_write(I2C2,device,address,data);
      }
      wait_ms(4);
}


unsigned char u8_read (unsigned char device,unsigned char address)
{
      unsigned char ret_value;
      /* Reload IWDG counter */
      IWDG_ReloadCounter();
      if (I2C_CHANNEL == 0) {
         ret_value = i2c_read(I2C1,device,address);
      } else {
         ret_value = i2c_read(I2C2,device,address);
      }
      return ret_value;
}


void init_i2c_seep (void)
{
      if (I2C_CHANNEL == 0) {
         I2C_LowLevel_Init();
         I2C_Cmd(I2C1, ENABLE);
      } else {
         I2C_LowLevel_Init();
         I2C_Cmd(I2C2, ENABLE);
      }
}


unsigned char eeprom_read (unsigned char address)
{
      return u8_read(SEEP_PAGE0,address);
}


void eeprom_write (unsigned char address,unsigned char data)
{
      u8_write(SEEP_PAGE0,address,data);
}