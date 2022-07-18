 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the AVR Timer driver
 *
 * Author: Ahmed Nasr
 *
 *******************************************************************************/
#include "timer.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h> /* To use Timer Registers */
#include <avr/interrupt.h> /* For Timer ISR */
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
static uint16 ticks=0;
uint16 Ticks=0;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{ticks++;
if(ticks==Ticks){
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}}
}
ISR(TIMER0_COMP_vect)
{ticks++;
	if(ticks==Ticks){
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		Ticks=0;
	}
	}}

void Timer_init(const Timer_ConfigType * Config_Ptr){
	SREG |= (1<<7); // Global interrupt enable
	Timer_setCallBack(Config_Ptr->a_ptr); // set the callback function from the config structure
	TCNT0 = 0; // Set Timer Initial Value 0
	Ticks= Config_Ptr->Ticks2;// set the number of overflows from the config structure
	OCR0 = Config_Ptr->compare_value; // set the compare value from the config structure
	TIMSK |= (1<<(Config_Ptr->mode));// enable the timer mode overflow/compare from the config structure
	/* Configure timer control register
	 * 1. Non PWM mode FOC0=1
	 * 2. set the prescaler from the config structure
	 */
	TCCR0 = (1<<FOC0) | ((Config_Ptr->mode)<<WGM01) | ((Config_Ptr->output)<<COM00);
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->clock);


}
void Timer_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
/*function to disable the timer*/
void Timer_dinit(void){
	Ticks=0; //clear ticks
	ticks=0; //clear Ticks
	TCNT0 = 0; //reset the timer value to zero
	TIMSK = (TCCR0 & 0xFC); //disable the overflow & compare mode interrupt
	TIFR |= (1<<TOV0); //clear the interrupt flag
	SREG &= ~(1<<7); // disable the global interrupt
}
