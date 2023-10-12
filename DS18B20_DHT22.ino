#include "DHT.h"
#include <OneWire.h>  
#include <DallasTemperature.h>

#define DHTPIN 22     // pode ser qualquer pino digital
#define DHTTYPE DHT22  
#define pinDS18 2 /*o pino de pinDS18 do sensor está ligado na porta 2 do Arduino*/ 

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(pinDS18);  /*Protocolo OneWire*/
DallasTemperature sensors(&oneWire); //ds18b20

float umidade, temperatura_dht, calor;

void setup() {
  Serial.begin(9600);
  dht.begin();
  init_ds18b20();
}

void loop(){
  read_data_dht(); 
  show_data_dht();
  verify_dht();
  data_ds18b20();
}

void read_data_dht(){
  delay(2000);
  umidade = dht.readHumidity();
  temperatura_dht = dht.readTemperature();
  calor = dht.computeHeatIndex(temperatura_dht, umidade, false);

}

void show_data_dht(){
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

void data_ds18b20(){
  Serial.println("Respostas do sensor DS18B20: ");
  sensors.requestTemperatures(); /* Envia o comando para leitura da temperatura */
  Serial.print("Temperatura: "); /* Printa "A temperatura é:" */
  Serial.print(sensors.getTempCByIndex(0)); /* Endereço do sensor */

}