/*
 * PWM_test.c
 *
 * Created: 27-02-2017 03:34:31 PM
 * Author : Anupam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define Kp 0.1
#define Kd 0.05
#define Ki 0.03
#define Speed 512

struct PID_data
{
	uint16_t current_speed, error, last_error, integral;
	}Motor;
	
void set_speed(uint16_t value)
{
	OCR0=255;
}

void pid_control(void)
{
	uint16_t result;
	Motor.error=Speed-Motor.current_speed;
	Motor.integral+=Motor.error;
	result=Kp*Motor.error+Ki*Motor.integral+Kd*(Motor.error-Motor.last_error);
	if(result>1024) result=1024;
	set_speed(result);
	Motor.last_error=Motor.error;
}

int main(void)
{
	TCCR0|=1<<WGM00|1<<WGM01|1<<COM01;
	ADMUX|=1<<REFS0;
	ADCSRA|=1<<ADEN|1<<ADIE;
	Motor.last_error=0;Motor.integral=0;
	set_speed(512);
	sei();
	ADCSRA|=1<<ADSC;
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(ADC_vect)
{
	uint16_t result;
	result=ADCH<<8|ADCL;
	Motor.current_speed=result;
	pid_control();
	ADCSRA|=1<<ADSC;
}
