/*
 * BoostConverter.c
 *
 * Created: 28-10-2016 12:21:40 PM
 * Author : Anupam
 */ 

//Attiny13A
//Output Voltage Select at ADC2 PB4 -- Input
//PWM at OC0A PB0 -- Output
//Voltage Set Button at INT0 PB1
//F_CPU 20MHz

#define F_CPU 20000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MAX_V 15

uint8_t Duty;
uint16_t desired_value;

void set_duty()
{
	OCR0A=Duty;
}

void init_pwm(void)
{
	//Fast PWM mode Clear at OCR0A
	//PS=1
	//F=20MHz/256/PS
	TCCR0A|=1<<COM0A1||1<<WGM00||1<<WGM01;
	Duty=180;
	set_duty();
}

void init_adc(void)
{
	ADMUX|=1<<REFS0||1<<ADLAR||1<<MUX1;
	ADCSRA|=1<<ADEN||1<<ADIE||1<<ADPS2;
	
}
/*
void init_ext_interrupt(void)
{
	MCUCR|=1<<ISC01;
	GIMSK|=1<<INT0;
}
*/
/*
void pid_control(void)
{
	uint16_t i;
	static int error, Integral;
	i=no_of_samples;
	Integral=0;
		error=desired_value-current_value;
		Integral=Integral+error*Ki;
		Duty=Kp*error+Integral;
		if (Duty>255) Duty=255;
		else if (Duty<0) Duty=0;
		set_duty();
		if(--i==0)
		{
			Integral=0;
		}
}
*/

int main(void)
{
	init_pwm();
	init_adc();
	//init_ext_interrupt();
	sei();
	//ADCSRA|=1<<ADSC; 
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(ADC_vect)
{
	desired_value=ADCH<<2|ADCL>>6;
	/*switch (ADMUX&0x03)
	{
		case 0x01: current_value=result; ADMUX=(ADMUX&~(1<<MUX0))|1<<MUX1;break;
		case 0x02: desired_value=result;
		if (desired_value*50/1024>MAX_V)
		{
			desired_value=MAX_V*1024/50;
		}
		 ADMUX=(ADMUX&~(1<<MUX1))|1<<MUX0;break;
	}
	pid_control();*/
	if (desired_value*50/1024>MAX_V)
	{
		desired_value=MAX_V*1024/50;
	}
	Duty=desired_value*256/1024;
	set_duty();
}
/*
ISR(INT0_vect)
{
	
}
*/
