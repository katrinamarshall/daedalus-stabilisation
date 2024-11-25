#define PWMPin 36
#define directionPin 33

void setup() {
  pinMode(PWMPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  analogWrite(PWMPin, 255); // PWM resolution is 8-bits between 0-255
  digitalWrite(directionPin, 1);
}
