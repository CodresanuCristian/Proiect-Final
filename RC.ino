#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <TFT.h>


#define LED 9
int X_JOY = A0;
int Y_JOY = A1;
int BTN_UP = 2;
int BTN_SELECT = 6;
int BTN_DOWN = 10;

struct package
{
  int servoPos, motorSpeed, hazards = 0, lights = 0, brake = 0, reverse = 0;
};
typedef struct package Package;
Package data;


const byte address[6] = "000001";
const byte ledIntensity = 10;
byte navIndex = 0;
int acceleration = 0;
bool upBtnState, selectBtnState, downBtnState;

RF24 RC(7, 8);
TFT Screen(3, 5, 4);



//================================ SETUP ======================================//

void setup() 
{
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  pinMode(X_JOY, INPUT);
  pinMode(Y_JOY, INPUT);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  RC.begin();
  RC.setChannel(115);
  RC.setPALevel(RF24_PA_MAX);
  RC.setDataRate(RF24_250KBPS);
  RC.openWritingPipe(address);
  RC.stopListening();

  Screen.begin();
  Screen.background(255, 255, 255);
  drawMenu();
}


//============================== LOOP ========================================//

void loop() 
{
  analogWrite(LED, ledIntensity);       LED on -> RC is on

  upBtnState = digitalRead(BTN_UP);
  selectBtnState = digitalRead(BTN_SELECT);
  downBtnState = digitalRead(BTN_DOWN);

  data.servoPos = analogRead(X_JOY);
  data.motorSpeed = analogRead(Y_JOY);

  if (data.motorSpeed >= 512){
      acceleration = 1;
      data.brake = 0;
      data.reverse = 0;
  }
  if ((data.motorSpeed < 512) and (acceleration == 1)){
      data.brake = 1;
      acceleration = 0;
  }
  if ((data.motorSpeed < 512) and (acceleration == 0)){
      data.reverse = 1;
      data.brake = 0;
  }
  
  data.servoPos = map(data.servoPos, 0 , 1023, 1000, 1500);
  data.motorSpeed = map(data.motorSpeed, 0, 1023, 1000, 2000);

  controlMenu();
  RC.write(&data, sizeof(data));
}


//=========================== FUNCTIONS =================================================//


void drawMenu()
{  
  Screen.background(245, 245, 245);             
  Screen.stroke(0, 0, 0);                      
  Screen.setTextSize(1);                                  // build shapes, borders and color them  
  Screen.text("RC CAR SETUP", 50, 10);          

  Screen.fill(0, 0, 0);                                         
  Screen.rect(3, 27, Screen.width()-4, Screen.height()-28);     
  Screen.setTextSize(1);                                        

  
  drawOptionMenu(navIndex);
}

//--------------------------------------------------------------------


void controlMenu()
{
  if (upBtnState == 0)                // menu functionality.
  {                                   // navigation from buttons    
    if (navIndex == 0)
      navIndex = 1;
    else
      navIndex = 0;
    drawOptionMenu(navIndex);
  }

    
  if (downBtnState == 0)
  {
    if (navIndex == 0)
      navIndex = 1;
    else
      navIndex = 0;
    drawOptionMenu(navIndex);
  }


  if (selectBtnState == 0)
  {
    if (navIndex == 0)
        if (data.hazards == 0) data.hazards = 1;
        else data.hazards = 0;
    if (navIndex == 1)
        if (data.lights < 2) data.lights++;
        else data.lights = 0;
    drawOptionMenu(navIndex);
  }
}

//---------------------------------------------------------------------


void drawOptionMenu(byte navIndex)
{  
  Screen.stroke(0, 0, 0);               // refresh the menu (options and their states)
  Screen.text("~", 15, 50);             // navigation from buttons
  Screen.text("~", 15, 70);
  Screen.text("~", 15, 90);

  Screen.stroke(245, 245, 245);
  if (navIndex == 0)
    Screen.text("~", 15, 50);
  if (navIndex == 1)
    Screen.text("~", 15 , 70);
  if( navIndex == 2)
    Screen.text("~", 15, 90);
 
  Screen.text("HAZARDS: ", 25, 50);
  Screen.text("LIGHTS: ", 25, 70);
  Screen.text("TUBES: ", 25, 90);

  if (data.hazards == 0){
    Screen.stroke(0, 0, 0);
    Screen.text("ON ", 115, 50);
    Screen.stroke(245, 245, 245);
    Screen.text("OFF ", 115, 50);
  }
  else{
    Screen.stroke(0, 0, 0);
    Screen.text("OFF ", 115, 50);
    Screen.stroke(245, 245, 245);
    Screen.text("ON ", 115, 50);
  }


  if (data.lights == 0){
    Screen.stroke(0, 0, 0);
    Screen.text("LONG", 115, 70);
    Screen.stroke(245, 245, 245);
    Screen.text("OFF", 115, 70);
  }
  if (data.lights == 1){
    Screen.stroke(0, 0, 0);
    Screen.text("OFF", 115, 70);
    Screen.stroke(245, 245, 245);
    Screen.text("SHORT", 115, 70);
  }
  if (data.lights == 2){
    Screen.stroke(0, 0, 0);
    Screen.text("SHORT", 115, 70);
    Screen.stroke(245, 245, 245);
    Screen.text("LONG", 115, 70);
  }

}   // end function
