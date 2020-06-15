#ifndef __MP3_H
#define __MP3_H

typedef struct ID3V1_t
{
      char Header[3];
      char Title[30];
      char Artist[30];
      char Album[30];
      char Year[4];
      char Comment[28];
      char Reserve;
      char Track;
      char Genre;
} ID3V1;

typedef struct {
      unsigned char first;
      unsigned short t_sec;
      unsigned char t_ms;
      unsigned short lrcoff;
      unsigned char tagcnt;
} lyric_struct;

#endif

