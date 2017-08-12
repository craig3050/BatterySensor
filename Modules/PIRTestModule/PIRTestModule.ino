//Global Variables
int pir_pin = 17;     // choose the input pin (for PIR sensor)
int pir_state = LOW;  // we start, assuming no motion detected
int pir_state = 0;    // variable for reading the pin status

void setup()
{
  pinMode(pir_pin, INPUT);  // declare sensor as input
  Serial.begin(115200);
}
 
void pir_loop()
{
  pir_state = digitalRead(pir_pin);
  if (pir_state == HIGH) {            // check if the input is HIGH
    if (pir_state == LOW) {
      Serial.println("Motion detected!");
      pir_state = HIGH;
    }
  } else {
    if (pir_state == HIGH){
      Serial.println("Motion ended!");
      pir_state = LOW;
    }
  }
}

void loop()
{
  pir_loop(); 
}

