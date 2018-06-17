#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//Setup input output pins
const int interruptPin = 2;
const int esp_completed_pin = 3;
const int pir_active_pin = 4;
const int power_rail_switch = 5;
volatile byte pir_active_flag = LOW;
//Bits for delay
unsigned long previousMillis = 0;  
const long interval = 10000; 

//specific item for sleep routine
volatile int f_wdt=1;

void setup() {
  pinMode(interruptPin, INPUT);
  pinMode(pir_active_pin, OUTPUT);
  pinMode(power_rail_switch, OUTPUT);
  
}

void loop() {
//switch on power rail
  digitalWrite(power_rail_switch, HIGH);
  
  if (pir_active_flag == HIGH){
    digitalWrite(pir_active_pin, HIGH);
  }
  else
  {
    digitalWrite(pir_active_pin, LOW);
  }

  if (esp_completed_pin == HIGH) {
    deep_sleep();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    deep_sleep();
  }
}

void pirActive() {
  pir_active_flag = HIGH;
}

void deep_sleep() {
  pir_active_flag, LOW; 
  digitalWrite(power_rail_switch, LOW);
  //sleep for 30 mins
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();   
  attachInterrupt(interruptPin, pirActive, RISING);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), pirActive, RISING);
  sleep_mode();  
  sleep_disable();   
  detachInterrupt(interruptPin);  
  //detachInterrupt(digitalPinToInterrupt(interruptPin)); 
  // Re-enable the peripherals.
  power_all_enable();
}


//Some sleep specific element
// Watchdog Interrupt Service. This is executed when watchdog timed out.
ISR(WDT_vect) {
  if(f_wdt == 0) {
    // here we can implement a counter the can set the f_wdt to true if
    // the watchdog cycle needs to run longer than the maximum of eight
    // seconds.
    f_wdt=1;
  }
}

// Setup the Watch Dog Timer (WDT)
void setupWatchDogTimer() {
  // The MCU Status Register (MCUSR) is used to tell the cause of the last
  // reset, such as brown-out reset, watchdog reset, etc.
  // NOTE: for security reasons, there is a timed sequence for clearing the
  // WDE and changing the time-out configuration. If you don't use this
  // sequence properly, you'll get unexpected results.

  // Clear the reset flag on the MCUSR, the WDRF bit (bit 3).
  MCUSR &= ~(1<<WDRF);

  // Configure the Watchdog timer Control Register (WDTCSR)
  // The WDTCSR is used for configuring the time-out, mode of operation, etc

  // In order to change WDE or the pre-scaler, we need to set WDCE (This will
  // allow updates for 4 clock cycles).

  // Set the WDCE bit (bit 4) and the WDE bit (bit 3) of the WDTCSR. The WDCE
  // bit must be set in order to change WDE or the watchdog pre-scalers.
  // Setting the WDCE bit will allow updates to the pre-scalers and WDE for 4
  // clock cycles then it will be reset by hardware.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /**
   *  Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
   *  WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
   *  0    0    0    0    |   2K cycles   | 16 ms
   *  0    0    0    1    |   4K cycles   | 32 ms
   *  0    0    1    0    |   8K cycles   | 64 ms
   *  0    0    1    1    |  16K cycles   | 0.125 s
   *  0    1    0    0    |  32K cycles   | 0.25 s
   *  0    1    0    1    |  64K cycles   | 0.5 s
   *  0    1    1    0    |  128K cycles  | 1.0 s
   *  0    1    1    1    |  256K cycles  | 2.0 s
   *  1    0    0    0    |  512K cycles  | 4.0 s
   *  1    0    0    1    | 1024K cycles  | 8.0 s
  */
  WDTCSR  = (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
  // Enable the WD interrupt (note: no reset).
  WDTCSR |= _BV(WDIE);
}



