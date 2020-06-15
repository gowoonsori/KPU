/********************************************************************************/
/* box3d.c                                                                      */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_test								*/
/********************************************************************************/
#include "hwdefs.h"
#include "color.h"
extern unsigned short foreground_color,background_color;
unsigned char demo;


/* Prototype
extern void bar (int x1, int y1, int x2, int y2, int color);
extern void norm_3d (int x1,int y1,int x2,int y2,int color);
extern void box_3d (int x1, int y1, int x2, int y2, int out_color, int in_color);
extern void vert_3d (int x1, int y1, int x2, int y2, int color);
extern void shade (int x1, int y1, int x2, int y2, int color);
extern void two_line_3d (int x1, int y1, int x2, int y2, int in_color, int out_color);
extern void double_3d (int x1, int y1, int x2, int y2, int color);
extern void round_3d (int x1, int y1, int x2, int y2, int color);
extern void inline_3d (int x1, int y1, int x2, int y2, int color);
extern void line_3d (int x1, int y1, int x2, int y2);
extern  void demo_box (void);
*/



void draw_horz_line (int x1, int x2, int y, unsigned short color)
{
      int idx;
      for (idx=x1;idx<(x2+1);idx++) set_pixel(idx,y,color);
}


void draw_vert_line (int x, int y1, int y2, unsigned short color)
{
      int idx;
      for (idx=y1;idx<(y2+1);idx++) set_pixel(x,idx,color);
}


void line (int x1, int y1, int x2, int y2 ,int color)
{
      int xdelta;
      int ydelta;
      int xinc;
      int yinc;
      int rem;
      int temp;
      if (y1 == y2) {
         if (x1 > x2) {
            temp = x1;
            x1 = x2 + 1;
            x2 = temp;
         } else --x2;
         draw_horz_line(x1, x2, y1, color);
         return;
      }
      if (x1 == x2) {
         if (y1 > y2) {
            temp = y1;
            y1 = y2 + 1;
            y2 = temp;
         } else --y2;
         draw_vert_line(x1, y1, y2, color);
         return;
      }
      xdelta = x2 - x1;
      ydelta = y2 - y1;
      if (xdelta < 0) xdelta = -xdelta;
      if (ydelta < 0) ydelta = -ydelta;
      xinc = (x2 > x1)? 1 : -1;
      yinc = (y2 > y1)? 1 : -1;
      set_pixel(x1, y1, color);
      if (xdelta >= ydelta) {
         rem = xdelta / 2;
         for (;;) {
             if (x1 == x2) break;
             x1 += xinc;
             rem += ydelta;
             if (rem >= xdelta) {
                rem -= xdelta;
                y1 += yinc;
             }
             set_pixel(x1, y1, color);
             if (x1 == x2) break;
         }
      } else {
         rem = ydelta / 2;
         for (;;) {
             if (y1 == y2) break;
             y1 += yinc;
             rem += xdelta;
             if (rem >= ydelta) {
                rem -= ydelta;
                x1 += xinc;
             }
             set_pixel(x1, y1, color);
             if (y1 == y2) break;
         }
      }
}


void rectangle (int x1,int y1,int x2,int y2,int color)
{
      line(x1,y1,x2,y1,color);
      line(x2,y1,x2,y2,color);
      line(x1,y2,x2,y2,color);
      line(x1,y1,x1,y2,color);
}


void fill_rectangle (int x1, int y1, int x2, int y2, int color)
{
      int idx;
      if (x1 > x2) {
         idx = x2;
         x2 = x1;
         x1 = idx;
      }
      if (y1 > y2) {
         idx = y2;
         y2 = y1;
         y1 = idx;
      }
      for (idx=y1;idx<(y2+1);idx++) {
          draw_horz_line(x1,x2,idx,color);
      }
}


void bar (int x1, int y1, int x2, int y2, int color)
{
      fill_rectangle(x1,y1,x2,y2,color);
}


