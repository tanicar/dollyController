
//---------------------------------------------------------//
//----------------- MOTOR VARIABLES -----------------------//
//---------------------------------------------------------//

// MOTOR 1 PINS
#define M1P1 31
#define M1P2 33
#define M1P3 35
#define M1P4 37
// TODO: Setup proper motor pins and steps
// motor's steps and speed
#define RESET_SPEED 60

// MOTOR VARIABLES
Stepper M1F(STEPS, M1P1, M1P2, M1P3, M1P4); // Motor 1 forward ->
Stepper M1B(STEPS, M1P1, M1P4, M1P2, M1P3); // Motor 1 backward <-

void motorSetup() {
  M1F.setSpeed(STEP_SPEED);
  M1B.setSpeed(STEP_SPEED);
}

void motorStep(int steps) {
  if ((CAMERA_POSITION + STEPPING_VALUE) >= END_POSITION) {
    RUNNING = 0;
    Serial.println("END POSITION REACHED, STOPPING NOW");
    return;
  }
  M1F.step(steps);
  CAMERA_POSITION = CAMERA_POSITION + steps;
  // DEBUG CODE
  /*
    Serial.print("Camera Position: ");
    Serial.print(CAMERA_POSITION);
    Serial.println(" steps.");
  */
}

void motorReset() {
  STEP_SPEED = RESET_SPEED;
  stepSpeed(STEP_SPEED);
  updateSpeed(0);
  while (CAMERA_POSITION >= STEPPING_VALUE) {
    M1B.step(STEPPING_VALUE);
    CAMERA_POSITION = CAMERA_POSITION - STEPPING_VALUE;
    updatePosition(0);
  }
  STEP_SPEED = DEFAULT_STEP_SPEED;
  stepSpeed(STEP_SPEED);
  updateSpeed(0);
}

void stepSpeed(int value) {
  M1F.setSpeed(value);
  M1B.setSpeed(value);
}

