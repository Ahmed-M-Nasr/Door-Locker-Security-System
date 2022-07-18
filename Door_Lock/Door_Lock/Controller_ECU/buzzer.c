/*
 * buzzer.c
 *
 *  Created on: Jul 3, 2022
 *      Author: future
 */
#include"buzzer.h"
#include"gpio.h"
#include "common_macros.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void Buzzer_Init(void){
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);
}
void Buzzer_On(void){
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_HIGH);
}
void Buzzer_Off(void){
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);
}
