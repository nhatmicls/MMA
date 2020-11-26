#include "avr/interrupt.h"

int i=0;

void setup()
{                 
    pinMode(7,OUTPUT);
    
    ​TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 34286;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << TOIE1);
}
 
void loop()
{
    digitalWrite(7,HIGH);
}
 
 
ISR (TIMER1_OVF_vect) 
{
  if(i==0)
  {
    digitalWrite(7,HIGH);
    i=1;
  }
  else
  {
    digitalWrite(7,LOW);
    i=0;
  }
  TCNT1 = 34286;
}
