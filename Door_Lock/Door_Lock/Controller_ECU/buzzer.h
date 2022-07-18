 /******************************************************************************
 *
 * Module: buzzer
 *
 * File Name:buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Ahmed Nasr                */

#ifndef BUZZER_H_
#define BUZZER_H_
#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT             PORTA_ID
#define BUZZER_PIN                 0
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);


#endif /* BUZZER_H_ */
