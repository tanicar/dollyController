
//---------------------------------------------------------//
//---------------- DISPLAY VARIABLES ----------------------//
//---------------------------------------------------------//

// LCD pins
#define LCD_CS A3 // SHARED PIN!
#define LCD_CD A2 // SHARED PIN!
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Touchscreen pins
#define YP A3 // SHARED PIN!
#define XM A2 // SHARED PIN!
#define YM 9
#define XP 8

// Color names
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define A_CYAN  0X0268
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// LCD dimension
#define WIDTH 320
#define HEIGTH 240

// Touchscreen effective values                  // TODO: Setup TS values
#define X_MIN 120
#define X_MAX 905
#define Y_MIN 75
#define Y_MAX 965

// Toucscreen pressure margins
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Touchscreen realtime variables
MCUFRIEND_kbv tft;
uint16_t ID; // display ID
TSPoint tsLastP; // Last pressed point
TSPoint tsCurrentP; // Current pressed point
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Display realtime variables
/*
  int speedUp = 0;
  int speedDown = 0;
  int speedDownX = 220;
  int speedDownY = 38;
  int speedUpX = 250;
  int speedUpY = 38;
  int speedButtonDim = 20;
*/
// TIMELAPSE DURATION BUTTONS
int hUpX = 22;
int hUpY = 40;
int hDownX = 22;
int hDownY = 105;
int mUpX = 77;
int mUpY = 40;
int mDownX = 77;
int mDownY = 105;
int sUpX = 130;
int sUpY = 40;
int sDownX = 130;
int sDownY = 105;
int timeButtonSize = 30;
int timelapseY = 77;
int elapsedY = 180;

// control panel position
int paneX = 210;
int paneY = 130;
int paneW = 88;
int paneH = 100;
// start button position
int startX = 220;
int startY = 140;
int startW = 68;
int startH = 20;
// stop button position
int stopX = 220;
int stopY = 170;
int stopW = 68;
int stopH = 20;
// reset button position
int resetX = 220;
int resetY = 200;
int resetW = 68;
int resetH = 20;

void displaySetup() {
  if (!Serial) delay(5000); // wait some time if Serial is not up
  ID = tft.readID(); // read display ID
  Serial.print("Display available with ID = 0x");
  Serial.println(ID, HEX);

  tft.begin(ID); // Start TFT display
  tft.setRotation(1); // Horizontal display
  Serial.print("Screen resolution: ");
  Serial.print(WIDTH);
  Serial.print("x");
  Serial.println(HEIGTH);
  tft.fillScreen(BLACK); // black screen
  /*for (int line = 0; line <= WIDTH; line+=2) {
    tft.drawLine(0,HEIGTH,line,0,BLACK);
    }
    for (int line = 0; line <= WIDTH; line+=2) {
    tft.drawLine(WIDTH,0,line,HEIGTH,BLACK);
    }*/

  tft.setTextSize(2);
  tsPinMode();
  tsLastP = ts.getPoint();
}

