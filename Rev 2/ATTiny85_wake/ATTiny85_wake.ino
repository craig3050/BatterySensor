//Setup input output pins
const int interruptPin = 2;
const int esp_completed_pin = 3;
const int pir_active_pin = 4;
const int power_rail_switch = 5;
volatile byte pir_active_flag = LOW;
//Bits for delay
unsigned long previousMillis = 0;  
const long interval = 10000; 

void setup() {
  pinMode(interruptPin, INPUT);
  pinMode(pir_active_pin, OUTPUT);
  pinMode(power_rail_switch, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pirActive, RISING);
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
  digitalWrite(pir_active_flag, HIGH);
}

void deep_sleep() {
  digitalWrite(pir_active_flag, LOW); 
  digitalWrite(power_rail_switch, LOW);
  //sleep for 30 mins
}




