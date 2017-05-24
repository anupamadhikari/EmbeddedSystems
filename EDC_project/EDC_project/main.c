/*
 * EDC_project.c
 * Program to perform four quadrant operation of DC Motor with PID F/B
 * Created: 18-03-2017 07:49:08 PM
 * Author : Anupam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

/*
Speed control at ADC0
Coasting operation interrupt at INT1
Reversing interrupt/pin at INT2/PINB2
Forward Motoring PWM at OC0
Reverse Motoring PWM at OC2
*/
int mode=1;

void set_duty(uint8_t duty)
{
		if(mode=1) {OCR0=duty;OCR2=0;}
		else { OCR2=duty;OCR0=0;PORTB|=1<<PINB0;}
	//pre_duty=duty;
	//OCR0=duty;
	
}

void init_ADC()
{
	ADMUX|=1<<REFS0|1<<ADLAR;
	ADCSRA|=1<<ADEN|1<<ADPS2|1<<ADATE;
}

void init_PWM()
{
	TCCR0|=1<<WGM00|1<<WGM01|1<<COM01|1<<CS02;
	OCR0=0;
	TCCR2|=1<<WGM21|1<<WGM20|1<<COM21|1<<CS22;
	OCR2=0;
}

void init_SYS()
{
	DDRD|=1<<PIND7;
	DDRB|=1<<PINB3;
	MCUCR|=1<<ISC11;
	GICR|=1<<INT0|1<<INT1;
	ADCSRA|=1<<ADSC;
	sei();
	
	
}

int main()
{
	DDRB|=1<<PINB0;
	DDRD|=1<<PIND3;
	PORTD|=1<<PIND3;
	init_ADC();
	init_PWM();
	init_SYS();
	
	while(1) {set_duty(ADCH<<2|ADCL>>6);ADCSRA|=1<<ADSC;}
	return 0;
}
ISR(INT0_vect)
{
	OCR0=0;
	OCR2=0;
}

ISR(INT1_vect)
{
	
	if(mode==0) mode=1;
	else {mode=0; }
}

ISR(ADC_vect)
{
	//static int i=0;
	//uint8_t Result;
	//Result=((ADCL)|(ADCH<<8))*256/1024;
	//set_duty(Result);
	set_duty(ADCH<<2|ADCL>>6);
	//if((ADCH<<2|ADCL>>6)>512) PORTB|=1<<PINB0;
	ADCSRA|=1<<ADSC;
	//if(i++>100) PORTB|=1<<PINB0;
	//PORTB&=~(1<<PINB0);
}