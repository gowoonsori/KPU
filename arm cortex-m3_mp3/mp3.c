/********************************************************************************/
/* mp3.c                                                                        */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include "hwdefs.h"
#include "mp3.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ff.h"			/* FatFs configurations and declarations */
#include "diskio.h"		/* Declarations of low level disk I/O functions */


struct farme
{
      unsigned char mp3name[65];
      unsigned char mp3time[20];
      unsigned char mp3format[20];
};


ID3V1 ID3V1_Buf;
unsigned short f_kbps = 0xffff;
float propos = 0;
unsigned short timetemp = 0;
const char *music_extend [4] = {"MP3","WMA","WAV","MID"};
struct farme mp3;
unsigned char lrc_buffer[7000];
unsigned char tag_table[512];
unsigned short lrc_tag_cnt = 0;
unsigned short music_number;
unsigned char lrc_tag_type = 0;
lyric_struct lyric;
unsigned char bt30ms = 0;
unsigned char spedate [14] = {0};
unsigned char lrc_data[512];
unsigned char lrc[80];
unsigned char buffer[1024];
unsigned int datasize = 0;
unsigned char ftemp,fdx,buffer_read;
unsigned char mp3_flag,new_play;
int seek_value;

FATFS fs;
FIL fsrc,fap,fii;
FRESULT res;
unsigned int bw,br;
FILINFO fno;
DIR dirs;

extern volatile unsigned char play_mp3,volume_flag;
extern volatile unsigned short second,volume;


struct FileList {
      char filename[150];
      struct FileList *next;
      struct FileList *back;
};

typedef struct FileList dnode;
typedef dnode *dlink;
dlink head;



char hex2dec (const char ch)
{
      if (ch <= '9') return (ch - '0');
      if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
      if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 10);
      return 0;
}


char dec2hex (const char ch)
{
      if (ch <= 9) return (ch + '0');
      if (ch >= 10 && ch <= 15) return (ch + 'A' - 10);
      return 0;
}


void mp3_prog_bar (unsigned int pos,unsigned int lenth)
{
      static unsigned short playtime = 0;
      char dig0,dig1,dig2,dig3;
      unsigned short time = 0;
      char buff[32];
      if (f_kbps == 0xffff) {
	 playtime = 0;
	 f_kbps = get_head_info();
      }
      if (lenth) {
	 propos = (float)pos / lenth;
	 if (propos > 1) propos = 1;
      } else {
      	 return;
      }
      if (f_kbps) {
	 time = (lenth / f_kbps) / 125;
	 time = propos * time;
      } else {
         time = get_decode_time();
      }
      if (time != playtime) {
	 time = (((float)lenth / f_kbps) / 125) * propos;
	 playtime = time;
	 if (new_play) {
	    lput_char_xy(2,2,"PLAY ");
	 }
	 if (time >= 3600) time = 3599;
	 dig3 = (time / 600);
	 dig2 = ((time / 60) % 10);
	 dig1 = ((time % 60) / 10);
	 dig0 = ((time % 60) % 10);	 
	 buff[0] = dec2hex(dig3);
	 buff[1] = dec2hex(dig2);
	 buff[2] = ':';
	 buff[3] = dec2hex(dig1);
	 buff[4] = dec2hex(dig0);
	 buff[5] = '/';
	 if (new_play == 0) {
	    buff[6] = 0;
	    lput_char_xy(7,2,buff);
	 }
	 if (new_play) {
	    if (f_kbps) time = (lenth / f_kbps) / 125;else time = 0;
	    if (time >= 3600) time = 3599;
	    dig3 = (time / 600);
	    dig2 = ((time / 60) % 10);
	    dig1 = ((time % 60) / 10);
	    dig0 = ((time % 60) % 10);	    	
	    buff[6] = dec2hex(dig3);
	    buff[7] = dec2hex(dig2);
	    buff[8] = ':';
	    buff[9] = dec2hex(dig1);
	    buff[10] = dec2hex(dig0);
	    buff[11] = 0;
	    mp3.mp3time[6] = dec2hex(dig3);
            mp3.mp3time[7] = dec2hex(dig2);
            mp3.mp3time[9] = dec2hex(dig1);
            mp3.mp3time[10] = dec2hex(dig0);
            lput_char_xy(13,2,buff+6);
            new_play = 0;
         }
      }
}


