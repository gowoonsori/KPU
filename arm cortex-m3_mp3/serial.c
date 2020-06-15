/********************************************************************************/
/* serial.c                                                                     */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "hwdefs.h"

#define SOH				0x01
#define STX				0x02
#define ETX				0x03
#define EOT				0x04
#define ENQ				0x05
#define ACK				0x06
#define NAK				0x15


volatile unsigned short rxcnt1,txcnt1,maxtx1,rxcnt2,txcnt2,maxtx2;
volatile unsigned char tx_flag1,tx_flag2,rxck1,rxck2,rxled,txled,this_id;
char txbuff1[256],rxbuff1[256],txbuff2[256],rxbuff2[256];
char sbuff[256];
unsigned char USART1_PORT;
volatile unsigned int flash_para[64];


void flash_read (void)
{
      unsigned int idx,flash_address;
      FLASH_Unlock();
      flash_address = (unsigned int)0x0807F800;
      for (idx=0;idx<64;idx++) {
          flash_para[idx] = (*(volatile unsigned int *)(flash_address + (idx * 4)));
      }
      FLASH_Lock();
}


void USART1_IRQHandler (void)
{
      if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
         /* Read one byte from the receive data register */
         rxbuff1[rxcnt1] = USART_ReceiveData(USART1);
         rxcnt1++;
         rxck1 = 0;
         if (rxled == 0) rxled = 1;
         USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      }
      if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
         /* Write one byte to the transmit data register */
      	 if (txcnt1 < maxtx1) {
            USART_SendData(USART1, txbuff1[txcnt1]);
      	    txcnt1++;
      	 } else {
      	    /* Disable the USART1 Transmit interrupt */
      	    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      	    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
      	 }
      	 USART_ClearITPendingBit(USART1, USART_IT_TXE);
      }
      if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) {
      	 USART_ITConfig(USART1, USART_IT_TC, DISABLE);
      	 USART_ClearITPendingBit(USART1, USART_IT_TC);
         tx_flag1 = 0;
         if (txled) txled = 0;
      }
}


void USART2_IRQHandler (void)
{
      if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
         /* Read one byte from the receive data register */
         rxbuff2[rxcnt2] = USART_ReceiveData(USART2);
         rxcnt2++;
         rxck2 = 0;
         if (rxled == 0) rxled = 1;
         USART_ClearITPendingBit(USART2, USART_IT_RXNE);
      }
      if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
         /* Write one byte to the transmit data register */
      	 if (txcnt2 < maxtx2) {
            USART_SendData(USART2, txbuff2[txcnt2]);
      	    txcnt2++;
      	 } else {
      	    /* Disable the USART2 Transmit interrupt */
      	    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
      	    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
      	 }
      	 USART_ClearITPendingBit(USART2, USART_IT_TXE);
      }
      if (USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
      	 USART_ITConfig(USART2, USART_IT_TC, DISABLE);
      	 USART_ClearITPendingBit(USART2, USART_IT_TC);
         tx_flag2 = 0;
         if (txled) txled = 0;
      }
}


void uasrt_send (USART_TypeDef *USARTx,char *buff,int length)
{
      if (USARTx == USART1) {
      	 while (tx_flag1);
         bzero(txbuff1,256);
         memcpy(txbuff1,buff,length);
         maxtx1 = length;
         txcnt1 = 0;
         tx_flag1 = 1;
         USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
         if (txled == 0) txled = 1;
      }
      if (USARTx == USART2) {
         while (tx_flag2);
         bzero(txbuff2,256);
         memcpy(txbuff2,buff,length);
         maxtx2 = length;
         txcnt2 = 0;
         tx_flag2 = 1;
         USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
         if (txled == 0) txled = 1;
      }
}


int uasrt_read (USART_TypeDef *USARTx,char *buff,int length)
{
      if (USARTx == USART1) {
      	 if (rxcnt1 == 0) return 0;
      	 if (rxcnt1 < length) length = rxcnt1;
      	 memcpy(buff,(char *)rxbuff1,rxcnt1);
      	 return rxcnt1;
      }
      if (USARTx == USART2) {
      	 if (rxcnt2 == 0) return 0;
      	 if (rxcnt2 < length) length = rxcnt2;
      	 memcpy(buff,(char *)rxbuff2,rxcnt2);
      	 return rxcnt2;
      }
      return 0;
}