//----------------------------------------//
//---- Touchscreen detection function ----//
//----------------------------------------//
void touchDetection() {
  tsPinMode();

  tsCurrentP = ts.getPoint();
  if (tsCurrentP.z < MINPRESSURE) {
    return; // stop if no touch is detected
  }
  int y = mapY(tsCurrentP.x) - 15;
  int x = mapX(tsCurrentP.y);

  if (tsCurrentP == tsLastP) {
    return; // stop if touching the same point than before
  }

  y = y + 15;
  if (x >= resetX && x <= (resetX + resetW)) {
    if (y >= resetY && y <= (resetY + resetH)) { // RESET BUTTON PRESSED
      Serial.println("RESET BUTTON PRESSED");
      resetPosition();
      return;
    }
  }
  y = y + 10;
  if (x >= startX && x <= (startX + startW)) {
    if (y >= startY && y <= (startY + startH)) { // START BUTTON PRESSED
      Serial.println("START BUTTON PRESSED");
      start();
      return;
    }
  }
  if (x >= stopX && x <= (stopX + stopW)) {
    if (y >= stopY && y <= (stopY + stopH)) { // STOP BUTTON PRESSED
      Serial.println("STOP BUTTON PRESSED");
      stopMotor();
      return;
    }
  }
  if (RUNNING == 1) {
    return; // DISABLE TIMELAPSE DURATION BUTTONS DURING TIMELAPSE
  }
  y = y - 25;
  if (x >= hUpX && x <= (hUpX + timeButtonSize)) {
    if (y >= hUpY && y <= (hUpY + timeButtonSize)) { // H_UP BUTTON PRESSED
      Serial.println("H_UP BUTTON PRESSED");
      if (timeH + 1 > TIME_H_MAX) {
        Serial.print("Can't last more than ");
        Serial.print(TIME_H_MAX);
        Serial.println("hours");
        timeH = 0;
        updateSpeed(0);
        return;
      }
      timeH = timeH + 1;
      updateSpeed(0);
      return;
    }
  }
  if (x >= hDownX && x <= (hDownX + timeButtonSize)) {
    if (y >= hDownY && y <= (hDownY + timeButtonSize)) { // H_DOWN BUTTON PRESSED
      Serial.println("H_DOWN BUTTON PRESSED");
      if (timeH == 0) {
        timeH = 24;
        updateSpeed(0);
        return;
      }
      timeH = timeH - 1;
      updateSpeed(0);
      return;
    }
  }
  if (x >= mUpX && x <= (mUpX + timeButtonSize)) {
    if (y >= mUpY && y <= (mUpY + timeButtonSize)) { // M:UP BUTTON PRESSED
      Serial.println("M_UP BUTTON PRESSED");
      if (timeM == 59) {
        timeM = 0;
        updateSpeed(0);
        return;
      }
      timeM = timeM + 1;
      updateSpeed(0);
      return;
    }
  }
  if (x >= mDownX && x <= (mDownX + timeButtonSize)) {
    if (y >= mDownY && y <= (mDownY + timeButtonSize)) { // M_DOWN BUTTON PRESSED
      Serial.println("M_DOWN BUTTON PRESSED");
      if (timeM == 0) {
        timeM = 59;
        updateSpeed(0);
        return;
      }
      timeM = timeM - 1;
      updateSpeed(0);
      return;
    }
  }
  if (x >= sUpX && x <= (sUpX + timeButtonSize)) {
    if (y >= sUpY && y <= (sUpY + timeButtonSize)) { // S_UP BUTTON PRESSED
      Serial.println("S_UP BUTTON PRESSED");
      if (timeS == 59) {
        timeS = 0;
        updateSpeed(0);
        return;
      }
      timeS = timeS + 1;
      updateSpeed(0);
      return;
    }
  }
  if (x >= sDownX && x <= (sDownX + timeButtonSize)) {
    if (y >= sDownY && y <= (sDownY + timeButtonSize)) { // S_DOWN BUTTON PRESSED
      Serial.println("S_DOWN BUTTON PRESSED");
      if (timeS == 0) {
        timeS = 59;
        updateSpeed(0);
        return;
      }
      timeS = timeS - 1;
      updateSpeed(0);
      return;
    }
  }
}

