#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <stdlib.h>

#define MrLCDsCrib PORTB
#define DataDir_MrLCDsCrib DDRB
#define MrLCDsControl PORTD
#define DataDir_MrLCDsControl DDRD
#define LightSwitch 5
#define ReadWrite 7
#define BiPolarMood 2

char firstColumnPositionsForMrLCD[4] = {0, 64, 20, 84};

void Check_IF_MrLCD_isBusy(void);
void Peek_A_Boo(void);
void Send_A_Command(unsigned char command);
void Send_A_Character(unsigned char character);
void Send_A_String(char *StringOfCharacters);
void GotoMrLCDsLocation(uint8_t x, uint8_t y);
void InitializeMrLCD(void);

void Check_IF_MrLCD_isBusy()
{
	DataDir_MrLCDsCrib = 0;
	MrLCDsControl |= 1<<ReadWrite;
	MrLCDsControl &= ~1<<BiPolarMood;

	while (MrLCDsCrib >= 0x80)
	{
		Peek_A_Boo();
	}

	DataDir_MrLCDsCrib = 0xFF; //0xFF means 0b11111111
}

void Peek_A_Boo()
{
	MrLCDsControl |= 1<<LightSwitch;
	asm volatile ("nop");
	asm volatile ("nop");
	MrLCDsControl &= ~1<<LightSwitch;
}

void Send_A_Command(unsigned char command)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = command;
	MrLCDsControl &= ~ ((1<<ReadWrite)|(1<<BiPolarMood));
	Peek_A_Boo();
	MrLCDsCrib = 0;
}

void Send_A_Character(unsigned char character)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = character;
	MrLCDsControl &= ~ (1<<ReadWrite);
	MrLCDsControl |= 1<<BiPolarMood;
	Peek_A_Boo();
	MrLCDsCrib = 0;
}

void Send_A_String(char *StringOfCharacters)
{
	while(*StringOfCharacters > 0)
	{
		Send_A_Character(*StringOfCharacters++);
	}
}

void GotoMrLCDsLocation(uint8_t x, uint8_t y)
{
	Send_A_Command(0x80 + firstColumnPositionsForMrLCD[y-1] + (x-1));
}

void InitializeMrLCD()
{
	DataDir_MrLCDsControl |= 1<<LightSwitch | 1<<ReadWrite | 1<<BiPolarMood;
	_delay_ms(15);

	Send_A_Command(0x01); //Clear Screen 0x01 = 00000001
	_delay_ms(2);
	Send_A_Command(0x38);
	_delay_us(50);
	Send_A_Command(0b00001110);
	_delay_us(50);
}

void Send_A_StringToMrLCDWithLocation(uint8_t x, uint8_t y, char *StringOfCharacters)
{
	GotoMrLCDsLocation(x, y);
	Send_A_String(StringOfCharacters);
}

void Send_An_IntegerToMrLCD(uint8_t x, uint8_t y, int IntegerToDisplay, char NumberOfDigits)
{
	char StringToDisplay[NumberOfDigits];
	itoa(IntegerToDisplay, StringToDisplay, 10);
	Send_A_StringToMrLCDWithLocation(x, y, StringToDisplay); Send_A_String(" ");
}


int main(void)
{
	InitializeMrLCD();
	Send_A_StringToMrLCDWithLocation(1,1,"X:");
	Send_A_StringToMrLCDWithLocation(1,2,"Y:");

	ADCSRA |= 1<<ADPS2;
	ADMUX |= 1<<REFS0;
	ADCSRA |= 1<<ADIE;
	ADCSRA |= 1<<ADEN;

	sei();

	ADCSRA |= 1<<ADSC;

	while (1)
	{
	}
}
ISR(ADC_vect)
{
	uint8_t theLow = ADCL;
	uint16_t theTenBitResult = ADCH<<8 | theLow;

	switch (ADMUX&(1<<MUX0))
	{
		case 0x00:
		if(theTenBitResult>400) PORTB|=1<<PINB0;
		else PORTB&=~(1<<PINB0);
		//Send_An_IntegerToMrLCD(4, 1, theTenBitResult, 4);
		ADMUX |=1<<MUX0;
		break;
		case 0x01:
		if(theTenBitResult>400) PORTB|=1<<PINB1;
		else PORTB&=~(1<<PINB1);
		//Send_An_IntegerToMrLCD(4, 2, theTenBitResult, 4);
		ADMUX &=~(1<<MUX0);
		break;
		default:
		//Default code
		break;
	} ADCSRA |= 1<<ADSC;
}