 /******************************************************************************
 *
 * Module: APP
 *
 * File Name: HMI_ECU.c
 *
 * Description: Application for HMI ECU
 *
 * Author: Ahmed Nasr
 *
 *******************************************************************************/
#include "timer.h"
#include "lcd.h"
#include "uart.h"
#include "keypad.h"
#include<util/delay.h>
#include <avr/io.h>
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Get_Pass(void);
void New_Pass(void);
void Timer_CallBackFunction(void);
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
// Timer flag used for polling
uint8 flag=1;
int main(void)
{
	// variable for acknowledgment of byte received by UART
	uint8 ack=0;
	//variable to receive the pressed key from keypad and check it
	uint8 k;
	// variable for acknowledgment of byte received by UART
	uint8 Check;
	// variable to check the number of trials of incorrect password
	uint8 Trials=0;
	// config structure for timer for the door opening and closing time
	Timer_ConfigType Timer_Config_Door = {F_CPU_1024,OVERFLOW,NORMAL,0,Timer_CallBackFunction,50};
	// config structure for timer for alarm time
	Timer_ConfigType Timer_Config_Alarm = {F_CPU_1024,OVERFLOW,NORMAL,0,Timer_CallBackFunction,200};
	Uart_ConfigType Uart_Config = {Double_speed,Asynch,Disabled,bit,9600};
	/* Initialize the UART driver with config structure*/
	UART_init(&Uart_Config);
	/* Initialize the UART driver*/
	LCD_init();
	while(1){
	while(1){
	LCD_clearScreen();
	//Function to get the new password from user twice*/
	New_Pass();
	/*check if the two passwords match by receiving acknowledgment from controller ECU*/
	Check=UART_recieveByte();
	//check if the two passwords doesn't match start over from the beginning
	if(Check=='F'){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Does Not Match");
	_delay_ms(2000);
	LCD_clearScreen();
	break;}
	while(1){
	//main menu always return to it after each cycle
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+:open door");
	LCD_displayStringRowColumn(1,0,"-:change password");
	//get pressed key from user then save it in a variable to check it
	k = KEYPAD_getPressedKey();
	_delay_ms(1000);
	LCD_clearScreen();
	LCD_moveCursor(1,0);
	//check the pressed key if '+' takes password from user
	if(k=='+'){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter password");
	_delay_ms(1000);
	//Function that get the key pressed by user and send it to Controller ECU by UART
	Get_Pass();
	LCD_clearScreen();
	//wait to receive acknowledgment from user to check if the password is correct
	// if true starts the door opening and closing
	ack = UART_recieveByte();
	if(ack==1){
	LCD_displayStringRowColumn(0,0,"Door opening");
	//initiate the timer driver with door config with 15 seconds time
	Timer_init(&Timer_Config_Door);
	while(flag==1){}
	flag=1;
	Trials=0;
	LCD_displayStringRowColumn(0,0,"Door closing");
	//initiate the timer driver with door config with 15 seconds time
	Timer_init(&Timer_Config_Door);
	while(flag==1){}
	flag=1;
	}
	//if password doesn't match display incorrect and start the loop again
	else {
		LCD_displayStringRowColumn(0,0,"Incorrect");
		//increment the trials number every time user enter incorrect password
		Trials++;
		//if trials reaches 3 send 'x' to Controller ECU to start the alarm
		if(Trials==3){
			UART_sendByte('x');
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,6,"Alarm");
			//initiate the timer driver with alarm config with 60 seconds time
			Timer_init(&Timer_Config_Alarm);
			while(flag==1){}
			flag=1;
			Trials=0;
		}else { UART_sendByte(0);}
	}
	_delay_ms(1000);
	}
	//if the pressed key is '-' send 'N' to controller to start the new password code
	else if(k=='-'){
		UART_sendByte('N');
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Enter old pass");
		//Function that get the key pressed by user and send it to Controller ECU by UART
		Get_Pass();
		//wait to receive acknowledgment from user to check if the password is correct
		// if true breaks from loop and start the new password
		ack = UART_recieveByte();
		if(ack=='T'){break;}
		else {
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Incorrect");
		_delay_ms(1000);
	}
	}}}}}

//Function that get the key pressed by user and send it to Controller ECU by UART
void Get_Pass(void){
	//variable to receive the pressed key from keypad and check it
	uint8 key;
	//counter to the number of digits in password
	uint8 Counter=0;
	LCD_moveCursor(1,0);
	while(Counter<=3){
		/* if any switch pressed for more than 500 ms it counts more than one press */
		key = KEYPAD_getPressedKey();
		Counter++;
		UART_sendByte(key);
		/* get the pressed key number */
		if((key <= 9) && (key >= 0))
		{
			LCD_displayCharacter('*'); /* display '*' with each press */
		}
		_delay_ms(500); /* Press time */
    }}
//Function to get the new password from user twice*/
void New_Pass(void){
	LCD_displayStringRowColumn(0,0,"Make password");
	LCD_moveCursor(1,0);
	//Function that get the key pressed by user and send it to Controller ECU by UART
	Get_Pass();
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Reenter password");
	LCD_moveCursor(1,0);
	//Function that get the key pressed by user and send it to Controller ECU by UART
	Get_Pass();
	LCD_clearScreen();
}
//call back function for timer
void Timer_CallBackFunction(void){
	//Uninitialize the timer driver
	Timer_dinit();
	//clear the flag to get out of polling
	flag=0;
}
