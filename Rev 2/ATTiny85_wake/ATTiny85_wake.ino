//Craig3050
//Home sensor wake-up and power control manager
//18.06.2018


#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

//Setup input output pins
const int interruptPin = 3; //PCINT3 = PB3 = Pin 2
const int esp_completed_pin = 2; //PB2 = Pin 7
const int pir_active_pin = 4; //PB4 = Pin 3
const int power_rail_switch = 1; //PB1 = Pin 6
volatile byte pir_active_flag = LOW;
const long interval = 10000; //interval for delay in ms - approx 10 seconds
int watchdog_counter = 0; //Set variable for watchdog counter
const int watchdog_sleep_count  = 225; //this is approx 8sec *225 times = 30mins

void setup() {
  pinMode(interruptPin, INPUT);
  pinMode(esp_completed_pin, INPUT);
  pinMode(pir_active_pin, OUTPUT);
  pinMode(power_rail_switch, OUTPUT);
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
  setup_watchdog(9); // approximately 8 seconds sleep
  }

void deep_sleep() {
  pir_active_flag = LOW; 

  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on
  } 

void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

ISR(PCINT3_vect) {
  // This is called when the interrupt occurs
  pir_active_flag = HIGH; 
}

ISR(WDT_vect) {
  pir_active_flag = LOW;
  watchdog_counter++;
}

void power_on() {
  digitalWrite(power_rail_switch, HIGH); //switch on power rail
  
  //define items for delay loop
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;  
  previousMillis = currentMillis;
  
  while (currentMillis - previousMillis <= interval)
  {
    currentMillis = millis(); //update current delay timer
    if (digitalRead(esp_completed_pin) == HIGH) {
    break;
    }    
  }
  digitalWrite(power_rail_switch, LOW); //switch off power rail
}

void loop() {
  if (watchdog_counter > watchdog_sleep_count)
  {
    watchdog_counter = 0;
    digitalWrite(pir_active_pin, LOW);
    power_on();
  }
  else if (pir_active_flag == HIGH) {
    digitalWrite(pir_active_pin, HIGH);
    power_on();
  }
  else {
    deep_sleep();
  }
}


