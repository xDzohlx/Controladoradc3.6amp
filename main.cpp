/*
 * Controladora sbus.cpp
 *
 * Created: 21/08/2020 20:09:52
 * Author : xDzohlx
 */ 
#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile static uint16_t accel = 0x0000;
volatile static uint16_t accel_val = 0x0000;
volatile static uint16_t accel_val_2 = 0x0000;
volatile static uint16_t accel_control = 0x0000;
volatile static uint16_t accel_control_2 = 0x0000;
volatile static uint16_t giro = 0x0000;
volatile static uint16_t giro_val = 0x0000;
volatile static uint16_t giro_val_2 = 0x0000;
volatile static uint16_t giro_control = 0x00;
volatile static uint16_t giro_control_2 = 0x00;
volatile static uint16_t canal[17];
volatile static uint16_t dato[24];//25 bytes de largo
volatile static uint16_t canal_3 = 0x0000;
volatile static uint8_t motor1pwm = 0x00;
volatile static uint8_t motor2pwm = 0x00;
volatile static uint8_t ledpwm[2];
uint16_t offsetgiro = 0x00;
uint16_t offsetaccel = 0x00;
int cont = 0;
int byte = 0;
bool button = true;
bool primero = false;
bool segundo = false;
bool tercero = false;
bool dch1 = false;
bool dch2 = false;
bool rx = false;
void setup(){

	//dos canales controlados por interrupcion
	DDRA = (1<<PORTA1);//
	DDRC = (1<<PORTC4)|(1<<PORTC0)|(1<<PORTC3)|(1<<PORTC1)|(1<<PORTC6);
	PUEB = (1<<PORTB6)|(1<<PORTB4);
	PORTB = (1<<PORTB6)|(1<<PORTB4);
	PORTC |= (1<<PORTC6);
	PCICR = (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);//interrucpciones canales
	TCCR0A |= (1<<COM0A0)|(1<<COM0A1)|(1<<WGM00)|(1<<WGM01)|(1<<COM0B1)|(1<<COM0B0);//timer0 8 bit
	TCCR0B |= (1<<CS00);
	TOCPMSA0 |= (1<<TOCC3S0);
	TOCPMSA1 |= (1<<TOCC4S0);
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
	TCCR1A = 0x00;//TIMER1 16 BIT TIEMPOS contador de rc
	TCCR1B = (1<<WGM12)|(1<<CS11);//|(1<<CS10);
	OCR1A = 0x0A;//validador de rc checar failsafe 898
	TIMSK1 = (1<<OCIE1A);//(1<<TOIE1)|
	TCNT1 = 0x0000;
	sei();//inicio de interrupciones
	_delay_ms(5000);
	giro_val = canal[0];
	_delay_ms(100);
	accel_val = canal[1];
}
void pwmled(){
	if (TCNT0==255&&primero){
		ledpwm[0]++;
		primero = false;
	}
	if(TCNT0!=255){
		primero=true;
	}
	if (ledpwm[0]==255&&segundo){
		ledpwm[1]++;
		segundo = false;
	}
	if(ledpwm[0]!=255){
		segundo = true;
	}
	if (ledpwm[1]==255){
		tercero = !tercero;
	}
	if (TCNT0==0){
		if (tercero){
			PORTC |= (1<<PORTC6);
			}else{
			PORTC &= ~(1<<PORTC6);
		}
	}
	if (ledpwm[1]>=TCNT0){
		if (tercero){
			PORTC &= ~(1<<PORTC6);
			}else{
			PORTC |= (1<<PORTC6);
		}
	}
}
//INTERRUPCIONES PARA LECTURA DE RC
ISR(PCINT0_vect){
	if (!(PINA & 0x01)){
		if (rx){
			rx= true;
		TCNT1 = 0x00;
		cont= 0;
		}
	}
	PCMSK0 &= ~(1<<PCINT0);// se desactiba esta instruccion momentanemente
}
ISR(TIMER1_COMPA_vect){//lectura de bit
	if(rx){
	cont++;
	if (cont>10){
		cont = 0;
		byte++;
		PCMSK0 |= (1<<PCINT0);
	}
	if (PINA & 0x01){//bit es igual a 1
		dato[byte] |= (1 << cont);
	}else{//bit es igual a 0
		dato[byte] &= ~(1 << cont);
	}
	if (byte==24){
		byte = 0;
		rx = false;
	}
	}
}
int main(void){
	setup();
	while(1){
		while(1){
			if (giro>= giro_val){
				giro_control = giro - giro_val;//offset
				if (giro_control>accel){
					accel_control = 0;
					}else{
					accel_control = accel - giro_control;//motor izquierdo
				}
				accel_control_2 = accel + giro_control;//motor derec
				}else{
				giro_control = giro_val - giro;//offset
				if (giro_control>accel){
					accel_control_2 = 0;
					}else{
					accel_control_2 = accel - giro_control;//motor izquierdo
				}
				accel_control = accel + giro_control;//motor izquierdo
			}
			if (accel_control>=accel_val){//MOTOR A
				//PORTA &= ~(1<<PORTA1);//arm
				//PORTC |= (1<<PORTC6);
				accel_val_2 = accel_control - accel_val;
				if (accel_val_2>510){
					accel_val_2 = 510;
				}
				accel_val_2 = accel_val_2 * (0.5);//
				TOCPMCOE |= (1<<TOCC0OE);//motor a
				TOCPMCOE &= ~(1<<TOCC1OE);
				TCCR0A |= (1<<COM0A0);
				OCR0A = accel_val_2;
				}else{
				accel_val_2=accel_val-accel_control;
				if (accel_val_2>510){
					accel_val_2 = 510;
				}
				accel_val_2 = (accel_val_2)*(0.5);
				TOCPMCOE |= (1<<TOCC1OE);//motor a
				TOCPMCOE &= ~(1<<TOCC0OE);
				OCR0A = accel_val_2;
			}
			
			if (accel_control_2>=accel_val){//MOTOR B
				giro_val_2 = accel_control_2 - accel_val;
				if (giro_val_2>510){
					giro_val_2 = 510;
				}
				giro_val_2 = giro_val_2 * (0.5);
				TOCPMCOE |= (1<<TOCC3OE);//motor a
				TOCPMCOE &= ~(1<<TOCC4OE);
				OCR0B = giro_val_2;
				}else{
				giro_val_2=accel_val-accel_control_2;
				if (giro_val_2>510){
					giro = 510;
				}
				giro_val_2 = (giro_val_2)*(0.5);
				TOCPMCOE |= (1<<TOCC4OE);//motor a
				TOCPMCOE &= ~(1<<TOCC3OE);
				OCR0B = giro_val_2;
			}
			pwmled();
		}//arm
		OCR0A = 0x00;//hard stop
		OCR0B = 0x00;
		TOCPMCOE &= ~(1<<TOCC0OE);//motor a
		TOCPMCOE &= ~(1<<TOCC1OE);
		TOCPMCOE &= ~(1<<TOCC3OE);//motor a
		TOCPMCOE &= ~(1<<TOCC4OE);
		PORTC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC3)|(1<<PORTC4);
	}//WHILE
}//INT MAIN
