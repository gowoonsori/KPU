/********************************************************************************/
/* print.c                                                                      */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_test								*/
/********************************************************************************/
#include <stdarg.h>

//void _stfp(double d, void *x);
unsigned int strlen (const char *str);
int errno;

typedef struct {
      char *ptr;
} StringOutputFile;

int _serror(char *f);


int _sputc(int ch, char *fp)
{
      StringOutputFile *sf;
      sf = (void *)fp;
      char *op = sf->ptr;
      int r = *op++ = ch;
      sf->ptr = op;
      return r;
}


#define IGNORE(x) ((x)=(x))
#define DecimalPoint '.'


int _serror(char *f)
{
      IGNORE(f);
      return 0;
}


typedef struct {
      int flags;
      char *prefix;
      int precision;
      int before_dot;
      int after_dot;
      double d;
} fp_print_rec;


typedef int fp_print(int ch, char buff[], fp_print_rec *p);


typedef struct {
      fp_print_rec fpr;
      fp_print *fn;
      int (*putc)(int, char *);
      int (*ferror)(char *);
      char *hextab;
      int width;
      unsigned long long llval;
} printf_display_rec;

#define _LJUSTIFY         0x1
#define _SIGNED           0x2
#define _BLANKER          0x4
#define _VARIANT          0x8
#define _PRECGIVEN       0x10
#define _LONGSPEC        0x20
#define _LLSPEC          0x40
#define _SHORTSPEC       0x80
#define _PADZERO        0x100    /* *** DEPRECATED FEATURE *** */
#define _FPCONV         0x200
#define intofdigit(x) ((x)-'0')

#define xputc(dr, ch, f) (dr->putc(ch, f))

#define pr_padding(dr, ch, n, f)  while (--n>=0) charcount++, xputc(dr, ch, f);

#define pre_padding(dr, f)                                                \
        if (!(flags&_LJUSTIFY))                                           \
        {   char padchar = flags & _PADZERO ? '0' : ' ';                  \
            pr_padding(dr, padchar, width, f); }

#define post_padding(dr, f)                                               \
        if (flags&_LJUSTIFY)                                              \
        {   pr_padding(dr, ' ', width, f); }
#define FLOATING_WIDTH 18
typedef enum { fp_ord, fp_nan, fp_inf } fp_type;

int is_digit (int c)
{
      return (unsigned int)(c - '0') <= 9;
}


static int printf_display (char *p, int ch, printf_display_rec *dr, unsigned int v)
{
      int len = 0, charcount = 0;
      char buff[32];       /* used to accumulate value to print    */
      int flags = dr->fpr.flags;
      dr->fpr.before_dot = -1, dr->fpr.after_dot = -1;
      if (!(dr->fpr.flags & (_FPCONV + _PRECGIVEN))) dr->fpr.precision = 1;
      switch (ch) {
	 case 'p':
	 case 'X':
	 case 'x':
	   if (flags & _LLSPEC) {
	      unsigned long long w = dr->llval;
	      while (w != 0) {
		    buff[len++] = dr->hextab[(int)w & 0xf];
		    w = w >> 4;
	      }
	   } else {
	      while (v != 0) {
		    buff[len++] = dr->hextab[v & 0xf];
		    v = v >> 4;
	      }
	   }
	   break;
	 case 'o':
	   if (flags & _LLSPEC) {
	      unsigned long long w = dr->llval;
	      while (w != 0) {
		    buff[len++] = dr->hextab[(int)w & 7];
		    w = w >> 3;
	      }
	   } else {
	      while (v != 0) {
		    buff[len++] = '0' + (v & 7);
		    v = v >> 3;
	      }
	   }
	   if (flags & _VARIANT) dr->fpr.precision--;
	   break;
	 case 'u':
	 case 'i':
	 case 'd':
	   if (flags & _LLSPEC) {
	      unsigned long long w = dr->llval;
	      while (w != 0) {
		    buff[len++] = '0' + (char)(w % 10);
                    w = w / 10;
              }
           } else {
              while (v != 0) {
		    buff[len++] = '0' + (char)(v % 10);
		    v = v / 10;
	      }
	   }
	   break;
         default:
	   buff[0] = '0';
           buff[1] = DecimalPoint;
           buff[2] = '0';
           len = 3;
           break;
      }
      {
      int precision;
      int width = dr->width;
      if (flags & _FPCONV) {
	 precision = 0;
	 if (dr->fpr.before_dot>0) precision = dr->fpr.before_dot-1;
	 if (dr->fpr.after_dot>0) precision += dr->fpr.after_dot-1;
      } else {
         precision = dr->fpr.precision;
         if ((precision -= len)<0) precision = 0;
      }
      width -= (precision + len + strlen(dr->fpr.prefix));
      if (!(flags & _PADZERO)) pre_padding(dr, p);
      {
      int c;                                      /* prefix    */
      char *prefix = dr->fpr.prefix;
      for (; (c=*prefix++)!=0; charcount++) xputc(dr, c, p);
      }
      pre_padding(dr, p);
      if (flags & _FPCONV) {
	 int i, c;
         for (i = 0; i<len; i++) {
	     switch (c = buff[i]) {
                case '<':
		  pr_padding(dr, '0', dr->fpr.before_dot, p);
		  break;
		case '>':
	       	  pr_padding(dr, '0', dr->fpr.after_dot, p);
	          break;
	        default:
	      	  xputc(dr, c, p);
	          charcount++;
	          break;
	     }
	 }
      } else {
	 pr_padding(dr, '0', precision, p);
	 charcount += len;
         while (len-- > 0) xputc(dr, buff[len], p);
      }
      post_padding(dr, p);
      return charcount;
      }
}


