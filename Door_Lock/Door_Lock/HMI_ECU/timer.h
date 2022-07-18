 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for the AVR Timer driver
 *
 * Author: Ahmed Nasr
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_
#include "std_types.h"
typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer_Clock;

typedef enum
{
	OVERFLOW,COMPARE
}Timer_Mode;
typedef enum
{
	NORMAL,TOGGLE,CLEAR,SET
}Compare_Pin;
typedef struct
{
	Timer_Clock clock;
	Timer_Mode mode;
	Compare_Pin output;
	uint8 compare_value;
	void(*a_ptr)(void);
	uint8 Ticks2;
}Timer_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*funtion to initiate timer 0*/
void Timer_init(const Timer_ConfigType * Config_Ptr);
/*funtion to set the call back funtion*/
void Timer_setCallBack(void(*a_ptr)(void));
/*funtion to uninitiate timer 0*/
void Timer_dinit(void);



#endif /* TIMER_H_ */
