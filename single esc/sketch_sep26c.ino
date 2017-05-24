#include <Servo.h>
 
Servo esc;
int throttlePin = 9;
 
void setup()
{
esc.attach(9);
Serial.begin(9600);
}
 
void loop()
{
int throttle = Serial.read();
throttle = map(throttle, 0, 1023, 0, 179);
esc.write(throttle);
}