//----------------------------------------//
//---- TFT Display drawing functions -----//
//----------------------------------------//
void drawScreen() {
  tftPinMode();

  // DEBUG CODE
  /*tft.setCursor(20, 20);
    tft.print("Stepping Value:");
    tft.print(STEPPING_VALUE);*/

  updateSpeed(1);

  // DEBUG CODE
  /*
    tft.setTextColor(BLACK);
    // SPEED DOWN BUTTON
    tft.fillRect(speedDownX, speedDownY, speedButtonDim, speedButtonDim, A_CYAN);
    tft.setCursor(speedDownX + 5, speedDownY + 3);
    tft.print("-");
    // SPEED UP BUTTON
    tft.fillRect(speedUpX, speedUpY, speedButtonDim, speedButtonDim, A_CYAN);
    tft.setCursor(speedUpX + 5, speedUpY + 3);
    tft.print("+");*/

  // TIMELAPSE DURATION BUTTONS
  tft.fillRect(hUpX, hUpY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(hUpX, hUpY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(hUpX + 10, hUpY + 8);
  tft.print("+");
  tft.fillRect(hDownX, hDownY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(hDownX, hDownY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(hDownX + 10, hDownY + 8);
  tft.print("-");
  tft.fillRect(mUpX, mUpY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(mUpX, mUpY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(mUpX + 10, mUpY + 8);
  tft.print("+");
  tft.fillRect(mDownX, mDownY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(mDownX, mDownY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(mDownX + 10, mDownY + 8);
  tft.print("-");
  tft.fillRect(sUpX, sUpY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(sUpX, sUpY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(sUpX + 10, sUpY + 8);
  tft.print("+");
  tft.fillRect(sDownX, sDownY, timeButtonSize, timeButtonSize, A_CYAN);
  tft.drawRect(sDownX, sDownY, timeButtonSize, timeButtonSize, WHITE);
  tft.setCursor(sDownX + 10, sDownY + 8);
  tft.print("-");

  // CONTROL PANEL
  tft.fillRect(paneX, paneY, paneW, paneH, A_CYAN);
  // RESET BUTTON
  tft.setTextColor(BLACK);
  tft.fillRect(resetX, resetY, resetW, resetH, YELLOW);
  tft.drawRect(resetX, resetY, resetW, resetH, WHITE);
  tft.setCursor(resetX + 5, resetY + 3);
  tft.print("RESET");
  // STOP BUTTON
  tft.setTextColor(WHITE);
  tft.fillRect(stopX, stopY, stopW, stopH, RED);
  tft.drawRect(stopX, stopY, stopW, stopH, WHITE);
  tft.setCursor(stopX + 5, stopY + 3);
  tft.print("STOP");
  // START BUTTON
  tft.setTextColor(BLUE);
  tft.fillRect(startX, startY, startW, startH, GREEN);
  tft.drawRect(startX, startY, startW, startH, WHITE);
  tft.setCursor(startX + 5, startY + 3);
  tft.print("START");

  // signature
  tft.setTextSize(1);
  tft.setTextColor(A_CYAN);
  tft.setCursor(10, 230);
  tft.print("Sketch by Andrea Fioroni");
  tft.setTextSize(2);
}

void updateSpeed(int startup) {
  tftPinMode();

  tft.setTextColor(WHITE);
  if (startup = 1) {
    tft.setCursor(20, 20);
    tft.print("Set timelapse duration:");
  }
  tft.fillRect(20, timelapseY, 141, 21, BLACK);
  tft.setCursor(20, timelapseY);
  tft.setTextSize(3);
  if (timeH < 10) {
    tft.print("0");
  }
  tft.print(timeH);
  tft.print(":");
  if (timeM < 10) {
    tft.print("0");
  }
  tft.print(timeM);
  tft.print(":");
  if (timeS < 10) {
    tft.print("0");
  }
  tft.print(timeS);

  // DEBUG CODE
  /*
    tft.setTextColor(WHITE);
    if (startup == 1) {
    tft.setCursor(20, 40);
    tft.print("Current Speed:");
    }
    tft.fillRect(185, 36, 28, 20, BLACK);
    tft.setCursor(188, 40);
    tft.print(STEP_SPEED);
    stepSpeed(STEP_SPEED);*/
  tft.setTextSize(2);
}

void updatePosition(int startup) {
  tftPinMode();

  elapsedTime = millis() - startTime;
  elapsedS = elapsedTime / 1000; // seconds
  elapsedM = 0;
  elapsedH = 0;
  while (elapsedS >= 60) {
    elapsedS = elapsedS - 60;
    elapsedM = elapsedM + 1;
  }
  while (elapsedM >= 60) {
    elapsedM = elapsedM - 60;
    elapsedH = elapsedH + 1;
  }
  if (RESET_STATE == 1) {
    tft.fillRect(20, 160, 160, 21, BLACK);
    tft.fillRect(20, elapsedY, 141, 21, BLACK);
    return;
  }
  tft.setTextColor(WHITE);
  if (startup == 1) {
    tft.setCursor(20, 160);
    tft.print("Elapsed Time:");
    tft.setCursor(20, elapsedY);
    tft.setTextSize(3);
    if (elapsedH < 10) {
      tft.print("0");
    }
    tft.print(elapsedH);
    tft.print(":");
    if (elapsedM < 10) {
      tft.print("0");
    }
    tft.print(elapsedM);
    tft.print(":");
    if (elapsedS < 10) {
      tft.print("0");
    }
    tft.print(elapsedS);

    return;
  }
  tft.setTextSize(3);
  tft.fillRect(128, elapsedY, 33, 21, BLACK);
  tft.setCursor(128, elapsedY);
  if (elapsedS < 10) {
    tft.print("0");
  }
  tft.print(elapsedS);
  if (elapsedS == 0) {
    tft.fillRect(74, elapsedY, 33, 21, BLACK);
    tft.setCursor(74, elapsedY);
    if (elapsedM < 10) {
      tft.print("0");
    }
    tft.print(elapsedM);
  }
  if (elapsedS == 0 && elapsedM == 0) {
    tft.fillRect(20, elapsedY, 33, 21, BLACK);
    tft.setCursor(20, elapsedY);
    tft.setTextSize(3);
    if (elapsedH < 10) {
      tft.print("0");
    }
    tft.print(elapsedH); 
  }

  tft.setTextSize(2);
}

void resetPosition() {
  if (RUNNING == 0) {
    RESET_STATE = 1;
    updatePosition(0);
    motorReset();
  } else {
    Serial.println("CAN'T RESET WHILE RUNNING, PRESS STOP FIRST");
  }
}

void start() {
  if ((CAMERA_POSITION + STEPPING_VALUE) >= END_POSITION) {
    Serial.println("END POSITION REACHED, CAN'T MOVE MORE");
    return;
  }
  if (RUNNING == 1) {
    Serial.print("Current position (in steps):");
    Serial.println(CAMERA_POSITION);
    return;
  }
                                                                              // TODO 
  /*if(STOP_STATE == 1) {
    stoppedTime = stoppedTime + millis()/1000 - startTime - elapsedTime;
    RUNNING = 1;
    STOP_STATE = 0;
    return;
  }*/

  int timeSec = (timeH * 3600) + (timeM * 60) + timeS;
  STEP_SPEED = ((END_POSITION / timeSec) / STEPS) * 60;
  STEPPING_VALUE = map(STEP_SPEED, STEP_MIN_SPEED, STEP_MAX_SPEED, 1, 64);
  if (STEPPING_VALUE > 64) {
    STEPPING_VALUE = 64;
  }
  if (STEP_SPEED > STEP_MAX_SPEED) {
    STEP_SPEED = STEP_MAX_SPEED;
  } else if (STEP_SPEED < STEP_MIN_SPEED) {
    STEP_SPEED = STEP_MIN_SPEED;
  }

  RESET_STATE = 0;
  RUNNING = 1;
  startTime = millis();
  elapsedTime = 0;
  updatePosition(1);
  stepSpeed(STEP_SPEED);

  Serial.print("Step speed is ");
  Serial.print(STEP_SPEED);
  Serial.print(", Stepping value is ");
  Serial.println(STEPPING_VALUE);
}

void stopMotor() {
  RUNNING = 0;
  //STOP_STATE = 1;
  elapsedTime = millis() - startTime;
}

//----------------------------------------//
//----- TFT Display custom functions -----//
//----------------------------------------//
void tsPinMode() { // set the Shared pins as input -> touchscreen mode
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}
void tftPinMode() { // set the Shared pins as output -> display mode
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}
int mapX(int value) { // map the value on the X_MIN - X_MAX range                             // TODO
  return map(value, X_MIN, X_MAX, 0, WIDTH);                                                  // BETTER SETUP MAPPING FUNCTIONS
}
int mapY(int value) { // map the value on the Y_MIN - Y_MAX range
  return map(value, Y_MIN, Y_MAX, 0, HEIGTH);
}


