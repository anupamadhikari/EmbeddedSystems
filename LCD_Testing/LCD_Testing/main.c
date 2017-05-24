/*
 * LCD_Testing.c
 *
 * Created: 25-10-2016 09:46:59 PM
 * Author : Anupam
 */ 

//#define F_CPU 11059000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define LCDControlPORT PORTD
#define LCDControlDDR DDRD
#define RS PD2
#define RW PD7
#define EN PD5
#define LCDDisplayPORT PORTB
#define LCDDisplayDDR DDRB

void send_char_to_LCD(unsigned char c)
{
	LCD_check();
	LCDDisplayPORT=c;
	LCDControlPORT&=~(1<<RW);
	LCDControlPORT|=1<<RS;
	LCDControlPORT|=1<<EN;
	asm volatile("nop");
	asm volatile("nop");
	LCDControlPORT&=~(1<<EN);
	LCDDisplayPORT=0;
}

void send_comm_to_LCD(unsigned char c)
{
	LCD_check();
	LCDDisplayPORT=c;
	LCDControlPORT&=~(1<<RS||1<<RW);
	LCDControlPORT|=1<<EN;
	asm volatile("nop");
	asm volatile("nop");
	LCDControlPORT&=~(1<<EN);
	LCDDisplayPORT=0;
}

void init_LCD(void)
{
	LCDControlDDR|=1<<RS||1<<RW||1<<EN;
	_delay_ms(15);
	send_comm_to_LCD(0x01);
	_delay_ms(2);
	send_comm_to_LCD(0x38);
	_delay_ms(50);
	send_comm_to_LCD(0x0C);
	_delay_ms(50);
}

void LCD_check(void)
{
	LCDDisplayDDR=0x00;
	LCDControlPORT|=1<<RW;
	LCDControlPORT&=~(1<<RS);
	while(LCDDisplayPORT>=0x80)
	{
		LCDControlPORT|=1<<EN;
		asm volatile("nop");
		asm volatile("nop");
		LCDControlPORT&=~(1<<EN);
	}
	LCDDisplayDDR=0xFF;
}

void print_to_LCD(uint8_t number1, uint8_t number2)
{
	uint8_t i;
	char temp[5],stringU[16]="Temp = ",stringL[16]="DevTemp = ";
	itoa(number1,temp,10);
	strcat(stringU,temp);
	itoa(number2,temp,10);
	strcat(stringL,temp);
	send_comm_to_LCD(0x01);
	_delay_ms(2);
	send_comm_to_LCD(0x80);
	for(i=0;stringU[i]!='\0';i++)
	send_char_to_LCD(stringU[i]);
	send_comm_to_LCD(0xC0);
	for(i=0;stringL[i]!='\0';i++)
	send_char_to_LCD(stringL[i]);
}

int main(void)
{
	init_LCD();
	print_to_LCD(10,20);
	_delay_ms(500);
	print_to_LCD(15,60);
	_delay_ms(500);
	print_to_LCD(76,35);
	_delay_ms(500);
	print_to_LCD(67,187);
	_delay_ms(500);
    /* Replace with your application code */
    while (1) 
    {
    }
}

