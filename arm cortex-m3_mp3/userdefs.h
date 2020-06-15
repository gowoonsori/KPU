/********************************************************************************/
/* userdefs.h                                                                   */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#ifndef __USERDEFS_H
#define __USERDEFS_H

// user code
// PORT_A
#define TXD2				GPIO_Pin_2	// Port A, RS232
#define RXD2				GPIO_Pin_3	// Port A, RS232
#define TXD1				GPIO_Pin_9	// Port A, RS232
#define RXD1				GPIO_Pin_10	// Port A, RS232

// PORT_B

// PORT C

// PORT D
#define OSC1				GPIO_Pin_0	// Port D, OSC_IN
#define OSC2				GPIO_Pin_1	// Port D, OSC_OUT
#define LED0				GPIO_Pin_8	// Port A, OUT PUT
#define LED1				GPIO_Pin_9	// Port A, OUT PUT
#define LED2				GPIO_Pin_10	// Port A, OUT PUT
#define LED3				GPIO_Pin_11	// Port A, OUT PUT

#define ON				1
#define OFF				!ON

#define BITBAND(address, bit_number)	((address & 0xF0000000) + 0x2000000 + ((address & 0xFFFFF) << 5) + (bit_number << 2))
#define MEM_ADDR(address)  		*((volatile unsigned long  *)(address))
#define BIT_ADDR(address, bit_number) 	MEM_ADDR(BITBAND(address, bit_number))

#define GPIOA_ODR_ADDRESS		(GPIOA_BASE + 12)	//0x4001080C
#define GPIOB_ODR_ADDRESS		(GPIOB_BASE + 12)	//0x40010C0C
#define GPIOC_ODR_ADDRESS		(GPIOC_BASE + 12)	//0x4001100C
#define GPIOD_ODR_ADDRESS		(GPIOD_BASE + 12)	//0x4001140C
#define GPIOE_ODR_ADDRESS		(GPIOE_BASE + 12)	//0x4001180C
#define GPIOF_ODR_ADDRESS		(GPIOF_BASE + 12)	//0x40011A0C
#define GPIOG_ODR_ADDRESS		(GPIOG_BASE + 12)	//0x40011E0C

#define GPIOA_IDR_ADDRESS		(GPIOA_BASE + 8)	//0x40010808
#define GPIOB_IDR_ADDRESS		(GPIOB_BASE + 8)	//0x40010C08
#define GPIOC_IDR_ADDRESS		(GPIOC_BASE + 8)	//0x40011008
#define GPIOD_IDR_ADDRESS		(GPIOD_BASE + 8)	//0x40011408
#define GPIOE_IDR_ADDRESS		(GPIOE_BASE + 8)	//0x40011808
#define GPIOF_IDR_ADDRESS		(GPIOF_BASE + 8)	//0x40011A08
#define GPIOG_IDR_ADDRESS		(GPIOG_BASE + 8)	//0x40011E08

#define PA_OUTPUT(n)			BIT_ADDR(GPIOA_ODR_ADDRESS,n)
#define PA_INPUT(n)    			BIT_ADDR(GPIOA_IDR_ADDRESS,n)

#define PB_OUTPUT(n)   			BIT_ADDR(GPIOB_ODR_ADDRESS,n)
#define PB_INPUT(n)    			BIT_ADDR(GPIOB_IDR_ADDRESS,n)

#define PC_OUTPUT(n)   			BIT_ADDR(GPIOC_ODR_ADDRESS,n)
#define PC_INPUT(n)    			BIT_ADDR(GPIOC_IDR_ADDRESS,n)

#define PD_OUTPUT(n)   			BIT_ADDR(GPIOD_ODR_ADDRESS,n)
#define PD_INPUT(n)    			BIT_ADDR(GPIOD_IDR_ADDRESS,n)

#define PE_OUTPUT(n)   			BIT_ADDR(GPIOE_ODR_ADDRESS,n)
#define PE_INPUT(n)    			BIT_ADDR(GPIOE_IDR_ADDRESS,n)

#define PF_OUTPUT(n)   			BIT_ADDR(GPIOF_ODR_ADDRESS,n)
#define PF_INPUT(n)    			BIT_ADDR(GPIOF_IDR_ADDRESS,n)

#define PG_OUTPUT(n)   			BIT_ADDR(GPIOG_ODR_ADDRESS,n)
#define PG_INPUT(n)    			BIT_ADDR(GPIOG_IDR_ADDRESS,n)

#define SD_CS_ENABLE()     		GPIOB->ODR &= ~(1 << 12)
#define SD_CS_DISABLE()    		GPIOB->ODR |= (1 << 12)
#define SD_PWR_ON()        		GPIOC->ODR &= ~(1 << 8)
#define SD_PWR_OFF()       		GPIOC->ODR |= (1 << 8)

#define MP3_CMD_CS     			(1 << 10)
#define MP3_DREQ       			(1 << 3)
#define MP3_DATA_CS    			(1 << 11)

typedef int INT;
typedef unsigned int UINT;

/* These types must be 8-bit integer */
typedef signed char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;

/* These types must be 16-bit integer */
typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned short WCHAR;

/* These types must be 32-bit integer */
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned int DWORD;

