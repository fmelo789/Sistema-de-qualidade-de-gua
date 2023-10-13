#define ecoPin 11 // attach pin D2 Arduino to pin Echo of JSN-SR04T
#define triggerPin 26 //attach pin D3 Arduino to pin Trig of JSN-SR04T

long duration; // variable for the duration of sound wave travel
int dist; // variable for the distance measurement
 
void setup() {
  Serial.begin(9600); 
  pinMode(triggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ecoPin, INPUT); // Sets the echoPin as an INPUT

}

void loop() {

  data_SR04T();
  print_data_SR04T();
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