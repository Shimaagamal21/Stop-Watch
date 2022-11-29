
#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>

unsigned char sec1 =0;
unsigned char sec2 =0;
unsigned char min1 =0 ;
unsigned char min2=0;
unsigned char hr1=0;
unsigned char hr2=0;
unsigned char flag=0;

void Timer1_init(void){

	TCNT1 = 0;
	OCR1A = 1000;
	TIMSK |= (1<<OCIE1A);
	TCCR1A = (1<<FOC1A) |(1<<FOC1B) ; //non PWM
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);   // CTC , prescaler 1024


}

ISR(TIMER1_COMPA_vect){
	flag =1;
	}



void INT0_init(void){

	SREG &= (~(1<<7));       //disable interrupts by clearing I-bit
	DDRD &= (~(1<<PD2));     //configure pin 2 of port D as input
	PORTD |= (1<<PD2);       //enable pull up resistor
	GICR |= (1<<INT0);       //enable external interrupt for pin INT0
	MCUCR |= (1<<ISC01);     //the falling edge of INT0 generates interrupt req
	SREG |= (1<<7);          //enable interrupts by setting I-Bit
}

ISR (INT0_vect){             // to reset

	sec1=0;
	sec2=0;
    min1=0;
    min2=0;
	hr1=0;
	hr2=0;

}

void INT1_init(void){

	SREG &= (~(1<<7));
	DDRD &= (~(1<<PD3));                   //configure pin 3 of port D as input
	GICR |= (1<<INT1);                    //enable external interrupt for pin INT1
	MCUCR |= (1<<ISC11)| (1<<ISC10);      //the rising edge of INT1 generates interrupt req
	SREG |= (1<<7);
}



ISR (INT1_vect){              // to pause

	TCCR1B &= (~(1<<CS12)) & (~(1<<CS10)) ;
}

void INT2_init (void){

	SREG &= (~(1<<7));
	DDRB &= (~(1<<PB2));       //configure pin 2 of port B as input
	GICR |= (1<<INT2);          //enable external interrupt for INT2
	MCUCSR |= (1<<ISC2);    // the rising edge of INT2 generates interrupt req
	SREG |= (1<<7);

}

ISR (INT2_vect){             //to resume

	TCCR1B |= (1<<CS12)| (1<<CS10);

}

int main(void){

	INT0_init();
	INT1_init();
	INT2_init();

	DDRC |= 0x0F;              //first 4 pins of port C output
	PORTC &= (0xF0);           //initialize the 7-segment with value 0
	DDRA |= 0x3F;              //first 6 pins of port A output
	PORTA |= (0x3F);           //initialize the 6 pins by value 1
	SREG |= (1<<7);
	Timer1_init();

	while (1){
		if (flag==1){
			flag =0;
			sec1++;
			if (sec1==10){
				sec2++;
				sec1=0;
			}

			if ((sec1==0) && (sec2 ==6)){
				min1++;
				sec2=0;
			}
			if (min1==10){
				min2++;
				min1=0;
			}
			if ( (min1==0) && (min2==6)){
				hr1++;
				min2=0;
			}

			if (hr1==10){
				hr2++;
				hr1=0;
			}

		}

		PORTA = (1<<PA0);
		PORTC = (PORTC & 0xF0 ) | (sec1 & 0x0F);
		_delay_ms(2);



		PORTA = (1<<PA1);
		PORTC= (PORTC & 0xF0) | (sec2&0x0F);
		_delay_ms(2);



		PORTA = (1<<PA2);
		PORTC= (PORTC & 0xF0) | (min1&0x0F);
		_delay_ms(2);



		PORTA = (1<<PA3);
		PORTC= (PORTC & 0xF0) | (min2&0x0F);
		_delay_ms(2);


		PORTA = (1<<PA4);
		PORTC= (PORTC & 0xF0) | (hr1&0x0F);
		_delay_ms(2);

		PORTA =(1<<PA5);
		PORTC= (PORTC & 0xF0) | (hr2&0x0F);
		_delay_ms(2);

	}
}
