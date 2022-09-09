/*********************************************************
* lab3.c
* Lab 3 ECE 231
* Writing a program to make LEDs show temperature on the breadboard 
* which is connected to the ATmega328p on Arduino Uno

* Date         Author          Revision
* 02/04/2022   Ritvik Verma    Lab 3 code write 

*References
*Proffesor McLaughlin's Lecture 11,12,13,14 Spring 2022
**********************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#define PERSISTENCE 5 
#define COUNTTIME 200

void uart_init(void); 
void uart_send(char letter);
void send_string(char *stringAddress);
void adc_init(void);
unsigned int get_adc(void);
void screen_init(void);

int main(void){
    
    double digitalValue, celcius, farenheit; //instantiating farenheit and celcius as int variables
    unsigned int t =0;
    
    adc_init();
    PORTC = 1<<PORTC5;  //PORT C5 has input for button
    screen_init();

    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};   //all possible LED digits (0-9)
    int j=0;
    unsigned char DIG1, DIG2, DIG3, DIG4;  //instantiating digits to find their value later
    unsigned  char var[] = {0b01110001, 0b00111001, 0b10000000}; //index 0 = unit F, index 1 = unit C, index 2 = decimal point


    while(1){
        digitalValue = get_adc(); // mV received from LED
        //celcius = ((digitalValue/10-32.0)*(5.0/9.0))*10;    //convert formula for celcius
        celcius = (digitalValue-750)/10 + 25;   //converting mV to Celcius
        farenheit = celcius*(9.0/5.0)+32.0;   //converting celcius to farenheit formula
        
        if ((PINC & (1<<PINC5))==0){    //when PIN5 is pressed 

            t = celcius*10;

            DIG4 = var[1];  //when button pressed the units are C
            DIG3 = (t %10);  //1st decimal place of temp
            DIG2 = ((t /10)%10); //one's place
            DIG1 = ((t /100)%10);    //ten's place
        }
        else{
            t = farenheit*10;
            DIG4 = var[0];  //if button is not pressed unit is F
            DIG3 = (t %10); //decimal place 
            DIG2 = ((t /10)%10);    //one's place   
            DIG1 = ((t /100)%10);   //ten's place
        }

         for (j=0; j<COUNTTIME/PERSISTENCE/4; j++){  //same loop to output the temperature on LED
                
            PORTD = DIG4;
            PORTB = ~ (1<<1);
            _delay_ms(PERSISTENCE);

            PORTD = ledDigits[DIG3];
            PORTB = ~ (1<<2);
            _delay_ms(PERSISTENCE);
                
            PORTD = var[2];
            PORTB = ~ (1<<3);
            _delay_ms(PERSISTENCE);
                
            PORTD = ledDigits[DIG2];
            PORTB = ~ (1<<3);
            _delay_ms(PERSISTENCE);

            PORTD = ledDigits[DIG1];
            PORTB = ~ (1<<4);
            _delay_ms(PERSISTENCE);
        }
    }
    
    return 0;
}

//code used from Lecture 14 slide 21

void screen_init(void){
    DDRD = 0xFF;
    DDRB = 0xFF;
    PORTB = 0xFF;
}

//initialize ATmega328 UART 
void uart_init(void){
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
    UBRR0L = 103;
}
// send 
void uart_send(char letter){
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = letter;
}
//read ADC value
unsigned int get_adc(){
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADIF))==0);
    return ADCL|(ADCH<<8);
}
//Enabling ADC and setting speed
void adc_init(void){
    DDRC = 0x00;
    ADMUX = 0xC0;
    ADCSRA = 0x87;
}
//send string of ASCII characters
void send_string(char *stringAddress){
    unsigned char i;
    for (i=0; i<strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}