void filtrate_music (void)
{
      BOOL flag;
      unsigned char temp[69];
      unsigned char temp1[35],a;
      unsigned short i,j,k,l;
      unsigned short m;
      music_number = find_music();
      for (i=1;i<=music_number;i++) {
          for (j=0;j<69;j++) temp[j] = 0;
          if (i < 10) {
             temp[0] = 0x30;
             temp[1] = 0x30;
             temp[2] = 0x30 + i;
             temp[3] = 0x2e;
          } else if (i < 100) {
             temp[0] = 0x30;
             temp[1] = 0x30 + i / 10;
             temp[2] = 0x30 + i % 10;
             temp[3] = 0x2e;
          } else {
             temp[0] = 0x30 + i / 100;
             temp[1] = 0x30 + i / 10;
             temp[2] = 0x30 + i % 10;
             temp[3] = 0x2e;
          }
	  strcpy((char *)&temp[4],(char *)head->filename);
  	  strcpy((char *)head->filename,(char *)temp);
	  l = strlen(head->filename);
	  for (k=0;k<4;k++) head->filename[l - 4 + k] = 0;
	  if (i != music_number) head = head->next;
      }
      i = music_number;
      while ((i--) && (head->back != NULL)) head = head->back;
      i = music_number;
      while (i--) {
	    m = strlen(head->filename);
	    for (k=0;k<35;k++) temp1[k] = 0;
	    if (m > 23) {
	       if (head->filename[22] < 0x80) {
	          strcpy((char *)temp1,(char *)&head->filename[23]);
	          head->filename[23] = 0x2e;
	          head->filename[24] = 0x2e;
	          head->filename[25] = 0x2e;
	          head->filename[26] = 0;
	          head->filename[27] = 1;
	          strcpy((char *)&head->filename[28],(char *)temp1);
	       } else {
	          a = 0;
	          while (1) {
                        if (head->filename[a] < 0x80) a++;else a += 2;
	    	        if (a == 23) {
	    	           flag = 1;
	    	           break;
	    	        } else if (a == 24) {
	    	           flag = 0;
	    	           break;
	    	        }
	          }
	          if (flag == 1) {
	    	     strcpy((char *)temp1,(char *)&head->filename[23]);
	    	     head->filename[23] = 0x2e;
	    	     head->filename[24] = 0x2e;
	    	     head->filename[25] = 0x2e;
	    	     head->filename[26] = 0;
	    	     head->filename[27] = 1;
	    	     strcpy((char *)&head->filename[28],(char *)temp1);
	          } else {
	    	     strcpy((char *)temp1,(char *)&head->filename[22]);
	    	     head->filename[22] = 0x20;
	    	     head->filename[23] = 0x2e;
	    	     head->filename[24] = 0x2e;
	    	     head->filename[25] = 0x2e;
	    	     head->filename[26] = 0;
	    	     head->filename[27] = 0;
	    	     strcpy((char *)&head->filename[28],(char *)temp1);
	          }
	       }
	    }
	    if (i != 0) head = head->next;
      }
      i = music_number;
      while ((i--) && (head->back != NULL)) head = head->back;
}


