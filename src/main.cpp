#include <Arduino.h>
#include <avr/io.h> 

#define BAUD 9600
#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD-1 

#define MASTER_ADDR 15
#define SLAVE1_ADDR 13
#define SLAVE2_ADDR 14


#define LED_PIN   PORTB5	// pin 13
#define ADDR3_PIN PINB3		// pin 11
#define ADDR2_PIN PINB2		// pin 10
#define ADDR1_PIN PINB1		// pin 9
#define ADDR0_PIN PINB0		// pin 8
#define BUT1_PIN  PIND7		// pin 7
#define BUT2_PIN  PIND6		// pin 6
#define WREN_PIN  PORTD2	// pin 2

void asynch9_init() {
  // put your code here, to setup asynchronous serial communication using 9 bits:
    uint8_t ubrr = MYUBRR;
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr);

	// Assync, 9 bit, 1 stop bit, parity bit even
	if((PINB & 0x0F) == MASTER_ADDR){
		 UCSR0B |= _BV(TXEN0) | _BV(UCSZ02);
	}
	else {
		UCSR0A |= _BV(MPCM0);
		UCSR0B |= _BV(RXEN0) | _BV(UCSZ02);
	}
	  
	UCSR0C = _BV(UCSZ00);
}

uint8_t send_addr(uint8_t toSend) {
	while(!(UCSR0A & (1 << UDRE0)));
	UCSR0B |= (1 << TXB80);
	UDR0 = toSend;
	return 1;
}

uint8_t send_data(uint8_t toSend){
	while(!(UCSR0A & (1 << UDRE0))); 
	UCSR0B &= ~(1 << TXB80);
	UDR0 = toSend;
	return 1;
}

uint8_t get_data() {
 	 uint8_t data;
	 uint8_t bit8;
	 uint8_t status = UCSR0A;
	 bit8 = UCSR0B & (1 << RXB80);
	 data = UDR0;

	// FE0: Frame Error, DOR0: Data OverRun, UPE0: USART Parity Error   
	if (status & ((1<<FE0) | (1<<DOR0) | (1<<UPE0)))    
    	return 0; 

	if(bit8) {
		//if address is the same disable multiproc flag to allow data frames
		if((PINB & 0x0F) == (data & 0x0F))
			UCSR0A &= ~(1 << MPCM0);
		else 
			UCSR0A |= 1 << MPCM0;
	}
	//Turn on or off LED according to data frames
	else {
		if(data == 0x00)
			PORTB &= ~(1 << LED_PIN);
		else
			PORTB |= (1 << LED_PIN);
	}
  return 0;
}

void setup() {
	// Disable interrupts 
	noInterrupts();

	// Set address pins as inputs and led as output (PORT B - pins 8 to 13)
	DDRB = 0;
	DDRB |= _BV(LED_PIN);

	// Enable pull-up resistors for address inputs and turn off led
	PORTB |= 	(1 << ADDR0_PIN) |
				(1 << ADDR1_PIN) |
				(1 << ADDR2_PIN) |
				(1 << ADDR3_PIN);

	PORTB &=	~(1 << LED_PIN);

	// Set button pins as inputs and WREN as output (PORT D - pins 0 to 7)
	DDRD = 		0;
	
	DDRD |=		(1 << WREN_PIN);

	// Enable pull-up resistors for buttons and set WREN low
	PORTD |= 	(1 << BUT1_PIN) |
				(1 << BUT2_PIN);

	PORTD &= 	~(1 << WREN_PIN);

	// Initialise asynchronous communication
	asynch9_init();

	if((PINB & 0x0F) == MASTER_ADDR) 
		PORTD |= (1 << WREN_PIN);
	else 
		PORTD &= ~(1 << WREN_PIN);
		
		
	// Enable interrupts
	interrupts();
}

uint8_t but1_past = 0, but2_past = 0, last_addr = 0;

void loop() {
	// Check if it is master
	if((PINB & 0x0F) == MASTER_ADDR) {
		if((PIND & (1 << BUT1_PIN)) != but1_past) {
			but1_past = (PIND & (1 << BUT1_PIN));
			if(last_addr != SLAVE1_ADDR) {
				send_addr(SLAVE1_ADDR);
				last_addr = SLAVE1_ADDR;
			}
			send_data((but1_past > 0) ? 0 : 1);
		}
		if((PIND & (1 << BUT2_PIN)) != but2_past) {
			but2_past = (PIND & (1 << BUT2_PIN));
			if(last_addr != SLAVE2_ADDR) {
				send_addr(SLAVE2_ADDR);
				last_addr = SLAVE2_ADDR;
			}
			send_data((but2_past > 0) ? 0 : 1);
		}
	}
	else {
		if(UCSR0A & (1 << RXC0))
			get_data();
	}

}
