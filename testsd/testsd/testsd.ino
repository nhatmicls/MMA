#include <avr/interrupt.h>
#define sensor A0
 
volatile int temp;
 
void setup()
{
    ​Serial.begin(9600)
    ​cli();                                  // tắt ngắt toàn cụ
    
    ​/* Reset Timer/Counter1 */
    ​TCCR1A = 0
    ​TCCR1B = 0
    ​TIMSK1 = 0
    
    ​/* Setup Timer/Counter1 */
    ​TCCR1B |= (1 << CS11) | (1 << CS10);    // prescale = 6
    ​TCNT1 = 40536
    ​TIMSK1 = (1 << TOIE1);                  // Overflow interrupt enable
    ​sei();                                  // cho phép ngắt toàn cụ
}
 
void loop()
{
    ​/* add main program code here */
}
 
 
ISR (TIMER1_OVF_vect) 
{
    ​TCNT1 = 40536
    ​temp = analogRead(sensor)
    ​Serial.print(F("Temp:"))
    ​Serial.println(temp)
}