void norm_3d (int x1,int y1,int x2,int y2,int color)
{
      bar(x1,y1,x2,y2,color);

      line(x1,y2,x1,y1,WHITE);
      line(x1 + 1,y2 - 1,x1 + 1,y1,WHITE);
      line(x1 + 2,y2 - 2,x1 + 2,y1,WHITE);
      line(x1,y1,x2,y1,WHITE);
      line(x1,y1 + 1,x2 - 1,y1 + 1,WHITE);
      line(x1,y1 + 2,x2 - 2,y1 + 2,WHITE);
      line(x1,y2,x2,y2,DARKGRAY);
      line(x1 + 1,y2 - 1,x2,y2 - 1,DARKGRAY);
      line(x1 + 2,y2 - 2,x2,y2 - 2,DARKGRAY);
      line(x2,y2,x2,y1,DARKGRAY);
      line(x2 - 1,y2,x2 - 1,y1 + 1,DARKGRAY);
      line(x2 - 2,y2,x2 - 2,y1 + 2,DARKGRAY);

}


void box_3d (int x1, int y1, int x2, int y2, int out_color, int in_color)
{
      bar(x1,y1,x2,y2,out_color);
      line(x1,y2,x1,y1,WHITE);
      line(x1 + 1,y2 - 1,x1 + 1,y1,WHITE);
      line(x1 + 2,y2 - 2,x1 + 2,y1,WHITE);
      line(x1,y1,x2,y1,WHITE);
      line(x1,y1 + 1,x2 - 1,y1 + 1,WHITE);
      line(x1,y1 + 2,x2 - 2,y1 + 2,WHITE);
      line(x1, y2,x2,y2,DARKGRAY);
      line(x1 + 1,y2 - 1,x2,y2 - 1,DARKGRAY);
      line(x1 + 2,y2 - 2,x2,y2 - 2,DARKGRAY);
      line(x2,y2,x2,y1,DARKGRAY);
      line(x2 - 1,y2,x2 - 1,y1 + 1,DARKGRAY);
      line(x2 - 2,y2,x2 - 2,y1 + 2,DARKGRAY);
      if ((y2 - y1) >= 40) {
         line(x1 + 7,y2 - 15,x1 + 7,y1 + 6,DARKGRAY);
         line(x1 + 8,y2 - 16,x1 + 8,y1 + 6,DARKGRAY);
         line(x1 + 7,y1 + 6,x2 - 7,y1 + 6,DARKGRAY);
         line(x1 + 7,y1 + 7,x2 - 8,y1 + 7,DARKGRAY);
         line(x1 + 7,y2 - 15,x2 - 7,y2 - 15,WHITE);
         line(x1 + 8,y2 - 16,x2 - 7,y2 - 16,WHITE);
         line(x2 - 7,y2 - 15,x2 - 7,y1 + 6,WHITE);
         line(x2 - 8,y2 - 15,x2 - 8,y1 + 7,WHITE);
         bar(x1 + 9,y1 + 7,x2 - 9,y2 - 17,in_color);
      }
}


void vert_3d (int x1, int y1, int x2, int y2, int color)
{
      bar(x1,y1,x2,y2,color);
      line(x1,y2,x1,y1,DARKGRAY);
      line(x1 + 1,y2 - 1,x1 + 1,y1,DARKGRAY);
      line(x1 + 2,y2 - 2,x1 + 2,y1,DARKGRAY);
      line(x1,y1,x2,y1,DARKGRAY);
      line(x1,y1 + 1,x2 - 1,y1 + 1,DARKGRAY);
      line(x1,y1 + 2,x2 - 2,y1 + 2,DARKGRAY);
      line(x1,y2,x2,y2,WHITE);
      line(x1 + 1,y2 - 1,x2,y2 - 1,WHITE);
      line(x1 + 2,y2 - 2,x2,y2 - 2,WHITE);
      line(x2,y2,x2,y1,WHITE);
      line(x2 - 1,y2,x2 - 1,y1 + 1,WHITE);
      line(x2 - 2,y2,x2 - 2,y1 + 2,WHITE);
}


void shade (int x1, int y1, int x2, int y2, int color)
{
      bar(x1 + 8,y1 + 6,x2 + 8,y2 + 6,BLACK);
      bar(x1,y1,x2,y2,color);
      rectangle(x1,y1,x2,y2,WHITE);
}


