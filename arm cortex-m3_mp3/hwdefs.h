/********************************************************************************/
/* hwdefs.h                                                                     */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#ifndef __HWDEFS_H
#define __HWDEFS_H

#ifndef __GNUC__
#define __GNUC__
#endif

#ifndef ARM_MATH_CM3
#define ARM_MATH_CM3
#endif

#define assert_param(X)		{}

#include <cmsis/stm32f1xx.h>
#include <fundefs.h>
#include "userdefs.h"

#endif	/* __HWDEFS_H */