/********************************************************************************/
/* time.c                                                                       */
/* STM32F10X                                                                    */
/*(Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                          */
/* stm_start                                                                    */
/********************************************************************************/
#include "hwdefs.h"
#include "fundefs.h"

volatile unsigned int now_time;
volatile unsigned char time_rtc_flag;
const unsigned char month_length[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
extern volatile unsigned short jiffes;


void time_from_seconds (time_rtc *ptime, unsigned int ul_seconds)
{
      long year, idx = 0;
      // Extract the number of seconds from the time.
      ptime->sec = ul_seconds % 60;
      ul_seconds /= 60;
      // Extract the number of minutes from the time.
      ptime->min = ul_seconds % 60;
      ul_seconds /= 60;
      // Extract the number of hours from the time.
      ptime->hour = ul_seconds % 24;
      ul_seconds /= 24;
      // We now have days, so add the number of days between January 1, 1900 and
      // January 1, 1970.
      ul_seconds += (70 * 365) + 18;
      // The day of the week just happens to work out this way.
      ptime->wday = ul_seconds % 7;
      // Compute the number of years in terms of group of years from leap year to
      // leap year.
      year = 4 * (ul_seconds / ((365 * 4) + 1));
      ul_seconds %= (365 * 4) + 1;
      // If there are more than 365 days left in the current count of days, then
      // subtract the days from the remaining non-leap years.
      if (ul_seconds >= 366) {
         year += (ul_seconds - 1) / 365;
         ul_seconds = (ul_seconds - 1) % 365;
      }
      // Save the computed year.
      ptime->year = 1900 + year;
      // If this is a non-leap year and the day is past February 28th, then
      // increment the count of days by one (i.e. act as if each year is a leap
      // year).
      if (((year & 3) != 0) && (ul_seconds >= (31 + 28))) {
         ul_seconds++;
      }
      // Subtract days for each month till we find the current month.
      while (ul_seconds >= month_length[idx]) {
            ul_seconds -= month_length[idx++];
      }
      // Save the computed month and day.
      ptime->day = ul_seconds + 1;
      ptime->month = idx + 1;
}



void time_to_seconds (time_rtc *ptime, unsigned int *pul_seconds)
{
      long idx;
      // Compute the number of days that have past in this year.
      *pul_seconds = ptime->day - 1;
      idx = ptime->month - 1;
      while (idx--) {
            *pul_seconds += month_length[idx];
      }
      // If this is a non-leap year and the day is past February 28th, then
      // subtract the count of days by one (since we treat each year as if it
      // were a leap year).
      if (((ptime->year & 3) != 0) && (*pul_seconds >= (31 + 28))) {
         (*pul_seconds)--;
      }
      // Add the days for the years that have past.
      idx = ptime->year - 1970;
      *pul_seconds += (idx * 365) + ((idx + 1) / 4);
      // Convert the days to hours and add the current hour.
      *pul_seconds = (*pul_seconds * 24) + ptime->hour;
      // Convert the hours to minutes and add the current minute.
      *pul_seconds = (*pul_seconds * 60) + ptime->min;
      // Convert the minutes to seconds and add the current second.
      *pul_seconds = (*pul_seconds * 60) + ptime->sec;
}


void RTC_IRQHandler (void)
{
      if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
      	 RTC_ClearITPendingBit(RTC_IT_SEC);
      	 time_rtc_flag = 1;
      	 RTC_WaitForLastTask();
      }
}


void set_time (time_rtc *p_time)
{
      unsigned int real_time_second;
      time_to_seconds(p_time,&real_time_second);
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
      /* Change the current time */
      RTC_SetCounter(real_time_second);
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
	  
	  
}


void check_rtc (void)
{
      time_rtc n_time;
      now_time = RTC_GetCounter();
      time_from_seconds(&n_time,now_time);
      if ((n_time.year < 2000) || (n_time.year > 2100)) {
         n_time.sec = 0;
         n_time.min = 0;
         n_time.hour = 12;
         n_time.day = 1;
         n_time.month = 1;
         n_time.year = 2016;
      	 set_time(&n_time);
      }
}


void setup_rtc (void)
{
      /* Enable PWR and BKP clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
      /* Allow access to BKP Domain */
      PWR_BackupAccessCmd(ENABLE);
      /* Reset Backup Domain */
      BKP_DeInit();
      /* Enable LSE */
      RCC_LSEConfig(RCC_LSE_ON);
      /* Wait till LSE is ready */
      jiffes = 0;
      while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
            if (jiffes) {
               jiffes = 0;
               /* Reload IWDG counter */
               IWDG_ReloadCounter();
            }
      }
      /* Select LSE as RTC Clock Source */
      RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
      /* Enable RTC Clock */
      RCC_RTCCLKCmd(ENABLE);
      /* Wait for RTC registers synchronization */
      RTC_WaitForSynchro();
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
      /* Set RTC prescaler: set RTC period to 1sec */
      RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
      check_rtc();
      BKP_WriteBackupRegister(BKP_DR1,0x0408);
}


void init_rtc (void)
{
      NVIC_InitTypeDef NVIC_InitStructure;
      /* Configure and enable RTC interrupt */
      NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      PWR_BackupAccessCmd(ENABLE);
      BKP_ClearFlag();
      if (BKP_ReadBackupRegister(BKP_DR1) != (unsigned short)0x0408) {
         setup_rtc();
         BKP_TamperPinCmd(DISABLE);
         BKP_ITConfig(DISABLE);
         BKP_ClearFlag();
         BKP_WriteBackupRegister(BKP_DR1,0x0408);
      } else {
      	 RTC_WaitForSynchro();
      }
      PWR_BackupAccessCmd(DISABLE);
      /* Enable the RTC Second */
      RTC_ITConfig(RTC_IT_SEC, ENABLE);
}