void two_line_3d (int x1, int y1, int x2, int y2, int in_color, int out_color)
{
      norm_3d(x1,y1,x2,y2,out_color);
      line(x1 + 7,y1 + 7,x1 + 7,y2 - 7,DARKGRAY);
      line(x1 + 7,y1 + 7,x2 - 7,y1 + 7,DARKGRAY);
      line(x1 + 9,y1 + 9,x1 + 9,y2 - 9,DARKGRAY);
      line(x1 + 9,y1 + 9,x2 - 9,y1 + 9,DARKGRAY);
      line(x1 + 7,y2 - 7,x2 - 7,y2 - 7,WHITE);
      line(x2 - 7,y2 - 7,x2 - 7,y1 + 7,WHITE);
      line(x1 + 9,y2 - 9,x2 - 9,y2 - 9,WHITE);
      line(x2 - 9,y2 - 9,x2 - 9,y1 + 9,WHITE);
      bar(x1 + 10,y1 + 10,x2 - 10,y2 - 10,in_color);

}


void double_3d (int x1, int y1, int x2, int y2, int color)
{
      line(x1,y2,x1,y1,DARKGRAY);
      line(x1 + 1,y2 - 1,x1 + 1,y1,DARKGRAY);
      line(x1 + 2,y2 - 2,x1 + 2,y1,DARKGRAY);
      line(x1,y1,x2,y1,DARKGRAY);
      line(x1,y1 + 1,x2 - 1,y1 + 1,DARKGRAY);
      line(x1,y1 + 2,x2 - 2,y1 + 2,DARKGRAY);
      line(x1,y2,x2,y2,WHITE);
      line(x1 + 1,y2 - 1,x2,y2 - 1,WHITE);
      line(x1 + 2,y2 - 2,x2,y2 - 2,WHITE);
      line(x2,y2,x2,y1,WHITE);
      line(x2 - 1,y2,x2 - 1,y1 + 1,WHITE);
      line(x2 - 2,y2,x2 - 2,y1 + 2,WHITE);
      bar(x1 + 3,y1 + 3,x2 - 3,y2 - 3,color);
      line(x1 + 3,y2 - 3,x1 + 3,y1 + 3,WHITE);
      line(x1 + 4,y2 - 4,x1 + 4,y1 + 3,WHITE);
      line(x1 + 3,y1 + 3,x2 - 3,y1 + 3,WHITE);
      line(x1 + 3,y1 + 4,x2 - 4,y1 + 4,WHITE);
      line(x1 + 3,y2 - 3,x2 - 3,y2 - 3,DARKGRAY);
      line(x1 + 4,y2 - 4,x2 - 3,y2 - 4,DARKGRAY);
      line(x2 - 3,y2 - 3,x2 - 3,y1 + 3,DARKGRAY);
      line(x2 - 4,y2 - 3,x2 - 4,y1 + 4,DARKGRAY);
}