void resume_music (unsigned char *name)
{
      unsigned char a,temp[35] = {0};
      unsigned char temp1[65] = {0};
      unsigned char record[] = {"0:/Music/"};
      for (a=0;a<65;a++) temp1[a] = 0;
      for (a=0;a<35;a++) temp[a] = 0;
      for (a=0;a<65;a++) mp3.mp3name[a] = 0;
      mp3.mp3name[0] = name[0];
      mp3.mp3name[1] = name[1];
      mp3.mp3name[2] = name[2];
      mp3.mp3name[3] = name[3];
      if (name[25] == 0x2e && name[26] == 0) {
	 if (name[27] == 1) {
	    strcpy((char *)temp,(char *)&name[28]);
	    for (a=23;a<65;a++)	name[a] = 0;
	    strcpy((char *)&name[23],(char *)temp);
	    strcpy((char *)temp1,(char *)&name[4]);
	    a = strlen((char *)temp1);
	    strcpy((char *)lrc,(char *)temp1);
	    temp1[a] = '.';
	    temp1[a + 1] = 'm';
	    temp1[a + 2] = 'p';
	    temp1[a + 3] = '3';
	    lrc[a] = '.';
	    lrc[a + 1] = 'l';
	    lrc[a + 2] = 'r';
	    lrc[a + 3] = 'c';
	    a = a + 4;
	    while (a < 55) temp1[a++] = 0;
	    for (a=0;a<65;a++) name[a] = 0;
	    strcpy((char *)name,(char *)record);
	    strcpy((char *)&name[9],(char *)temp1);
	    strcpy((char *)&mp3.mp3name[4],(char *)temp1);
	 } else if (name[27] == 0) {
	    strcpy((char *)temp,(char *)&name[28]);
	    for (a=22;a<65;a++) name[a] = 0;
	    strcpy((char *)&name[22],(char *)temp);
	    strcpy((char *)temp1,(char *)&name[4]);
	    a = strlen((char *)temp1);
	    strcpy((char *)lrc,(char *)temp1);
	    temp1[a] = '.';
	    temp1[a + 1] = 'm';
	    temp1[a + 2] = 'p';
	    temp1[a + 3] = '3';
	    lrc[a] = '.';
	    lrc[a + 1] = 'l';
	    lrc[a + 2] = 'r';
	    lrc[a + 3] = 'c';
	    a = a + 4;
	    while (a < 55) temp1[a++] = 0;
	    for (a=0;a<65;a++) name[a] = 0;
	    strcpy((char *)name,(char *)record);
	    strcpy((char *)&name[9],(char *)temp1);
	    strcpy((char *)&mp3.mp3name[4],(char *)temp1);
	 }
      } else {
	 strcpy((char *)temp1,(char *)&name[4]);
	 a = strlen((char *)temp1);
         strcpy((char *)lrc,(char *)temp1);
	 temp1[a] = '.';
	 temp1[a + 1] = 'm';
	 temp1[a + 2] = 'p';
	 temp1[a + 3] = '3';
	 lrc[a] = '.';
	 lrc[a + 1] = 'l';
	 lrc[a + 2] = 'r';
	 lrc[a + 3] = 'c';
	 a = a + 4;
         while (a < 55) temp1[a++] = 0;
	 for (a=0;a<65;a++) name[a] = 0;
	 strcpy((char *)name,(char *)record);
	 strcpy((char *)&name[9],(char *)temp1);
	 strcpy((char *)&mp3.mp3name[4],(char *)temp1);
      }
      a = strlen((char *)mp3.mp3name);
      mp3.mp3name[a - 1] = 0;
      mp3.mp3name[a - 2] = 0;
      mp3.mp3name[a - 3] = 0;
      mp3.mp3name[a - 4] = 0;
}


unsigned char find_music (void)
{
      dlink before = 0;
      dlink new_node;
      unsigned char file = 0, i;
      char *fn;
#if _USE_LFN
      static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
      fno.lfname = lfn;
      fno.lfsize = sizeof(lfn);
#endif
      res = f_opendir(&dirs, "0:/Music");
      if (res == FR_OK) {
         while ((f_readdir(&dirs, &fno) == FR_OK) && fno.fname[0]) {
#if _USE_LFN
               fn = *fno.lfname ? fno.lfname : fno.fname;
#else
               fn = fno.fname;
#endif
	       i = strlen(fn);
	       if (strcasecmp(&fn[i-3],music_extend[0]) == 0 || strcasecmp(&fn[i-3],music_extend[1]) == 0 || strcasecmp(&fn[i-3],music_extend[2]) == 0 || strcasecmp(&fn[i-3],music_extend[3]) == 0) {
	          file++;
	          if (file == 1) {
		     head = (dlink)malloc(sizeof(dnode));
		     if (!head) return 0;
		     strcpy(head->filename,fn);
		     head->next = NULL;
		     head->back = NULL;
		     before = head;
                  } else {
		     new_node = (dlink)malloc(sizeof(dnode));
		     if (!new_node) return 0;
                     strcpy(new_node->filename,fn);
                     new_node->next = NULL;
		     new_node->back = before;
		     before->next = new_node;
		     before = new_node;
	          }
	       }
	       //lcd_printf(2,3+file," %s ",fn);
         }
	 return file;
      }
      return 0;
}


