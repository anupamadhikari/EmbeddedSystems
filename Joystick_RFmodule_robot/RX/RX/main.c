/*
 * RX.c
 *
 * Created: 03-11-2016 11:37:51 PM
 * Author : Anupam
 */ 
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 1200
#define UBRR (F_CPU/(BAUD*16UL)-1)

#define SYNC 0xAA
#define RADDR 0x44
#define FRONT 0x11
#define BACK 0x21
#define RIGHT 0x1B
#define LEFT 0x2B
#define STOP 0x31

void USART_init(void)
{
	UBRRH=(uint8_t)(UBRR>>8);
	UBRRL=(uint8_t)UBRR;
	UCSRC|=1<<UCSZ1|1<<UCSZ0|1<<URSEL;
	UCSRB|=1<<RXCIE|1<<RXEN;
}

uint8_t USART_RxByte(void)
{
	while((UCSRA&(1<<RXC))==0);
	return UDR;
}

ISR(USART_RXC_vect)
{
	uint8_t address,data,check;
	address=USART_RxByte();
	data=USART_RxByte();
	check=USART_RxByte();
	if((check==address+data)&&(address=RADDR))
	{
		switch(data)
		{
			case FRONT: PORTB=0b00001001;break;
			case BACK: PORTB=0b00000110;break;
			case RIGHT: PORTB=0b00001010;break;
			case LEFT: PORTB=0b00000101;break;
			case STOP: PORTB=0x00;break;
		}
	}
	
}

int main(void)
{
	DDRB=0x0F;
	sei();
	USART_init();
    /* Replace with your application code */
    while (1) 
    {
    }
}

