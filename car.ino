#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Servo.h>

#define FRONTLIGHTS 10
#define BACKLIGHTS 11
#define REVERSELIGHTS 12
#define HAZARDLIGHTS 13

struct package
{
  int servoPos, motorSpeed, hazards, lights, brake, reverse;
};
typedef struct package Package;
package data;

const byte address[6] = "000001";
unsigned long prevMillis = 0, currentMillis;
bool hazardsLedState = 1;


RF24 car(7, 8);
Servo myServo, ESC;




//============================ SETUP ========================================//

void setup() 
{
  Serial.begin(9600);

  pinMode(FRONTLIGHTS, OUTPUT);
  pinMode(BACKLIGHTS, OUTPUT);
  pinMode(REVERSELIGHTS, OUTPUT);
  pinMode(HAZARDLIGHTS, OUTPUT);

  myServo.attach(6);
  ESC.attach(9);

  car.begin();
  car.setChannel(115);
  car.setPALevel(RF24_PA_MAX);
  car.setDataRate(RF24_250KBPS);
  car.openReadingPipe(0, address);
  car.startListening();
}



//=========================== LOOP ==========================================//

void loop() 
{
  if (car.available())
    car.read(&data, sizeof(data));

  lights(); 
      
  ESC.writeMicroseconds(data.motorSpeed);
  myServo.writeMicroseconds(data.servoPos);
}


//========================= FUNCTIONS ====================================//


void lights()
{
  if (data.hazards == 1)
    hazardsBlink();           
  else
    digitalWrite(HAZARDLIGHTS, LOW);


  if (data.lights != 0){
    if (data.lights == 1) analogWrite(FRONTLIGHTS, 40);    // Short Phase
    if (data.lights == 2) analogWrite(FRONTLIGHTS, 100);   // Long Phase 
    analogWrite(BACKLIGHTS, 30);
  }
  else{
    digitalWrite(FRONTLIGHTS, LOW);
    digitalWrite(BACKLIGHTS, LOW);
  }

  
  if (data.brake == 1)
    analogWrite(BACKLIGHTS, 100);

  if (data.reverse == 1)
    digitalWrite(REVERSELIGHTS, HIGH);
  else
    digitalWrite(REVERSELIGHTS, LOW);
}

//-------------------------------------------------------------

void hazardsBlink()
{
  currentMillis = millis();

  if ((currentMillis - prevMillis) >= 500)
  {
    prevMillis = currentMillis;

    if (hazardsLedState == 1)
      hazardsLedState = 0;
    else
      hazardsLedState = 1;

    digitalWrite(HAZARDLIGHTS, hazardsLedState);
  }
}
