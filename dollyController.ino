
//---------------------------------------------------------//
//---------------------------------------------------------//
//------------- Arduino Mega Dolly Controller -------------//
//---- This sketch allows to control a dolly using an -----//
//- Arduino Mega 2560 board with a TFT Touchscreen shield -//
//---------------------------------------------------------//
//----- code by Andrea Fioroni (andrifiore@gmail.com) -----//
//---------------------------------------------------------//
//---------------------------------------------------------//

//------- INCLUDES -------//
#include <Stepper.h>;
#include <SPI.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_TFTLCD.h> // not needed -> implemented by MCUFRIEND_kbv
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
//------------------------//

// delay value for loop() code
#define DELAY 100

#define DEFAULT_STEP_SPEED 20
#define STEP_MIN_SPEED 0
#define STEP_MAX_SPEED 60
#define STEP_INCREASE 2
#define STEPS 64

//------------------------//
// PROJECT SPECIFICATIONS //
//------------------------//
// dolly length, millimeters
#define DOLLY_LENGTH 2000 // 2 meters
// single step length, millimeters
#define STEP_LENGTH 0.1
#define TIME_H_MAX 24
/*
    E.G. 2m dolly, with 1 millimeter step motor => 2000 step for the full dolly
    so with a STEP_SPEED of 6 steps/s it will take (2000*6) seconds (12000 sec, or 200 min, or 3,3 hours
*/

//------------------------//

// RUNTIME VARIABLES
int CAMERA_POSITION = 0;
int STEPPING_VALUE = 10;
int STEP_SPEED = 6;
int RUNNING = 0;
int RESET_STATE = 0;
int STOP_STATE = 0;
float END_POSITION; // number of steps to reach the dolly end

// TIME VARIABLES
int timeH = 0;
int timeM = 10;
int timeS = 0;
int elapsedH = 0;
int elapsedM = 0;
int elapsedS = 0;
float startTime;
float elapsedTime;
float stoppedTime = 0.0;

int timeEval = 0;

void setup() {
  Serial.begin(9600);
  END_POSITION = DOLLY_LENGTH / STEP_LENGTH;

  Serial.println("-------------------------------------------");
  Serial.print("Dolly length is: ");
  Serial.print(DOLLY_LENGTH);
  Serial.print("mm - Step length is: ");
  Serial.print(STEP_LENGTH);
  Serial.print("mm - End position (in steps) is:");
  Serial.println(END_POSITION);
  Serial.println("-------------------------------------------");

  motorSetup();
  displaySetup();
  drawScreen();
}

void loop() {
  //timeEval = millis();                // timeEval is a debugging variable
  touchDetection();                     // touchDetection() usually takes around 1 ms
  //timeEval = millis() - timeEval;
  //Serial.print("Function touchDetection() took ");
  //Serial.print(timeEval);
  //Serial.println(" milliseconds");
  if (RUNNING == 1) {
    motorStep(STEPPING_VALUE);
    //timeEval = millis();
    updatePosition(0);                  // updatePosition() usually takes around 51 ms
    //timeEval = millis() - timeEval;
    //Serial.print("Function updatePosition() took ");
    //Serial.print(timeEval);
    //Serial.println(" milliseconds");
  }
  if (RUNNING == 0) {
    delay(DELAY);
  }
}


