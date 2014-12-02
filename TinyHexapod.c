#define F_CPU 12000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define SIPO_PORT  D
#define SPIN 3
#define CLK  5
#define STB  6
#include<sipolcd.h>

#define _delay_s(x) _delay_ms(x * 1000)

typedef unsigned char uchar;

void initSoftPWM()
{
	//Approx 9Khz frequency; CTC mode OCF2 Interrupt, Triggers ISR at about 9Khz rate
	TCNT2 = 0;
	OCR2 = 166;
	TCCR2 = (1<<WGM21) | (1<<CS21);
	TIMSK |= (1<<OCIE2);
}


#define motor1_pin 0
#define motor2_pin 1
#define motor3_pin 2

//A variable that is global and is accessed in a ISR must be declared as volatile otherwise
//there will be a lot of problem i have experienced it.
//It happens because of the compiler optimization routines. 
volatile uchar count = 0;

volatile uchar motor1_duty=0, motor2_duty=0, motor3_duty=0;



uchar servo_angle(int angle)
{//to understand this conversion of angle to duty cycle refer to Servo_Test program
		uchar val,duty;
		val = 13 + (angle*100)/375;
		duty = (val*100)/256; //calculate equivalent duty cycle from given angle
		return duty;
}

void motor1(uchar angle)
{
	static uchar last_value=190; //why i gave 190 as initialiser because since it is static
	//we have to initialise it but if i give zero and my first angle is 0 then it takes
	//no actiion
	if(angle == last_value)
		return;
	
	else
	{
		motor1_duty = servo_angle(angle);	
	}
}

void motor2(uchar angle)
{
	static uchar last_value=190;
	if(angle == last_value)
		return;
	
	else
	{
		motor2_duty = servo_angle(angle);	
	}
}

void motor3(uchar angle)
{
	static uchar last_value=190;
	if(angle == last_value)
		return;
	
	else
	{
		motor3_duty = servo_angle(angle);	
	}
}


void StepFront();
void StepBack();
void ResetLegs();

int main()
{

	lcd_init();
	lcd_print("Program Initialised");

	DDRB =0xff;
	
	initSoftPWM();

	
	sei();
	//init all motors
	ResetLegs();
	_delay_ms(1000);

	StepFront();	
	
	for(uint8_t i=0;i<10;i++)
		StepFront();
	for(uint8_t i=0;i<10;i++)
		StepBack();
	while(1)
	{

				

	}
}

void ResetLegs()
{
	motor1(30);
	motor2(30);
	motor3(30);
	_delay_ms(300);
}

//motor1 is middle
//motor2 is left
//motor3 is right
void StepFront()
{
#define delayms 300 //80 is minimum optimal value but 300 and 100 give good displacement per step
	motor1(18);
	_delay_ms(delayms);
	motor2(0);
	_delay_ms(delayms);
	motor3(0);
	_delay_ms(delayms);
	motor1(44);
	_delay_ms(delayms);
	motor2(40);
	_delay_ms(delayms);
	motor3(40);
	_delay_ms(delayms);	
}

void StepBack()
{
#define delayms 300
	motor1(44);
	_delay_ms(delayms);
	motor2(0);
	_delay_ms(delayms);
	motor3(0);
	_delay_ms(delayms);
	motor1(18);
	_delay_ms(delayms);
	motor2(40);
	_delay_ms(delayms);
	motor3(40);
	_delay_ms(delayms);	
}

ISR(TIMER2_COMP_vect)
{

	

	if(motor1_duty == count)
	{
		PORTB &= ~(1<<motor1_pin);
	}	

	if(motor2_duty == count)
	{
		PORTB &= ~(1<<motor2_pin);
	}

	if(motor3_duty == count)
	{
		PORTB &= ~(1<<motor3_pin);
	}
	
	count++;

	if(count == 100)
	{
		count = 0;
		PORTB |= (1<<motor1_pin);
		PORTB |= (1<<motor2_pin);
		PORTB |= (1<<motor3_pin);
	}

	
}
