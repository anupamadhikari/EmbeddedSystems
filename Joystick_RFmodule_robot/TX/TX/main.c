/*
 * TX.c
 *
 * Created: 03-11-2016 11:37:16 PM
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

#define LowLimit 100
#define HighLimit 900

#define SYNC 0xAA
#define RADDR 0x44
#define FRONT 0x11
#define BACK 0x21
#define RIGHT 0x1B
#define LEFT 0x2B
#define  STOP 0x31

void USART_init()
{
	UBRRH=(uint8_t)(UBRR>>8);
	UBRRL=(uint8_t)UBRR;
	UCSRC|=1<<URSEL|1<<UCSZ1|1<<UCSZ0;
	UCSRB|=1<<TXEN;
}

void USART_TxByte(uint8_t data)
{
	while((UCSRA&(1<<UDRE))==0);
	UDR=data;
	while((UCSRA&(1<<TXC))==0);
	UDR=0;
}

void Send_Packet(uint8_t address,uint8_t data)
{
	USART_TxByte(SYNC);
	USART_TxByte(address);
	USART_TxByte(data);
	USART_TxByte(address+data);
}

void ADC_init(void)
{	
	ADCSRA |= 1<<ADPS2;
	ADMUX |= 1<<REFS0;
	ADCSRA |= 1<<ADIE;
	ADCSRA |= 1<<ADEN;
}

int main(void)
{
	DDRB=0xFF;
	USART_init();
	ADC_init();
	sei();
	ADCSRA|=1<<ADSC;
    /* Replace with your application code */
    while (1) 
    {/*
		if(x<200)
		Send_Packet(RADDR,FRONT);
		//_delay_ms(500);
		else if(x>800)
		Send_Packet(RADDR,BACK);
		//_delay_ms(500);
		 if(y>800)
		Send_Packet(RADDR,RIGHT);
		//_delay_ms(500);
		else if(y<200)
		Send_Packet(RADDR,LEFT);
		//_delay_ms(500);
		_delay_ms(200);
		*/
    }
}

ISR(ADC_vect)
{
	//uint8_t theLow = ADCL;
	uint16_t theTenBitResult = ADC;//ADCH<<8 | theLow;

	switch (ADMUX&(1<<MUX0))
	{
		case 0x00:
		if(theTenBitResult>HighLimit) 
		{
			PORTB|=1<<PINB0;
			Send_Packet(RADDR,BACK);
		}
		else if(theTenBitResult<LowLimit) 
		{
			PORTB&=~(1<<PINB0);
			Send_Packet(RADDR,FRONT);
		}
		else
		{
			PORTB&=~(1<<PINB0);
			Send_Packet(RADDR,STOP);
		}
		ADMUX |=1<<MUX0;
		break;
		case 0x01:
		if(theTenBitResult>HighLimit) 
		{
			PORTB|=1<<PINB1;
			Send_Packet(RADDR,LEFT);
		}
		else if(theTenBitResult<LowLimit)
		{
			PORTB&=~(1<<PINB1);
			Send_Packet(RADDR,RIGHT);
		}
		else
		{
			PORTB&=~(1<<PINB1);
			Send_Packet(RADDR,STOP);
		}
		ADMUX &=~(1<<MUX0);
		break;
		default:
		//Default code
		break;
	} ADCSRA |= 1<<ADSC;
}