void round_3d (int x1, int y1, int x2, int y2, int color)
{
      bar(x1,y1,x2,y2,color);
      line(x1,y2,x1,y1,WHITE);
      line(x1 + 1,y2 - 1,x1 + 1,y1,WHITE);
      line(x1,y1,x2,y1,WHITE);
      line(x1,y1 + 1,x2 - 1,y1 + 1,WHITE);
      line(x1,y2,x2,y2,DARKGRAY);
      line(x1 + 1,y2 - 1,x2,y2 - 1,DARKGRAY);
      line(x2,y2,x2,y1,DARKGRAY);
      line(x2 - 1,y2,x2 - 1,y1 + 1,DARKGRAY);
      line(x1 + 2,y1 + 15,x1 + 7,y1 + 15,WHITE);
      line(x1 + 15,y1 + 2,x1 + 15,y1 + 7,WHITE);
      line(x2 - 2,y1 + 15,x2 - 7,y1 + 15,WHITE);
      line(x2 - 15,y1 + 2,x2 - 15,y1 + 7,WHITE);
      line(x1 + 2,y2 - 15,x1 + 7,y2 - 15,WHITE);
      line(x1 + 15,y2 - 2,x1 + 15,y2 - 7,WHITE);
      line(x2 - 2,y2 - 15,x2 - 7,y2 - 15,WHITE);
      line(x2 - 15,y2 - 2,x2 - 15,y2 - 7,WHITE);
      line(x2 - 8,y1 + 8, x2 - 8, y2 - 8,WHITE);
      line(x2 - 8,y2 - 8, x1 + 8, y2 - 8,WHITE);
      line(x1 + 2,y1 + 14,x1 + 7,y1 + 14,DARKGRAY);
      line(x1 + 14,y1 + 2,x1 + 14,y1 + 7,DARKGRAY);
      line(x2 - 2,y1 + 14,x2 - 7,y1 + 14,DARKGRAY);
      line(x2 - 14,y1 + 2,x2 - 14,y1 + 7,DARKGRAY);
      line(x1 + 2,y2 - 14,x1 + 7,y2 - 14,DARKGRAY);
      line(x1 + 14,y2 - 2,x1 + 14,y2 - 7,DARKGRAY);
      line(x2 - 2,y2 - 14,x2 - 7,y2 - 14,DARKGRAY);
      line(x2 - 14,y2 - 2,x2 - 14,y2 - 7,DARKGRAY);
      line(x1 + 8,y1 + 8,x2 - 8,y1 + 8,DARKGRAY);
      line(x1 + 8,y1 + 8,x1 + 8,y2 - 8,DARKGRAY);
}


void inline_3d (int x1, int y1, int x2, int y2, int color)
{
      line(x1,y1,x1,y2,DARKGRAY);
      line(x1,y1,x2,y1,DARKGRAY);
      line(x1 + 2,y1 + 2,x1 + 2,y2 - 2,DARKGRAY);
      line(x1 + 2,y1 + 2,x2 - 2,y1 + 2,DARKGRAY);
      line(x1,y2,x2,y2,WHITE);
      line(x2,y2,x2,y1,WHITE);
      line(x1 + 2,y2 - 2,x2 - 2,y2 - 2,WHITE);
      line(x2 - 2,y2 - 2,x2 - 2,y1 + 2,WHITE);
      bar(x1 +3,y1 + 3,x2 - 3,y2 - 3,color);
}


void line_3d (int x1, int y1, int x2, int y2)
{
      line(x1,y1,x1,y2,DARKGRAY);
      line(x1,y1,x2,y1,DARKGRAY);
      line(x1,y2,x2,y2,WHITE);
      line(x2,y2,x2,y1,WHITE);
}


void demo_box (void)
{
      lcd_clear(background_color);
      switch (demo) {
    	 case 0:
           lcd_printf(1,1,"bar()");
           bar(20, 40, 220, 300, RED);
           break;
    	 case 1:
           lcd_printf(1,1,"norm_3d()");
           norm_3d (20, 40, 220, 300, LIGHTGRAY);
           break;
    	 case 2:
           lcd_printf(1,1,"box_3d()");
           box_3d (20, 40, 220, 300, LIGHTGRAY, BLACK);
           break;
    	 case 3:
           lcd_printf(1,1,"vert_3d()");
           vert_3d (20, 40, 220, 300, LIGHTGRAY);
           break;
    	 case 4:
           lcd_printf(1,1,"shade()");
           shade (20, 40, 220, 300, GREEN);
           break;
	 case 5:
	   lcd_printf(1,1,"two_line_3d()");
           two_line_3d (20, 40, 220, 300, BLUE, RED);
           break;
	 case 6:
	   lcd_printf(1,1,"double_3d()");
           double_3d (20, 40, 220, 300, LIGHTGRAY);
           break;
	 case 7:
	   lcd_printf(1,1,"round_3d()");
           round_3d (20, 40, 220, 300, WHITE);
           break;
	 case 8:
	   lcd_printf(1,1,"inline_3d()");
           inline_3d (20, 40, 220, 300, LIGHTGRAY);
           break;
	 case 9:
	   lcd_printf(1,1,"line_3d()");
           line_3d (20, 40, 220, 300);
           break;
      }
      demo++;
      if (demo >= 10) demo = 0;
}