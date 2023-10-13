#include <Wire.h>
#include "DHT.h"
#include <OneWire.h>  
#include <DallasTemperature.h>

#define ecoPin 11 // JSN-SR04T
#define triggerPin 26 // JSN-SR04T
#define DHTPIN 22     // pode ser qualquer pino digital
#define DHTTYPE DHT22  
#define pinDS18 24 /*o pino de pinDS18 do sensor está ligado na porta 2 do Arduino*/ 
#define TdsSensorPin A1
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(pinDS18);  /*Protocolo OneWire*/
DallasTemperature sensors(&oneWire); //ds18b20

float umidade, temperatura_dht, calor;      // variaveis do dht22
long duration; // variavel do sensor ultrassonico
int dist; // variavel do sensor ultrassonico
float calibration_value = 22.189; // (VALOR DE CALIBRACION OBTENIDO DE GRAFICA FORMULA DE EXCEL)
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
float volt, ph_act;
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;


void setup(){
  Serial.begin(9600);
  dht.begin();
  init_ds18b20();
  pinMode(triggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ecoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(TdsSensorPin,INPUT);
  
}
void loop(){
  read_data_dht(); 
  verify_dht();
  data_SR04T();
  print_data_SR04T();
  print_data_dht();
  print_data_ds18b20();
  get_data_ph();
  print_data_ph();
  get_data_TDS();
  print_data_TDS();

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

void get_data_ph(){
  for (int i = 0; i < 10; i++){
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++){
    for (int j = i + 1; j < 10; j++){
      if (buffer_arr[i] > buffer_arr[j]){
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++){
    avgval += buffer_arr[i];
  }
  volt = (float)avgval * 5.0 / 1024 / 6;
  ph_act = -6.3094 * volt + calibration_value; // (ACOMPLETAR VALORES CON DATOS OBTENIDOS DE FORMULA GRAFICA DE EXCEL)

}
void print_data_ph(){
  Serial.print("volt val:");
  Serial.println(volt);
  Serial.print("PH:");
  Serial.println(ph_act);
  delay(1000);
}

void get_data_TDS(){

  static unsigned long analogSampleTimepoint = millis();
  static unsigned long printTimepoint = millis();

  if(millis()-analogSampleTimepoint > 40U){
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT)
    analogBufferIndex = 0;
  }
  
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
    analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF/ 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
    tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value

  }
}

void print_data_TDS(){
  Serial.print("TDS Value:");
  Serial.print(tdsValue,0);
  Serial.println("ppm");
}


int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++){
    for (i = 0; i < iFilterLen - j - 1; i++){
      if (bTab[i] > bTab[i + 1]){
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
        }
      }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }else{
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}