int usart_check (USART_TypeDef *USARTx)
{
      if (USARTx == USART1) {
         return rxcnt1;
      }
      if (USARTx == USART2) {
         return rxcnt2;
      }
      return 0;
}


void s_printf (USART_TypeDef *USARTx,char *form,...)
{
      va_list argptr;
      va_start(argptr,form);
      vsprintf(sbuff,form,argptr);
      uasrt_send(USARTx,sbuff,strlen(sbuff));
      va_end(argptr);
}


void receive_serial1 (void)
{
      unsigned short idx;
      unsigned char bcc;
      if (rxcnt1) {
         if ((rxbuff1[0] == STX) && (rxbuff1[5] == ETX) && ((rxbuff1[1] == this_id) || (rxbuff1[1] == 33))) {
	    bcc = rxbuff1[0];
	    for (idx=1;idx<6;idx++) bcc = bcc ^ rxbuff1[idx];
            if (bcc == rxbuff1[6]) {
	       if ((rxbuff1[2] == 0xFF) && (rxbuff1[3] == 0xFF) && (rxbuff1[4] == 0xFF)) {
	       	  NVIC_SystemReset();
	       }
	    }
         }
         for (idx=0;idx<rxcnt1;idx++) {
      	     rxbuff1[idx] = 0;
      	 }
         rxcnt1 = 0;
      }
      if (rxled) rxled = 0;
}


void receive_serial2 (void)
{
      unsigned short idx;
      unsigned char bcc;
      if (rxcnt2) {
         if ((rxbuff2[0] == STX) && (rxbuff2[5] == ETX) && ((rxbuff2[1] == this_id) || (rxbuff2[1] == 33))) {
	    bcc = rxbuff2[0];
	    for (idx=1;idx<6;idx++) bcc = bcc ^ rxbuff2[idx];
            if (bcc == rxbuff2[6]) {
	       if ((rxbuff2[2] == 0xFF) && (rxbuff2[3] == 0xFF) && (rxbuff2[4] == 0xFF)) {
	       	  NVIC_SystemReset();
	       }
	    }
         }
         for (idx=0;idx<rxcnt2;idx++) {
      	     rxbuff2[idx] = 0;
      	 }
         rxcnt2 = 0;
      }
      if (rxled) rxled = 0;
}


void serial_init (void)
{
      USART_InitTypeDef	USART_InitStructure;
      NVIC_InitTypeDef NVIC_InitStructure;
      USART1_PORT = 0;
      rxcnt1 = 0;
      txcnt1 = 0;
      maxtx1 = 0;
      tx_flag1 = 0;
      rxcnt2 = 0;
      txcnt2 = 0;
      maxtx2 = 0;
      tx_flag2 = 0;
      /* Enable the USART1 Interrupt */
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      /* Enable the USART2 Interrupt */
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      if (USART1_PORT == 0) {
         /* Configure USART1 TX (PA9) as push-pull */
         GPIO_Init_Pin(GPIOA,GPIO_Pin_9,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
         /* Configure USART1 RX (PA10) as input floating */
         GPIO_Init_Pin(GPIOA,GPIO_Pin_10,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);
      } else {
         GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
         /* Configure USART1 TX (PB6) as push-pull */
         GPIO_Init_Pin(GPIOB,GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
         /* Configure USART1 RX (PB7) as input floating */
         GPIO_Init_Pin(GPIOB,GPIO_Pin_7,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);
      }
      /* Configure USART2 TX (PA2) as push-pull */
      GPIO_Init_Pin(GPIOA,GPIO_Pin_2,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      /* Configure USART2 RX (PA3) as input floating */
      GPIO_Init_Pin(GPIOA,GPIO_Pin_3,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);
      USART_InitStructure.USART_BaudRate = 9600;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      USART_InitStructure.USART_Parity = USART_Parity_No;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      /* Configure USART1 */
      USART_Init(USART1, &USART_InitStructure);
      /* Configure USART2 */
      USART_Init(USART2, &USART_InitStructure);
      /* Enable USART1 Receive and Transmit interrupts */
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
      /* Enable USART2 Receive and Transmit interrupts */
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
      /* Enable the USART1 */
      USART_Cmd(USART1, ENABLE);
      /* Enable the USART2 */
      USART_Cmd(USART2, ENABLE);
      flash_read();
      if (flash_para[0] == 0x68050555) {
         this_id = flash_para[1];
      }
      if ((this_id == 0) || (this_id > 32)) this_id = 1;
}