typedef unsigned char INT8U;
typedef signed char INT8S;
typedef unsigned int INT16U;
typedef signed int INT16S;
typedef unsigned long INT32U;
typedef signed long INT32S;

typedef enum {
      FALSE = 0,
      TRUE
} BOOL;
// userdefs.h ?? ?? 
typedef struct
{
      short year;
      char month;
      char day;
      char hour;
      char min;
      char sec;
      char wday;
} time_rtc;

// time.c
extern void time_from_seconds (time_rtc *ptime, unsigned int ul_seconds);
extern void time_to_seconds (time_rtc *ptime, unsigned int *pul_seconds);
extern void set_time (time_rtc *p_time);
extern void init_rtc (void);
extern void time_display(void);

// serial.c
extern void s_printf (USART_TypeDef *USARTx,char *form,...);
extern void serial_init (void);
extern void receive_serial1 (void);
extern void receive_serial2 (void);

// main.c
extern void led_control (unsigned short led, unsigned short ctl);
extern void wait_ms (unsigned short delay);

// lcd.c
extern void lcd_init (void);
extern void lcd_fill (unsigned char xsta,unsigned short ysta,unsigned char xend,unsigned short yend,unsigned short color);
extern void lcd_draw_line (unsigned char x1, unsigned short y1, unsigned char x2, unsigned short y2);
extern void lcd_draw_rectangle (unsigned char x1, unsigned short y1, unsigned char x2, unsigned short y2);
extern void lcd_draw_circle (unsigned char x0,unsigned short y0,unsigned char r);
extern void lcd_printf (unsigned short x,unsigned short y,char *form,...);
extern void lcd_clear (unsigned short color);
extern void lcd_set_pixel (unsigned char x,unsigned short y);
extern void set_color (unsigned short color);
extern void set_background_color (unsigned short color);
extern void set_pixel (unsigned short x, unsigned short y, unsigned short color);
extern void put_engxy (unsigned short x,unsigned short y,char pdata);
extern void lput_char_xy (unsigned short x,unsigned short y,char *str);
extern unsigned short uni_to_kssm (unsigned short wchar);

// ads7843.c
extern void touch_process (unsigned short *x,unsigned short *y);
extern void touch_init (void);

// mp3.c
extern void mp3_displaylist(void);
extern void mp3_displayinit (void);
extern void display_id3v1_tag (void);
extern void mp3_prog_bar (unsigned int pos,unsigned int lenth);
extern void mp3_name (void);
extern unsigned char find_lrc (unsigned char *name);
extern unsigned char get_byte (unsigned char rst);
extern unsigned char get_num (void);
extern unsigned char get_tag (void);
extern void init_tag (void);
extern unsigned char copy_lyric (unsigned char tmin,unsigned char tsec,unsigned char tmms);
extern void copy_lrc_to_ram (void);
extern void lrc_read_init (void);
extern void read_lrc_file_data (void);
extern void lyric_display_ctrl (void);
extern void lrc_synchron (void);
extern void filtrate_music (void);
extern unsigned char find_music (void);
extern void resume_music (unsigned char *name);
extern void mp3_init (void);
extern void mp3_play (void);
extern void play_next (void);
extern void play_prev (void);
extern void mp3_display (void);
extern void mp3_seek (unsigned char sw);

// sd_driver.c
extern void SPI_configuration (void);
extern void SPI_set_speed (unsigned char SpeedSet);
extern unsigned char SPI_read_write_byte (unsigned char TxData);
extern unsigned char SD_wait_ready (void);
extern unsigned char SD_send_command (unsigned char cmd, unsigned int arg, unsigned char crc);
extern unsigned char SD_send_command_no_deassert (unsigned char cmd, unsigned int arg, unsigned char crc);
extern unsigned char SD_init (void);
extern unsigned char SD_receive_data (unsigned char *data, unsigned short len, unsigned char release);
extern unsigned char SD_get_cid (unsigned char *cid_data);
extern unsigned char SD_get_csd (unsigned char *csd_data);
extern unsigned int SD_get_capacity (void);
extern unsigned char SD_read_single_block (unsigned int sector, unsigned char *buffer);
extern unsigned char SD_write_single_block (unsigned int sector, const unsigned char *buffer);
extern unsigned char SD_read_multi_block (unsigned int sector, unsigned char *buffer, unsigned char count);
extern unsigned char SD_write_multi_block (unsigned int sector, const unsigned char *data, unsigned char count);

// vs1003.c
extern void delay_ms (unsigned short nms);
extern unsigned short vs1003_reg_read (unsigned char address);
extern void vs1003_data_write (unsigned char *buff);
extern void vs1003_cmd_write (unsigned char address,unsigned short data);
extern void vs1003_init (void);
extern void mp3_reset (void);
extern void vs1003_soft_reset (void);
extern void set_vs1003 (void);
extern void vs1003_ram_test (void);
extern void vs1003_sine_test (void);
extern unsigned short get_decode_time (void);
extern unsigned short get_head_info (void);
extern void reset_decode_time (void);
extern void load_patch (void);
extern void get_spec (unsigned char *p);
extern void vs1003_set_volume (unsigned short volume);
extern void vs1003_get_mp3info (void);

#endif	/* __USERDEFS_H */