void mp3_name (void)
{
      static unsigned char flag = 0,j = 0,count = 0;
      unsigned char i,dig0,dig1,dig2;
      unsigned char temp[48] = {0};
      if (flag == 0) {
         if (count == 0) {
            i = strlen((char *)mp3.mp3name);
            if (i > 18) {
               i = 0;
               while (1) {
                     if (mp3.mp3name[i] < 0x80) i++;else i += 2;
                     if (i == 18) {
                     	j = 1;
                     	break;
                     } else if (i == 19) {
                     	j = 0;
                     	break;
                     }
               }
               if (j == 1) {
                  for (i=0;i<18;i++) temp[i] = mp3.mp3name[i];
                  lcd_printf(2,7," %s ",temp);
                  flag = 0;
                  count++;
               } else {
                  for (i=0;i<17;i++) temp[i] = mp3.mp3name[i];
                  lcd_printf(2,7," %s ",temp);
                  flag = 0;
                  count++;
               }
            } else {
               lcd_printf(2,7," %s ",mp3.mp3name);
               flag = 1;
               count = 0;
            }
         } else if (count == 1) {
            if (j == 1) {
               for (i=18;i<36;i++) temp[i-18] = mp3.mp3name[i];
               lcd_printf(2,7," %s ",temp);
               flag = 1;
               count = 0;
               j = 0;
            } else {
               for (i=17;i<35;i++) temp[i-17] = mp3.mp3name[i];
               lcd_printf(2,7," %s ",temp);
               flag = 1;
               count = 0;
            }
         }
      } else if (flag == 1) {
      	 dig2 = (f_kbps / 100);
      	 dig1 = ((f_kbps % 100) / 10);
      	 dig0 = (f_kbps % 10);
         mp3.mp3format[10] = dec2hex(dig2);
         mp3.mp3format[11] = dec2hex(dig1);
         mp3.mp3format[12] = dec2hex(dig0);
         lcd_printf(2,8," %s ",mp3.mp3format);
         flag++;
      } else {
         lcd_printf(2,9," %s ",mp3.mp3time);
         flag = 0;
      }
}


unsigned char find_lrc (unsigned char *name)
{
      unsigned char i = 0;
      unsigned short j;
      unsigned char record[] = {"0:/lrc/"};
      char *fn;
#if _USE_LFN
      static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
      fno.lfname = lfn;
      fno.lfsize = sizeof(lfn);
#endif
      lrc_read_init();
      for (j=0;j<7000;j++) lrc_buffer[j] = 0;
      for (j=0;j<512;j++) tag_table[j] = 0;
      for (j=0;j<512;j++) lrc_data[j] = 0;
      res = f_opendir(&dirs,"0:/lrc");
      while ((f_readdir(&dirs, &fno) == FR_OK) && fno.fname[0]) {
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (strcasecmp((char *)fn,(char *)name) == 0) {
               strcpy((char *)fn,(char *)record);
               strcpy((char *)&fn[7],(char *)name);
               bw = f_open(&fap,fn,FA_OPEN_EXISTING | FA_READ);
               if (bw) break;
               get_tag();
               if (i == 0) break;
               init_tag();
               copy_lrc_to_ram();
               return 1;
            }
      }
      return 0;
}


unsigned char read_byte_lrcbuf (unsigned short addr)
{
      if (addr < 7000) return lrc_buffer[addr];else return 0xff;
}


unsigned char write_byte_lrcbuf (unsigned short addr,unsigned char data)
{
      if (addr < 7000) lrc_buffer[addr] = data;else return 0xff;
      return 1;
}


void lyric_display_ctrl (void)
{
      static unsigned int time = 0;
      static unsigned char ms = 0;
      unsigned int t1;
      if (!bt30ms) return;
      ms += 3;
      bt30ms = 0;
      t1 = (((float)fsrc.fsize / f_kbps) / 125) * (propos);
      if (t1 != time) {
	 time = t1;
	 ms = 0;
	 return;
      }
      if (((t1 == lyric.t_sec) && (ms >= lyric.t_ms)) || (t1 > lyric.t_sec)) {
	 read_lrc_file_data();
      }
}


void lrc_read_init (void)
{
      lyric.t_sec = 0;
      lyric.t_ms = 0;
      lyric.tagcnt = 0;
      lyric.lrcoff = 0;
      lyric.first = 1;
}


