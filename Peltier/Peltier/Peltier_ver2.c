/*
 * Peltier.c
 *
 * Created: 22-10-2016 11:03:18 PM
 * Author : Anupam
 */ 
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <string.h>

/*
Power Save Mode
Connection Information
Peltier MOSFET at OC0/PB3
Fan MOSFET at OC1B/PD4
Fan Speed control enable at INT2
Temperature Sensor at ADC0/A0
Fan Speed Controller at ADC1/A1
Temperature up button at INT0
Temperature down button at INT1
LCD Control
	RS	PA5
	RW	PA6
	E	PA7
LCD Data at PORTC
*/

//MACRO definitions
#define min_temp 12
#define max_temp 30 
#define init_temp 25

#define Kp 0.5
#define Ki 0.3
#define Kd 0.1
#define Temp_correction 1.953125

#define LCDControlPORT PORTA
#define LCDControlDDR DDRA
#define RS PA5
#define RW PA6
#define EN PA7
#define LCDDisplayPORT PORTC
#define LCDDisplayDDR DDRC

uint8_t Temp;

void set_Peltier_temp(uint8_t value)
{
	OCR0=value;
}

void set_Fan_speed(uint8_t value)
{
	OCR1B=value;
}

struct PID_data
{
	float integral,error,last_error;
	uint8_t current_value;
	}Peltier;

void PID_controller(struct PID_data *a)
{
	uint8_t result;
	a->error=Temp- a->current_value;
	a->integral+=a->error;
	if(((Kp*a->error+Ki*a->integral+Kd*(a->error-a->last_error))*256/(max_temp-min_temp))>255) result=255;
	else if(((Kp*a->error+Ki*a->integral+Kd*(a->error-a->last_error))*256/(max_temp-min_temp))<0) result=0;
	else result=(Kp*a->error+Ki*a->integral+Kd*(a->error-a->last_error))*256/(max_temp-min_temp);
	a->last_error=a->error;
	set_Peltier_temp(result);
}

void init_PWM()
{
	TCCR0|=1<<WGM01|1<<WGM00;	//Set OCR0 to Fast PWM
	TCCR1A|=1<<WGM10;
	TCCR1B|=1<<WGM12;			//Set OCR1B to Fast PWM
	TCCR0|=1<<COM01|1<COM00;	//Set OCR0 to inverting mode
	TCCR1A|=1<<COM1B1;			//Set OCR1B to non inverting mode
}

void init_ADC()
{
	ADCSRA|=1<<ADIE|1<<ADEN|1<<ADPS0|1<<ADPS1|1<<ADPS2|1<<REFS0; //Enable ADC; Set ADC Interrupt; Set Prescaler to 128; Aref to AVcc
}

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
	char temp[5],stringU[16]="Temp: ",stringL[16]="DevTemp: ";
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

void init_system()
{
	
	MCUCR|=1<<ISC11|1<<ISC01;	//Configure INTO and INT1 as edge triggered
	GICR|=1<<INT0|1<<INT1|1<<INT2;	//Enable External Interrupts
	init_ADC();	//Initialise ADC
	init_PWM();	//Initialise PWM
	init_LCD();	//Initialise LCD
	Temp=init_temp;
	set_Fan_speed(127);
	sei();	//Set Global Interrupts
	ADCSRA|=1<<ADSC;	//Start ADC Conversion
}

int main()
{
	init_system();
	while(1)
	{
		if(!Peltier.error) MCUCR|=1<<SM0;	//Enter ADC Noise Reduction Mode
		else MCUCR&=~(1<<SM0);				//Enter Idle Mode
		ADCSRA=1<<ADSC;
	}
}

ISR(INT0_vect)
{
	Temp++;//Increase Temperature
	if(Temp>max_temp) Temp=max_temp;
}

ISR(INT1_vect)
{
	Temp--;//Decrease Temperature
	if(Temp<min_temp) Temp=min_temp;
}

ISR(INT2_vect)
{
	ADMUX|=1<<MUX0;//Switch Channel to ADC1
}

ISR(ADC_vect)
{
	uint16_t Value;
	uint8_t Value_8;
	Value=ADCL|ADCH<<8;
	Value_8=Value*256/1024;
	switch(ADMUX&&1<<MUX0)
	{
		case 0<<MUX0:	Peltier.current_value=Value_8*Temp_correction;
						PID_controller(&Peltier);
						break;
		case 1<<MUX0:	set_Fan_speed(Value_8);
						ADMUX&=~(1<<MUX0);
						break;
	}print_to_LCD(Temp,Peltier.current_value);
	//ADCSRA|=1<<ADSC;
}