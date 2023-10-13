#include "DHT.h"
#include <OneWire.h>  
#include <DallasTemperature.h>

#define ecoPin 11 // JSN-SR04T
#define triggerPin 26 // JSN-SR04T
#define DHTPIN 22     // pode ser qualquer pino digital
#define DHTTYPE DHT22  
#define pinDS18 24 /*o pino de pinDS18 do sensor está ligado na porta 2 do Arduino*/ 

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(pinDS18);  /*Protocolo OneWire*/
DallasTemperature sensors(&oneWire); //ds18b20

float umidade, temperatura_dht, calor;      // variaveis do dht22
long duration; // variavel do sensor ultrassonico
int dist; // variable for the distance measurement

void setup() {
  Serial.begin(9600);
  dht.begin();
  init_ds18b20();
  pinMode(triggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ecoPin, INPUT); // Sets the echoPin as an INPUT
}

void loop(){
  read_data_dht(); 
  verify_dht();
  data_SR04T();
  print_data_SR04T();
  print_data_dht();
  print_data_ds18b20();
}

void read_data_dht(){
  delay(2000);
  umidade = dht.readHumidity();
  temperatura_dht = dht.readTemperature();
  calor = dht.computeHeatIndex(temperatura_dht, umidade, false);

}

void print_data_dht(){
  Serial.print(F("Umidade: "));
  Serial.print(umidade);
  Serial.print(F("%\nTemperatura: "));
  Serial.print(temperatura_dht);
}

void verify_dht(){
    if (isnan(umidade) || isnan(temperatura_dht)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }
}

void init_ds18b20(){
  Serial.begin(9600); /*definição de Baudrate de 9600*/
  sensors.begin(); /*inicia biblioteca*/
}

void print_data_ds18b20(){
  Serial.println("Respostas do sensor DS18B20: ");
  sensors.requestTemperatures(); /* Envia o comando para leitura da temperatura */
  Serial.print("Temperatura: "); /* Printa "A temperatura é:" */
  Serial.print(sensors.getTempCByIndex(0)); /* Endereço do sensor */

}
  
void data_SR04T(){
  digitalWrite(triggerPin, LOW);  //
  delayMicroseconds(2);// Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(ecoPin, HIGH);
  dist = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
}

void print_data_SR04T(){
  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.println(" cm");// working  code for aj-sr04m
} 