void read_lrc_file_data (void)
{
      unsigned char *lbuf = (unsigned char *)tag_table;
      unsigned char c;
      unsigned char cc;
      unsigned short t;
      if (lyric.tagcnt >= lrc_tag_cnt) return;
      lyric.t_sec = 0xffff;
      lyric.t_ms = 0;
      t = 0;
      if (!lyric.first) {
	 while (1) {
	       c = read_byte_lrcbuf(lyric.lrcoff++);
	       if (c == '[') {
		  cc = read_byte_lrcbuf(lyric.lrcoff);
		  if (cc <= 9) {
		     lyric.lrcoff--;
		     break;
		  }
	       }
	       if (c == 0x0d || c == 0x0a || c== '\0') break;
	       lbuf[t] = c;
	       t++;
	 }
	 lbuf[t] = '\0';
         if (t > 2) lcd_printf(2,10," %s ",lbuf);
      }
      lyric.first = 0;
      while (1) {
	    if (lyric.lrcoff > 7000) return;
	    c = read_byte_lrcbuf(lyric.lrcoff++);
	    if (c == '[') {
	       lyric.tagcnt++;
	       lyric.t_sec = read_byte_lrcbuf(lyric.lrcoff++);
	       lyric.t_sec *= 60;
	       lyric.lrcoff++;
	       lyric.t_sec += read_byte_lrcbuf(lyric.lrcoff++);
	       c = read_byte_lrcbuf(lyric.lrcoff++);
	       if (c == ']') break;
	       lyric.t_ms = read_byte_lrcbuf(lyric.lrcoff++);
	    } else if (c == ']') break;
      }
}


void  lrc_synchron (void)
{
      unsigned char c;
      lrc_read_init();
      do {
      	  c = read_byte_lrcbuf(lyric.lrcoff++);
      	  if (c == '[') {
      	     lyric.tagcnt++;
      	     lyric.t_sec = read_byte_lrcbuf(lyric.lrcoff++);
      	     lyric.t_sec *= 60;
      	     lyric.lrcoff++;
      	     lyric.t_sec += read_byte_lrcbuf(lyric.lrcoff++);
	  }
	  if (lyric.lrcoff > 7000) return;
      } while (lyric.t_sec < timetemp);
      lyric.lrcoff -= 5;
      lyric.tagcnt--;
      while (read_byte_lrcbuf(lyric.lrcoff--) != '[');
      lyric.lrcoff++;
      lyric.lrcoff++;
      lyric.t_sec = read_byte_lrcbuf(lyric.lrcoff++);
      lyric.t_sec *= 60;
      lyric.lrcoff++;
      lyric.t_sec += read_byte_lrcbuf(lyric.lrcoff++);
      while (read_byte_lrcbuf(lyric.lrcoff--) != '[');
      lyric.lrcoff++;
}


unsigned char get_byte (unsigned char rst)
{
      static unsigned short bytepos;
      unsigned short temp;
      if (rst) {
      	 if (rst == 1) {
            f_lseek(&fap,0);
            f_read(&fap, lrc_data, 512, &br);
	    bytepos = 0;
	 } else {
	    bytepos--;
	 }
	 return 0xff;
      }
      if (bytepos > 511) {
	 for (temp=0;temp<512;temp++) lrc_data[temp] = 0xff;
         f_read(&fap, lrc_data, 512, &br);
	 bytepos=0;
      }
      return lrc_data[bytepos++];
}


unsigned char get_num (void)
{
      unsigned char t1 = 0;
      unsigned char t2 = 0;
      t1 = get_byte(0);
      t2 = get_byte(0);
      if (t1 < '9' && t1 > '0' && t2 == ']') {
	 get_byte(2);
	 return (t1 - '0') * 10;
      }
      t1 -= '0';
      t2 -= '0';
      if (t1 < 10 && t2 < 10) {
	 return t1 * 10 + t2;
      }
      return 0xFF;
}


unsigned char get_tag (void)
{
      unsigned short i = 0;
      unsigned char numx;
      unsigned char temp;
      unsigned char min;
      unsigned char sec;
      unsigned char mms;
      lrc_tag_cnt = 0;
      lrc_tag_type = 0;
      get_byte(1);
      while (1) {
	    if (lrc_tag_cnt > 426) break;
	    while (1) {
		  temp = get_byte(0);
		  if (temp == '[') {
		     break;
		  } else if (temp == 0xff) {
		     if (lrc_tag_cnt) return 1;else return 0;
		  }
	    }
	    numx = get_num();
	    if (get_byte(0) != ':' || numx == 0xff) continue;
	    min = numx;
	    numx = get_num();
	    temp = get_byte(0);
	    if ((temp != '.' && temp != ']') || numx == 0xff) continue;
	    sec = numx;
	    if (temp != '.') {
	       tag_table[i] = min;
	       tag_table[i + 1] = sec;
	       i += 2;
	       lrc_tag_type = 0;
	       lrc_tag_cnt++;
	       continue;
	    }
	    numx = get_num();
	    temp = get_byte(0);
	    if (temp != ']' || numx == 0xff) continue;
	    mms = numx;
	    lrc_tag_type = 1;
	    lrc_tag_cnt++;
	    tag_table[i] = min;
	    tag_table[i + 1] = sec;
	    tag_table[i + 2] = mms;
	    i += 3;
      }
      return 1;
}