int __vfprintf (char *p, const char *fmt, va_list args, printf_display_rec *dr)
{
      int ch, charcount = 0;
      while ((ch = *fmt++) != 0) {
      	    if (ch != '%') {
      	       xputc(dr, ch, p);
      	       charcount++;
      	    } else {
      	       int flags = 0, width = 0;
               unsigned int v = 0;
               for (;;) {
                   switch (ch = *fmt++) {
                      case '-':   flags = _LJUSTIFY | (flags & ~_PADZERO);
                        continue;
                      case '+':   flags |= _SIGNED;
                        continue;
                      case ' ':   flags |= _BLANKER;
                        continue;
                      case '#':   flags |= _VARIANT;
                        continue;
                      case '0':   flags |= _PADZERO;
                        continue;
                      default:
                        break;
                   }
                   break;
               }
               {
               int t = 0;
               if (ch == '*') {
                  t = va_arg(args, int);
                  if (t < 0) {
                     t = - t;
                     flags ^= _LJUSTIFY;
                  }
                  ch = *fmt++;
               } else {
               	  while (is_digit(ch)) {
                        t = t*10 + intofdigit(ch);
                        ch = *fmt++;
                  }
               }
               width = t>=0 ? t : 0;                 /* disallow -ve arg */
               }
               if (ch == '.') {                           /* precision spec */
                  int t = 0;
                  ch = *fmt++;
                  if (ch == '*') {
                     t = va_arg(args, int);
                     ch = *fmt++;
                  } else {
                     while (is_digit(ch)) {
                           t = t*10 + intofdigit(ch);
                           ch = *fmt++;
                     }
                  }
                  if (t >= 0) flags |= _PRECGIVEN, dr->fpr.precision = t;
               }
               if (ch == 'l' || ch == 'L') {
                  ch = *fmt++;
                  if (ch == 'l' || ch == 'L') {
                     flags |= _LLSPEC;
                     ch = *fmt++;
                  } else {
                    flags |= _LONGSPEC;
                  }
               } else if (ch == 'h') {
                  flags |= _SHORTSPEC;
                  ch = *fmt++;
               }
               switch (ch) {
                  case 'c':
                    ch = va_arg(args, int);
                  default:
                    width--;                        /* char width is 1       */
                    pre_padding(dr, p);
                    xputc(dr, ch, p);
                    charcount++;
                    post_padding(dr, p);
                    continue;
                  case 0:     fmt--;
                    continue;
                  case 'n':
                    if (flags & _SHORTSPEC) *va_arg(args, short *) = (short)charcount;
                    else if (flags & _LLSPEC) *va_arg(args, long long *) = charcount;
                    else if (flags & _LONGSPEC) *va_arg(args, long *) = charcount;
                    else *va_arg(args, int *) = charcount;
                    continue;
                  case 's':
                  {
                    char *str = va_arg(args, char *);
                    int i, n;
                    if (flags & _PRECGIVEN) {
                       int precision = dr->fpr.precision;
                       for (n = 0; n < precision && str[n] != 0; n++) continue;
                    } else n = strlen(str);
                    width -= n;
                    pre_padding(dr, p);
                    for (i=0; i<n; i++) xputc(dr, str[i], p);
                    charcount += n;
                    post_padding(dr, p);
                  }
                    continue;
                  case 'X':
                    if (flags & _LLSPEC) dr->llval = va_arg(args, long long);else v = va_arg(args, int);
                    if (flags & _SHORTSPEC) v = (unsigned short)v;
                    dr->hextab = "0123456789ABCDEF";
                    dr->fpr.prefix = (flags&_VARIANT) ? "0X" : "";
                    if (flags & _PRECGIVEN) flags &= ~_PADZERO;
                    break;
                  case 'x':
                    if (flags & _LLSPEC) dr->llval = va_arg(args, long long);else v = va_arg(args, int);
                    if (flags & _SHORTSPEC) v = (unsigned short)v;
                    dr->hextab = "0123456789abcdef";
                    dr->fpr.prefix = (flags&_VARIANT) ? "0x" : "";
                    if (flags & _PRECGIVEN) flags &= ~_PADZERO;
                    break;
                  case 'p':
                    v = (unsigned int)va_arg(args, void *);
                    dr->hextab = "0123456789abcdef";
                    dr->fpr.prefix = (flags&_VARIANT) ? "@" : "";
                    dr->fpr.precision = 8;
                    flags |= _PRECGIVEN;
                    break;
                  case 'o':
                    if (flags & _LLSPEC) dr->llval = va_arg(args, long long);else v = va_arg(args, int);
                    if (flags & _SHORTSPEC) v = (unsigned short)v;
                    dr->fpr.prefix = (flags&_VARIANT) ? "0" : "";
                    if (flags & _PRECGIVEN) flags &= ~_PADZERO;
                    break;
                  case 'u':
                    if (flags & _LLSPEC) dr->llval = va_arg(args, long long);else v = va_arg(args, unsigned int);
                    if (flags & _SHORTSPEC) v = (unsigned short)v;
                    dr->fpr.prefix = "";
                    if (flags & _PRECGIVEN) flags &= ~_PADZERO;
                    break;
                  case 'i':
                  case 'd':
                  {
                    int w;
                    if (flags & _LLSPEC) {
                       dr->llval = va_arg(args, long long);
                       if ((long long)dr->llval < 0) {
                          dr->llval = -dr->llval;
                          w = -1;
                       } else w = 0;
                    } else {
                       w = va_arg(args, int);
                       if (flags & _SHORTSPEC) w = (signed short)w;
                       v = (w < 0) ? -w : w;
                    }
                    dr->fpr.prefix = (w < 0) ? "-" : (flags & _SIGNED) ? "+" : (flags & _BLANKER) ? " " : "";
                  }
                    if (flags & _PRECGIVEN) flags &= ~_PADZERO;
                    break;
                  case 'f':
                  case 'e':
                  case 'E':
                  case 'g':
                  case 'G':
                    // Not SUPPORT
                    flags |= _FPCONV;
                    if (!(flags & _PRECGIVEN)) dr->fpr.precision = 6;
                    {
                    int w = va_arg(args, int);
                    w = va_arg(args, int);
                    dr->fpr.prefix = (flags&_SIGNED) ? "+" : (flags&_BLANKER) ? " " : "";
                    if (w) w = 0;
                    }
                    break;
               }
               dr->width = width;
               dr->fpr.flags = flags;
               charcount += printf_display(p, ch, dr, v);
               continue;
            }
      }
      return dr->ferror(p) ? 0 : charcount;
}


int sprintf (char *buff, const char *fmt, ...)
{
      StringOutputFile sf;
      va_list a;
      int length;
      printf_display_rec dr;
      va_start(a, fmt);
      sf.ptr = buff;
      dr.putc = _sputc;
      dr.ferror = _serror;
      dr.fn = 0;
      length = __vfprintf((char *)&sf, fmt, a, &dr);
      _sputc(0, (char *)&sf);
      va_end(a);
      return length;
}


int vsprintf (char *buff, const char *fmt, va_list a)
{
      StringOutputFile sf;
      int length;
      printf_display_rec dr;
      dr.putc = _sputc;
      dr.ferror = _serror;
      dr.fn = 0;
      sf.ptr = buff;
      length = __vfprintf((char *)&sf, fmt, a, &dr);
      _sputc(0, (char *)&sf);
      return length;
}
