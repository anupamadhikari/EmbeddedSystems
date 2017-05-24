/*
 * BuckBoost.c
 *
 * Created: 02-11-2016 01:41:04 PM
 * Author : Anupam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void set_duty(uint8_t duty)
{
	OCR0A=duty;
}

int main(void)
{
	DDRB|=1<<PINB0;
	TCCR0A|=1<<COM0A1|1<<WGM00|1<<WGM01;
	TCCR0B|=1<<CS00|1<<WGM02;
	set_duty(0);
	ADMUX|=1<<ADLAR|1<<MUX0;
	ADCSRA|=1<<ADEN|1<<ADIE|1<<ADPS2;
	sei();
	ADCSRA|=1<<ADSC;
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(ADC_vect)
{
	uint16_t voltage;
	uint8_t duty;
	voltage=ADCH<<2|ADCL>>6;
	duty=voltage;
	if (duty>255) duty=255;
	else if(duty<0) duty=0;
	set_duty(duty);
}
