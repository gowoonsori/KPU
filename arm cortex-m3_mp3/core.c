/********************************************************************************/
/* core.c                                                                       */
/* STM32F10X                                                                    */
/* ÀÌ Ã¢¿ì (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 */
/*                                                                              */
/********************************************************************************/
#define SBRK_VERBOSE 1

#include <reent.h>
#include <errno.h>
#include <stdlib.h> /* abort */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

extern void s_printf (char *form,...);
/*
unsigned int __get_PSP (void) __attribute__( ( naked ) );
unsigned int __get_PSP (void)
{
      unsigned int result = 0;
      __asm volatile ("MRS %0, psp\n\t" "MOV r0, %0 \n\t" "BX  lr     \n\t"  : "=r" (result) );
      return(result);
}


void __set_PSP (unsigned int topOfProcStack) __attribute__( ( naked ) );
void __set_PSP (unsigned int topOfProcStack)
{
      __asm volatile ("MSR psp, %0\n\t" "BX  lr     \n\t" : : "r" (topOfProcStack) );
}


void __set_MSP (unsigned int topOfMainStack) __attribute__( ( naked ) );
void __set_MSP (unsigned int topOfMainStack)
{
      __asm volatile ("MSR msp, %0\n\t" "BX  lr     \n\t" : : "r" (topOfMainStack) );
}


unsigned int __get_BASEPRI (void)
{
      unsigned int result=0;
      __asm volatile ("MRS %0, basepri_max" : "=r" (result) );
      return(result);
}


void __set_BASEPRI (unsigned int value)
{
      __asm volatile ("MSR basepri, %0" : : "r" (value) );
}


unsigned int __get_PRIMASK (void)
{
       unsigned int result=0;
      __asm volatile ("MRS %0, primask" : "=r" (result) );
      return(result);
}


void __set_PRIMASK (unsigned int priMask)
{
      __asm volatile ("MSR primask, %0" : : "r" (priMask) );
}


unsigned int __get_FAULTMASK (void)
{
      unsigned int result = 0;
      __asm volatile ("MRS %0, faultmask" : "=r" (result) );
      return(result);
}


void __set_FAULTMASK (unsigned int faultMask)
{
      __asm volatile ("MSR faultmask, %0" : : "r" (faultMask) );
}


unsigned int __get_CONTROL (void)
{
      unsigned int result = 0;
      __asm volatile ("MRS %0, control" : "=r" (result) );
      return(result);
}


void __set_CONTROL (unsigned int control)
{
      __asm volatile ("MSR control, %0" : : "r" (control) );
}


unsigned int __REV (unsigned int value)
{
      unsigned int result = 0;
      __asm volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
      return(result);
}


unsigned int __REV16 (unsigned short value)
{
      unsigned int result = 0;
      __asm volatile ("rev16 %0, %1" : "=r" (result) : "r" (value) );
      return(result);
}


int __REVSH (short value)
{
      unsigned int result=0;
      __asm volatile ("revsh %0, %1" : "=r" (result) : "r" (value) );
      return(result);
}


unsigned int __RBIT (unsigned int value)
{
      unsigned int result=0;
      __asm volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
      return(result);
}


unsigned char __LDREXB (unsigned char *addr)
{
      unsigned char result=0;
      __asm volatile ("ldrexb %0, [%1]" : "=r" (result) : "r" (addr) );
      return(result);
}


unsigned short __LDREXH (unsigned short *addr)
{
      unsigned short result = 0;
      __asm volatile ("ldrexh %0, [%1]" : "=r" (result) : "r" (addr) );
      return(result);
}


unsigned int __LDREXW (unsigned int *addr)
{
      unsigned int result = 0;
      __asm volatile ("ldrex %0, [%1]" : "=r" (result) : "r" (addr) );
      return(result);
}


unsigned int __STREXB (unsigned char value, unsigned char *addr)
{
      unsigned int result = 0;
      __asm volatile ("strexb %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
      return(result);
}


unsigned int __STREXH (unsigned short value, unsigned short *addr)
{
      unsigned int result = 0;
      __asm volatile ("strexh %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
      return(result);
}


unsigned int __STREXW (unsigned int value, unsigned int *addr)
{
      unsigned int result = 0;
      __asm volatile ("strex %0, %2, [%1]" : "=r" (result) : "r" (addr), "r" (value) );
      return(result);
}
*/


unsigned int __get_MSP (void) __attribute__( ( naked ) );
unsigned int __get_MSP (void)
{
      unsigned int result=0;
      __asm volatile ("MRS %0, msp\n\t" "MOV r0, %0 \n\t" "BX  lr     \n\t"  : "=r" (result) );
      return(result);
}


int _kill (int pid, int sig)
{
      pid = pid; sig = sig;		/* avoid warnings */
      errno = EINVAL;
      return -1;
}


void _exit (int status)
{
      s_printf("_exit called with parameter %d \r\n", status);
      while(1) {;}
}


int _getpid (void)
{
      return 1;
}


extern char _end;			/* Defined by the linker */
static char *heap_end;


char *get_heap_end (void)
{
      return (char*) heap_end;
}


char *get_stack_top (void)
{
      return (char*) __get_MSP();
}


caddr_t _sbrk (int incr)
{
      char *prev_heap_end;
#if SBRK_VERBOSE
      s_printf("_sbrk called with incr %d \r\n", incr);
#endif
      if (heap_end == 0) {
	 heap_end = &_end;
      }
      prev_heap_end = heap_end;
#if 1
      if (heap_end + incr > get_stack_top()) {
	  s_printf("Heap and stack collision \r\n");
         abort();
      }
#endif
      heap_end += incr;
      return (caddr_t) prev_heap_end;
}


int _close (int file)
{
      file = file;			/* avoid warning */
      return -1;
}


int _fstat (int file, struct stat *st)
{
      file = file;			/* avoid warning */
      st->st_mode = S_IFCHR;
	return 0;
}


int _isatty (int file)
{
      file = file;			/* avoid warning */
      return 1;
}


int _lseek (int file, int ptr, int dir)
{
      file = file;			/* avoid warning */
      ptr = ptr;			/* avoid warning */
      dir = dir;			/* avoid warning */
      return 0;
}


int _read (int file, char *ptr, int len)
{
      file = file;			/* avoid warning */
      ptr = ptr;			/* avoid warning */
      len = len;			/* avoid warning */
      return 0;
}


int _write (int file, char *ptr, int len)
{
      int todo;
      file = file;			/* avoid warning */
      for (todo=0;todo<len;todo++) {
          s_printf("%c",(*ptr++));
      }
      return len;
}