void init_tag (void)
{
      unsigned short t = 0;
      unsigned char t1;
      unsigned char temp;
      unsigned short tagcnt = 0;
      unsigned char offset;
      if (lrc_tag_type) {
	 tagcnt = (lrc_tag_cnt - 1) * 3;
	 offset = 3;
      } else {
	 tagcnt = (lrc_tag_cnt - 1) * 2;
	 offset = 2;
      }
      do {
	  t1 = 0;
	  for (t=0;t<tagcnt;t+=offset) {
	      if (tag_table[t] > tag_table[t + offset]) {
		 temp = tag_table[t + offset];
		 tag_table[t + offset] = tag_table[t];
		 tag_table[t] = temp;
		 temp = tag_table[t + offset + 1];
		 tag_table[t + offset + 1] = tag_table[t + 1];
		 tag_table[t + 1] = temp;
		 if (offset == 3) {
		    temp = tag_table[t + 5];
		    tag_table[t + 5] = tag_table[t + 2];
		    tag_table[t + 2] = temp;
		 }
		 t1 = 1;
	      } else if (tag_table[t + 1] > tag_table[t + offset + 1] && tag_table[t] == tag_table[t + offset])	{
		 temp = tag_table[t + offset + 1];
		 tag_table[t + offset + 1] = tag_table[t + 1];
		 tag_table[t + 1] = temp;
		 if (offset == 3) {
		    temp = tag_table[t + 5];
		    tag_table[t + 5] = tag_table[t + 2];
		    tag_table[t + 2] = temp;
		 }
		 t1 = 1;
	      } else if (offset == 3) {
		 if (tag_table[t + 2] > tag_table[t + 5] && tag_table[t] == tag_table[t + 3] && tag_table[t + 1] == tag_table[t + 4]) {
		    temp = tag_table[t + 5];
		    tag_table[t + 5] = tag_table[t + 2];
		    tag_table[t + 2] = temp;
		    t1 = 1;
		 }
	      }
	  }
      } while (t1);
}


unsigned char copy_lyric (unsigned char tmin,unsigned char tsec,unsigned char tmms)
{
      static unsigned short tagpos = 0;
      static unsigned short lrcbufpos = 0;
      unsigned char temp;
      unsigned char temp1;
      unsigned short tagcnt = 0;
      if (tmin == 0xff) {
      	 tagpos = 0;
      	 lrcbufpos = 0;
      	 return 1;
      }
      if (lrc_tag_type) tagcnt = (lrc_tag_cnt - 1) * 3;else tagcnt = (lrc_tag_cnt - 1) * 2;
      if (tagpos > tagcnt) {
	 write_byte_lrcbuf(lrcbufpos,'\0');
	 return 0xff;
      }
      if (tmin == tag_table[tagpos] && tsec == tag_table[tagpos + 1] && lrc_tag_type == 0) {
	 tagpos += 2;
      } else if (tmin == tag_table[tagpos] && tsec == tag_table[tagpos + 1] && tmms == tag_table[tagpos + 2] && lrc_tag_type == 1) {
	 tagpos+=3;
      } else return 1;
      write_byte_lrcbuf(lrcbufpos++,'[');
      write_byte_lrcbuf(lrcbufpos++,tmin);
      write_byte_lrcbuf(lrcbufpos++,':');
      write_byte_lrcbuf(lrcbufpos++,tsec);
      if (lrc_tag_type) {
	 write_byte_lrcbuf(lrcbufpos++,'.');
	 write_byte_lrcbuf(lrcbufpos++,tmms);
      }
      write_byte_lrcbuf(lrcbufpos++,']');
      while (1) {
	    temp = get_byte(0);
	    if (temp == '[') {
	       write_byte_lrcbuf(lrcbufpos,temp);
	       temp1 = get_byte(0);
	       temp = temp1;
	       temp1 -= '0';
	       if (temp1 < 10) {
		  while (1) {
			temp1 = get_byte(0);
			if (temp1 == ']' || temp1 == 0xff) break;
		  }
	       } else {
		  lrcbufpos++;
	          write_byte_lrcbuf(lrcbufpos++,temp);
		  break;
	       }
	    } else if (temp != ' ') {
	       if (temp != 0xFF && temp != 0x00) write_byte_lrcbuf(lrcbufpos++,temp);
	       break;
	    }
      }
      while (1) {
	    temp = get_byte(0);
	    if (temp != 0xFF && temp != 0x00) write_byte_lrcbuf(lrcbufpos++,temp);
	    if (temp == 0x0A || temp == 0xFF || temp == 0x00) {
	       write_byte_lrcbuf(lrcbufpos++,temp);
	       break;
	    }
      }
      return 1;
}


