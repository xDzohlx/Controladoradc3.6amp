/*
 *main.cpp
 *
 * Created: 23/05/2017 23:52:50
 * Author : dzohl
 */ 

//detalles de version
//todavia no esta bien saturados los canales
//
//version acceptable y estable faltan limites inferiores se arregla desde el control con endpoints


//inputs canal 1 pa2, canal 2 pa3
//motores 


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
volatile static uint16_t canal_3 = 0x0000;
volatile static uint8_t motor1pwm = 0x00;
volatile static uint8_t motor2pwm = 0x00;
volatile static uint8_t ledpwm[2];
//volatile static uint16_t miliseg = 0x00;
uint16_t offsetgiro = 0x00;
uint16_t offsetaccel = 0x00;

bool primero = false;
bool segundo = false;
bool tercero = false;
bool button = true; 
bool canal = true; 
bool canal_1 = false;
bool canal_2 = false;
bool select_canal_1 = false;
bool select_canal_2 = false;
bool seleccion = false;
bool forward = false;
bool adelante = false;
bool forward_canal_2 = false;
bool right = false;
bool superbutton = false;
bool canal_1_valido = false;
bool canal_2_valido = false;
bool canal_3_valido = false;
bool failsafe = true;


void setup(){	
	//dos canales controlados por interrupcion
	DDRC = (1<<DDC4)|(1<<PORTC0)|(1<<PORTC3)|(1<<PORTC1)|(1<<PORTC6);
	PUEB = (1<<PORTB6)|(1<<PORTB4);
	PORTB = (1<<PORTB6)|(1<<PORTB4);
	PORTC |= (1<<PORTC6);
	//DDRA = (1<<PORTA0)|(1<<PORTA1);
	//PORTA |= (1<<PORTA1);//robot encendido
	//selecciondecanales();
	//interrupciones pines externos canal1
	PCICR = (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0)|(1<<PCINT1);//|(1<<PCINT4);//interrucpciones canales
	//timer0 8 bit
	TCCR0A |= (1<<COM0A0)|(1<<COM0A1)|(1<<WGM00)|(1<<WGM01)|(1<<COM0B1)|(1<<COM0B0);
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
	//TIMER1 16 BIT TIEMPOS contador de rc
	TCCR1A = 0x00;
	TCCR1B = (1<<WGM12)|(1<<CS11);//|(1<<CS10);
	
	OCR1A = 0xFF00;//validador de rc checar failsafe
	
	//TIMSK1 = (1<<TOIE1)|(1<<OCIE1A);
	
	TCNT1 = 0x0000;
	//selecciondecanales();
	sei();
	//parchada 
	_delay_ms(2000);
	_delay_ms(2000);
	_delay_ms(2000);
	_delay_ms(2000);
	_delay_ms(2000);
	giro_val = giro;
	filtro_giro = giro_val;
	_delay_ms(2000);
	accel_val = accel;
	filtro_accel = accel_val;
}
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
//INTERRUPCIONES PARA LECTURA DE RC
//inputs canal 1 pa2, canal 2 pa3
ISR(PCINT0_vect){
//funciona de forma sincrona ESTA PARCHADO
if (!(PINA & 0x02)){
	if (canal_1_valido){
		if(tiempo1< TCNT1)//ESTO ES UN PARCHE
		giro = TCNT1 - tiempo1;//lectura 1
		canal_1_valido = false;
		failsafe  = false;
	}
} else{
	if (canal){
		TCNT1 = 0x0000;
		canal = false;
	}
	if (!canal_1_valido){
		tiempo1 = TCNT1;//lectura 2
		canal_1_valido = true;
	}
}
//lectura 2
if (!(PINA & 0x01)){
	if (canal_2_valido){
		if(tiempo2< TCNT1)//ESTO ES UN PARCHE
		accel = TCNT1 - tiempo2;//lectura 1
		canal_2_valido = false;
	}
	} else{
	if (canal){
		TCNT1 = 0x0000;
		canal = false;
	}
	if (!canal_2_valido){
		tiempo2 = TCNT1;//lectura 2
		canal_2_valido = true;
	}
	//reset contador
	if (!(PINA & 0x02)&&!(PINA & 0x01)){
		canal = true;
		TCNT1 = 0x0000;
	}
}
//arm
//if (!(PINA & 0x10)){
	//if (canal_3_valido){
		//canal_3 = TCNT1 - tiempo3;
		//canal_3_valido = false;
	//}
	//} else{
	//if (!canal_3_valido){
		//tiempo3 = TCNT1;//lectura 2
		//canal_3_valido = true;
	//}
//}
}
//contador useg
ISR(TIMER1_COMPA_vect){
	failsafe  = true;
}
int main(void){
	setup();
	//PORTA &= ~(1<<PORTA1);
	while(1){
		//falta failsafe
	while(1){
		
			giro_control = giro;
			accel_control = accel;
			
		if (giro_control >= giro_val){// sentido del volante
			giro_control = giro_control-giro_val;//offset
			accel_control_2 = accel_control - giro_control;//motor 1
			accel_control = accel_control + giro_control;//motor 1
			}else{
			giro_control = giro_val - giro_control;//offset
			accel_control_2 = accel_control + giro_control;//motor 2
			accel_control = accel_control - giro_control;//motor 2
		}


//------------------------Motores
		if (accel_control>=accel_val){//MOTOR A

			accel_val_2 = accel_control - accel_val;
			if (accel_val_2>510)
			{
				accel_val_2 = 510;
			}
			accel_val_2 = accel_val_2>>1;//
			TOCPMCOE |= (1<<TOCC0OE);//motor a
			TOCPMCOE &= ~(1<<TOCC1OE);
			TCCR0A |= (1<<COM0A0);

			OCR0A = accel_val_2;
			}else{
				accel_val_2=accel_val-accel_control;
				if (accel_val_2>510)
				{
					accel_val_2 = 510;
				}
			accel_val_2 = accel_val_2>>1;

			TOCPMCOE |= (1<<TOCC1OE);//motor a
			TOCPMCOE &= ~(1<<TOCC0OE);
			//TCCR0A &= ~(1<<COM0A0);			
			OCR0A = accel_val_2;
		}
		
		if (accel_control_2>=accel_val){//MOTOR B
			//PORTA &= ~(1<<PORTA1);//arm
			
			giro_val_2 = accel_control_2 - accel_val;
			if (giro_val_2>510)
			{
				giro_val_2 = 510;
			}
			giro_val_2 = giro_val_2>>1;
			TOCPMCOE |= (1<<TOCC3OE);//motor
			TOCPMCOE &= ~(1<<TOCC4OE);

			OCR0B = giro_val_2;
			}else{
			giro_val_2=accel_val-accel_control_2;
			if (giro_val_2>510)
			{
				giro = 510;
			}
			giro_val_2 = giro_val_2>>1;

			TOCPMCOE |= (1<<TOCC4OE);//motor
			TOCPMCOE &= ~(1<<TOCC3OE);
			//TCCR0A &= ~(1<<COM0A0);
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
	//PORTA &= ~(1<<PORTA0);
	}//WHILE
}//INT MAIN
