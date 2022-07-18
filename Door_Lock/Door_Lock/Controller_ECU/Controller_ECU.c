 /******************************************************************************
 *
 * Module: APP
 *
 * File Name: Controller_ECU.c
 *
 * Description: Application for Controller ECU
 *
 * Author: Ahmed Nasr
 *
 *******************************************************************************/
#include "timer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "buzzer.h"
#include "uart.h"
#include<util/delay.h>
#include <avr/io.h>
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Door(void);
void Timer_CallBackFunction(void);
void New_Pass(void);
uint8 Check_Pass(void);
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
//pointer used for the eeprom memory
uint16 *add;
// Timer flag used for polling
uint8 flag=1;
// config structure for timer for the door opening and closing time
Timer_ConfigType Timer_Config_Door = {F_CPU_1024,OVERFLOW,NORMAL,0,Timer_CallBackFunction,50};
// config structure for timer for the door stop time
Timer_ConfigType Timer_Config_Door_Stop = {F_CPU_1024,OVERFLOW,NORMAL,0,Timer_CallBackFunction,10};
// config structure for timer for the Alarm time
Timer_ConfigType Timer_Config_Buzzer = {F_CPU_1024,OVERFLOW,NORMAL,0,Timer_CallBackFunction,200};
int main(void)
{
	// variable to check the number of trials of incorrect password
	uint8 Trials=0;
	//global interrupt enable
	SREG |= (1<<7);
	// variable for password check
	uint8 check=1;
	// variable for acknowledgment of byte received by UART
	uint8 key;
	//Counter for the number of digits in password
	uint8 Counter=0;
	//variables used for checking
	uint8 c;
	uint8 c2;
	//start address for saving data into eeprom memory
	add=0x0311;
	//variable to save data after reading it from eeprom memory
	uint8 val;
	/* Initialize the dc motor driver */
	DcMotor_Init();
	/* Initialize the buzzer driver */
	Buzzer_Init();
	/* Initialize the eeprom with I2C driver */
	EEPROM_init();
	//config structure for UART Initialization
	Uart_ConfigType Uart_Config = {Double_speed,Asynch,Disabled,bit,9600};
	/* Initialize the UART driver with config structure */
	UART_init(&Uart_Config);
	while(1){
	while(1){
	add=0x0311;
	New_Pass();
	add=0x0311;
	c=Check_Pass();
	/*if check password is correct 'T' sent by UART to HMI ECU*/
	if(c==0){
		UART_sendByte('F');
		break;}
	else{ UART_sendByte('T');}
	add=0x0311;
	while(1){
    add=0x0311;
	check=1;
	Counter=0;
	while(Counter<=3){
		key=UART_recieveByte();
		if(key=='N'){
		c2=Check_Pass();
		if(c2!=0){
		UART_sendByte('T');
		break;
		}else {UART_sendByte('s');}
		}else {
		EEPROM_readByte(add,&val);
		if(key!=val){
			check=0;}
		add++;
		Counter++;}}
	if(key!='N'){
	if(check==1){
	UART_sendByte(1);
	Door();
    }else {
	UART_sendByte(0);
	Trials=UART_recieveByte();
	if(Trials=='x'){
	Buzzer_On();
	Timer_init(&Timer_Config_Buzzer);
	while(flag==1){}
	flag=1;
	Buzzer_Off();}
	}}else break;}}}}
/*Function to do the door opening and closing*/
void Door(void){
	/*Rotate motor ccw*/
	DcMotor_Rotate(CCW);
	//initiate the timer driver with door config with 15 seconds time
	Timer_init(&Timer_Config_Door);
	while(flag==1){}
	flag=1;
	DcMotor_Rotate(STOP);
	//initiate the timer driver with door stop config with 5 seconds time
	Timer_init(&Timer_Config_Door_Stop);
	while(flag==1){}
	flag=1;
	DcMotor_Rotate(CW);
	//initiate the timer driver with door config with 15 seconds time
	Timer_init(&Timer_Config_Door);
	while(flag==1){}
	flag=1;
	DcMotor_Rotate(STOP);
}
/*Funtion to receive data from UART and save it to the eeprom memory*/
void New_Pass(void){
	/*Array to save the digits received from UART*/
	uint8 pass[4]={0};
	/*Counter for the number of digits in password*/
	uint8 counter=0;
	while(counter<=3){
	pass[counter]= UART_recieveByte();
	EEPROM_writeByte(add,pass[counter]); /* Write digit in the external EEPROM */
	_delay_ms(10);
	counter++;
	add++;
	}
}
/*Function to check if the password received from UART match with the one saved in the eeprom memory and return
 * the check value */
uint8 Check_Pass(void){
	/*Array to save the digits received from UART*/
	uint8 pass[4]={0};
	/*Counter for the number of digits in password*/
	uint8 counter=0;
	/*variable to check if one digit doesn't match it is cleared*/
	uint8 check=1;
	//variable to save data after reading it from eeprom memory
	uint8 val;
	/*move pointer to the start address*/
	add=0x0311;
	while(counter<=3){
	pass[counter]= UART_recieveByte();
	EEPROM_readByte(add,&val);
	if(val!=pass[counter]){
		check=0;
	}
	counter++;
	add++;
	}
	return check;
}
void Timer_CallBackFunction(void){
	//Uninitialize the timer driver
	Timer_dinit();
	//clear the flag to get out of polling
	flag=0;
}