void copy_lrc_to_ram (void)
{
      unsigned char numx;
      unsigned char temp;
      unsigned char min;
      unsigned char sec;
      unsigned char mms;
      copy_lyric(0xff,0,0);
      get_byte(1);
      while (1) {
	    while (1) {
		  temp = get_byte(0);
		  if (temp == '[') {
		     break;
		  } else if (temp == 0xff) {
		     get_byte(1);
		  }
	    }
	    numx = get_num();
	    if (get_byte(0) != ':' || numx == 0xff) continue;
	    min = numx;
	    numx = get_num();
	    temp = get_byte(0);
	    if ((temp != '.' && temp != ']') || numx == 0xff) continue;
	    sec = numx;
	    if (temp != '.') {
	       if (copy_lyric(min,sec,0) == 0xFF) return;
	       continue;
	    }
	    numx = get_num();
	    temp = get_byte(0);
	    if (temp != ']' || numx == 0xff) continue;
	    mms = numx;
	    if (copy_lyric(min,sec,mms) == 0xFF) return;
      }
}


void display_id3v1_tag (void)
{
      unsigned char i, buff[128];
	  lcd_printf(1,1,"                              ");
	  lcd_printf(1,2,"                              ");
	  lcd_printf(1,3,"                              ");
	  lcd_printf(1,4,"                              ");
	  lcd_printf(1,5,"                              ");
	  lcd_printf(1,6,"                              ");
	  lcd_printf(1,7,"                              ");
	  lcd_printf(1,8,"                              ");
	  lcd_printf(1,9,"                              ");
      lcd_printf(1,10,"                              ");
      lcd_printf(1,11,"                              ");
      lcd_printf(1,12,"                              ");
      lcd_printf(1,13,"                              ");
      lcd_printf(1,14,"                              ");
      lcd_printf(1,15,"                              ");
      f_lseek(&fsrc, fsrc.fsize - 128);
      f_read(&fsrc,buff,128,&br);
      if ((buff[0] == 'T') && (buff[1] == 'A') && (buff[2] == 'G')) {
         for (i=0;i<30;i++) ID3V1_Buf.Title[i] = buff[3 + i];
         for (i=0;i<30;i++) ID3V1_Buf.Artist[i] = buff[33 + i];
         for (i=0;i<30;i++) ID3V1_Buf.Album[i] = buff[63 + i];
         lcd_printf(1,10,"Tttle: %s ",ID3V1_Buf.Title);
         lcd_printf(1,12,"Artist: %s ",ID3V1_Buf.Artist);
         lcd_printf(1,14,"Album: %s ",ID3V1_Buf.Album);
      }
      f_lseek(&fsrc,0);
}

void mp3_displaylist(void){
	int i =0;
	for(i=0;i<18;i++){
		lcd_printf(0,i,"                             ");
	}
	for(i=0;i<5;i++){
		lcd_printf(2,i+4,"%d. %s      ",i+1,head->filename);
		head=head->next;
		bw = f_open(&fsrc,head->filename, FA_OPEN_EXISTING | FA_READ);
	}
}

void mp3_displayinit (void){
	lcd_printf(1,1,"                              ");
	lcd_printf(2,0,"MP3 Init OK!!! %d  ",ftemp);
      lcd_printf(2,3,"MP3 Open %d Size %d ",music_number,fsrc.fsize);
      lcd_printf(2,6,"%2d: %s    ",music_number,head->filename);
	  
}

void mp3_init (void)
{
      SPI_configuration();
      lcd_printf(2,0,"SPI Init OK  ");
      SD_init();
      lcd_printf(2,0,"SD Init OK  ");
      vs1003_init();
      lcd_printf(2,0,"VS1003 Init OK  ");
      mp3_reset();
      lcd_printf(2,0,"VS1003 Reset1 OK  ");
      vs1003_soft_reset();
      load_patch();
      lcd_printf(2,0,"VS1003 Reset2 OK  ");
      f_mount(0,&fs);
      lcd_printf(2,0,"FAT Mount OK     ");
      ftemp = find_music();
      lcd_printf(2,0,"MP3 List %d    ",ftemp);
      music_number = ftemp;
      f_chdir("0:/Music");
      bw = f_open(&fsrc,head->filename, FA_OPEN_EXISTING | FA_READ);
      set_vs1003();
      lcd_printf(2,0,"MP3 Set ");
      vs1003_sine_test();
      play_prev();
      lcd_printf(2,0,"MP3 Init OK!!! %d  ",ftemp);
      lcd_printf(2,3,"MP3 Open %d Size %d ",music_number,fsrc.fsize);
      lcd_printf(2,6,"%2d: %s    ",music_number,head->filename);
      //display_id3v1_tag();
      new_play = 1;
}


