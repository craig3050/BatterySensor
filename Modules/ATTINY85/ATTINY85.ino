#include <avr/sleep.h>

const byte interruptPin = 4;
volatile byte state = LOW;
int hold_off_pin = 3;
int light_input_pin = 2;
int light_output_pin = 1;
int pir_input_pin = 4;
int ext_reset_pin = 0;
int illuminance = 0;
int illuminance_threshold = 100;


void setup() 
{
  pinMode(hold_off_pin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(light_input_pin, INPUT);
  pinMode(light_output_pin, OUTPUT);
  pinMode(pir_input_pin, INPUT);
  pinMode(ext_reset_pin, OUTPUT);
  attachInterrupt(interruptPin, wake_up, CHANGE);
}


void wake_up()
{
  sleep_disable();         // first thing after waking from sleep:
  detachInterrupt(interruptPin);  
}


void sleep_now()
{
  digitalWrite(hold_off_pin, LOW);
  digitalWrite(light_output_pin, LOW);  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  sleep_enable();          // enables the sleep bit in the mcucr register
  attachInterrupt(interruptPin, wake_up, CHANGE);
  sleep_mode();            // here the device is actually put to sleep!!
  wake_up();
}


void counter()
{
//Start counter for 2 mins, if PIR changes to HIGH in that time then restart counter
  delay(200000);
  if (pir_input_pin = HIGH)
  {
    counter();
  }
  else
  {
    sleep_now();
  }
}


void loop() 
{
//check if hold off pin is high
//if hold off pin is high, attach interrupt and go to sleep
  if (hold_off_pin = HIGH)
  {
    sleep_now();
  }
  else
  {
//if hold off pin is pin is LOW, send reset pulse    
    digitalWrite(ext_reset_pin, HIGH);
    delay(100);
    digitalWrite(ext_reset_pin, LOW);    
  }

//Read lighting value - if lower than threshold DigitalWrite light pin to HIGH, else LOW
  illuminance = digitalRead(light_input_pin);
  if (illuminance < illuminance_threshold)
  {
    digitalWrite(light_output_pin, HIGH);
  }
  else
  {
    digitalWrite(light_output_pin, LOW);
  }

//DigitalWrite PIR active pin to HIGH
  digitalWrite(hold_off_pin, HIGH);
  
//Start counter for 2 mins, if PIR changes to HIGH in that time then restart counter
  counter();
  

  
//Attach interrupt and go back to sleep
  sleep_now();

}
