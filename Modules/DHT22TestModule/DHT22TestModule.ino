#include <dht.h>

dht DHT;

#define DHT22_PIN 15

//Global Variables
float temperature = 0;
float humidity = 0;

void setup()
{
  Serial.begin(115200);
}

void dht_22()
{
  // READ DATA
  Serial.print("DHT22, \t");
  int chk = DHT.read22(DHT22_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  humidity = (DHT.humidity, 1);
  temperature = (DHT.temperature, 1);
}

void loop()
{
  dht_22();
  Serial.println(temperature);
  Serial.println(humidity);
  delay(1000);
}