void play_next (void)
{
      delay_ms(1000);
     if ((music_number--) && (head != NULL)) {
	 if (music_number == 0) {
	    while (head->back != NULL) head = head->back;
	 }
	 if (music_number != 0) head = head->next;else music_number = ftemp;
	 bw = f_open(&fsrc,head->filename, FA_OPEN_EXISTING | FA_READ);
	 datasize = 0;
	 seek_value = 0;
	 new_play = 1;
         buffer_read = 0;
         fdx = 0;
         f_kbps = 0xffff;
         memset((char *)buffer,0,1024);
	 lcd_printf(2,3,"MP3 Open %d Size %d ",music_number,fsrc.fsize);
	 lcd_printf(2,6,"%2d: %s    ",music_number,head->filename);
	 //display_id3v1_tag();
      }
}


void play_prev (void)
{
      music_number++;
      if (music_number == ftemp + 1) {
         while(head->next != NULL) head = head->next;
         head = head->back;
         music_number = 1;
      } else {
         if (music_number != ftemp) {
            head=head->back;
            head=head->back;
         } else {
            mp3_flag = 0;
            datasize = 0;
            ftemp = find_music();
            music_number = ftemp;
            bw = f_open(&fsrc,head->filename, FA_OPEN_EXISTING | FA_READ);
         }
      }
      music_number++;
      play_next();
}


void mp3_display (void)
{
      mp3_prog_bar(datasize,fsrc.fsize);
}


void mp3_seek (unsigned char sw)
{
       if (sw) {
       	  seek_value += 8192;
       	  if (seek_value >= (int)fsrc.fsize) seek_value = (int)fsrc.fsize;
       } else {
       	  seek_value -= 8192;
       	  if (seek_value < 0) seek_value = 0;
       }
       f_lseek(&fsrc,seek_value);
}


void mp3_play (void)
{
      mp3_flag = 0;
      if ((datasize >= fsrc.fsize) || (fsrc.fsize == 0)) mp3_flag = 1;
      if ((mp3_flag == 1) && (play_mp3)) {
	 mp3_flag = 0;
	 if ((music_number--) && (head != NULL)) {
	    if (music_number == 0) {
	       while (head->back != NULL) head = head->back;
	    }
	    if (music_number != 0) head = head->next;else music_number = ftemp;
	    bw = f_open(&fsrc,head->filename, FA_OPEN_EXISTING | FA_READ);
	    datasize = 0;
	    seek_value = 0;
	    f_kbps = 0xffff;
	    new_play = 1;
            memset((char *)buffer,0,1024);
	    lcd_printf(2,3,"MP3 Open %d Size %d ",music_number,fsrc.fsize);
	    lcd_printf(2,6,"%2d: %s    ",music_number,head->filename);
	    //display_id3v1_tag();
	 }
      }
      if ((datasize < fsrc.fsize) && (fsrc.fsize) && (buffer_read == 0) && (play_mp3)) {
         if ((f_kbps == 0xffff) && (datasize > 1024)) {
            vs1003_get_mp3info();
         }
         if (second >= 1) {
      	    second = 0;
      	    if (f_kbps != 0xffff) mp3_display();
         }
         if (volume_flag) {
            volume_flag = 0;
            vs1003_set_volume(volume);
         }
         datasize += 512;
         seek_value += datasize;
         bw = f_read(&fsrc,buffer,512,&br);
         datasize += 512;
         seek_value += datasize;
         bw = f_read(&fsrc,buffer+512,512,&br);
         buffer_read = 1;
         fdx = 0;
      }
      if ((fdx < 32) && (buffer_read) && (play_mp3)) {
      	 if (GPIOC->IDR & MP3_DREQ) {
	    vs1003_data_write(buffer + fdx * 32);
	    fdx++;
	    if (fdx >= 32) {
	       buffer_read = 0;
	       fdx = 0;
	    }
         }
      }
}