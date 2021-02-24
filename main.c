/*
 * esc3.6amp.c
 *
 * Created: 02/01/2021 20:30:50
 * Author : Usuario
 */ 
//input canal 1 pa0,
//ouput decoder canal 2 pa1 
//motores 

//funciona la lectura de canales pero no la traccion parece que solo lee el canal 2
#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>


volatile static uint16_t contador = 0x0000;
volatile static uint16_t contador2 = 0x0000;
volatile static uint16_t filtro_accel = 0x0000;
volatile static uint16_t accel = 0x0000;
volatile static uint16_t accel_val = 0x0000;
volatile static uint16_t accel_val_2 = 0x0000;
volatile static uint16_t accel_control = 0x0000;
volatile static uint16_t accel_control_2 = 0x0000;
volatile static uint16_t filtro_giro = 0x0000;
volatile static uint16_t giro = 0x0000;
volatile static uint16_t giro_val = 0x0000;
volatile static uint16_t giro_val_2 = 0x0000;
volatile static uint16_t giro_control = 0x00;
volatile static uint16_t giro_control_2 = 0x00;
volatile static uint16_t tiempo1 = 0x0000;
volatile static uint16_t tiempo2 = 0x0000;
volatile static uint16_t tiempo3 = 0x0000;
volatile static uint16_t canal[6];
//volatile static uint16_t canal_offset[5];
volatile static uint16_t canal_3 = 0x0000;
volatile static uint8_t motor1pwm = 0x00;
volatile static uint8_t motor2pwm = 0x00;
volatile static uint8_t ledpwm[2];
uint16_t offsetgiro = 0x00;
uint16_t offsetaccel = 0x00;
int cont = 0;

bool button = true; 
bool primero = false;
bool segundo = false;
bool tercero = false;
bool seleccion = false;
bool forward = false;
bool adelante = false;
bool forward_canal_2 = false;
bool right = false;
bool superbutton = false;
//bool failsafe = true;


void setup(){	

	//dos canales controlados por interrupcion
	DDRA = (1<<PORTA1);//
	DDRC = (1<<PORTC4)|(1<<PORTC0)|(1<<PORTC3)|(1<<PORTC1)|(1<<PORTC6);
	PUEB = (1<<PORTB6)|(1<<PORTB4);
	PORTB = (1<<PORTB6)|(1<<PORTB4);

	PCICR = (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);//|(1<<PCINT1);//|(1<<PCINT4);//interrucpciones canales
	//timer0 8 bit
	TCCR0A |= (1<<COM0A0)|(1<<COM0A1)|(1<<WGM00)|(1<<WGM01)|(1<<COM0B1)|(1<<COM0B0);
	TCCR0B |= (1<<CS01);

	//TOCPMSA0 |= (1<<TOCC3S0);
	//
	//TOCPMSA1 |= (1<<TOCC4S0);
	//
	TOCPMSA0 &= ~(1<<TOCC0S0);
	TOCPMSA0 &= ~(1<<TOCC1S0); 
	
	TOCPMCOE |= (1<<TOCC1OE);//motor a
	TOCPMCOE &= ~(1<<TOCC0OE);
	
	PORTC |=(1<<PORTC0);
	PORTC |=(1<<PORTC1);

	PORTC |=(1<<PORTC3);
	PORTC |=(1<<PORTC4);
		
	TOCPMCOE |= (1<<TOCC3OE);//motor b
	TOCPMCOE &= ~(1<<TOCC4OE);
	
	OCR0A = 0x00;
	OCR0B = 0x00;
	//TIMER1 16 BIT TIEMPOS contador de rc
	TCCR1A = 0x00;
	TCCR1B = (1<<WGM12)|(1<<CS11);//|(1<<CS10);
	
	OCR1A = 0xFA0;//validador de rc checar failsafe 898
	
	TIMSK1 = (1<<OCIE1A);//(1<<TOIE1)|
	
	TCNT1 = 0x0000;
	//selecciondecanales();
	sei();


	_delay_ms(5000);
	accel_val = accel;

}
//INTERRUPCIONES PARA LECTURA DE RC
void pwmled(){
				if (TCNT0>=0&&primero){
					//PORTC |= (1<<PORTC6);
					primero = false;
					ledpwm[0]++;
					if (segundo){
						PORTC |= (1<<PORTC6);
						}else{
						PORTC &= ~(1<<PORTC6);
					}
				}
				if (TCNT0>=ledpwm[0]&&!primero){
					//PORTC &= ~(1<<PORTC6);
					primero = true;
					if (segundo){
						PORTC &= ~(1<<PORTC6);
						}else{
						PORTC |= (1<<PORTC6);
					}
				}
				if (ledpwm[0]==255&&tercero){
					segundo = !segundo;
					tercero=false;
				}
				if (ledpwm[0]==0){
					tercero = true;
				}
}
ISR(PCINT0_vect){
if (!(PINA & 0x01)){
		accel =TCNT1;
}else{	TCNT1 = 0x00;}
}
//contador useg
ISR(TIMER1_COMPA_vect){
	cont = 0;
}
int main(void){
	setup();
	while(1){
	while(1){//while(canal_3>1300){//probar diferencial

		if (accel>=accel_val){//MOTOR A
			//PORTA &= ~(1<<PORTA1);//arm
			accel_val_2 = accel - accel_val;
			if (accel_val_2>510)
			{
				accel_val_2 = 510;
			}
			accel_val_2 = accel_val_2 * (0.5);//
						TOCPMCOE &= ~(1<<TOCC1OE);
						TOCPMCOE &= ~(1<<TOCC4OE);
			TOCPMCOE |= (1<<TOCC0OE)|(1<<TOCC3OE);
			TCCR0A |= (1<<COM0A0);
			//PORTC |= (1<<PORTC1);//arm
			OCR0A = accel_val_2;
			}else{
				accel_val_2=accel_val-accel;
				if (accel_val_2>510)
				{
					accel_val_2 = 510;
				}
			accel_val_2 = (accel_val_2)*(0.5);
			//PORTA |= (1<<PORTA1);//arm
						TOCPMCOE &= ~(1<<TOCC3OE);
						TOCPMCOE &= ~(1<<TOCC0OE);
			TOCPMCOE |= (1<<TOCC1OE)|(1<<TOCC4OE);
			
			TOCPMCOE &= ~(1<<TOCC0OE);
			//TCCR0A &= ~(1<<COM0A0);			
			OCR0A = accel_val_2;
		}	
		}//arm
	OCR0A = 0x00;//hard stop
	OCR0B = 0x00;
	TOCPMCOE &= ~(1<<TOCC0OE);//motor a
	TOCPMCOE &= ~(1<<TOCC1OE);
	TOCPMCOE &= ~(1<<TOCC3OE);//motor a
	TOCPMCOE &= ~(1<<TOCC4OE);
	PORTC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC3)|(1<<PORTC4);
	//PORTA &= ~(1<<PORTA0);
	}//WHILE
}//INT MAIN