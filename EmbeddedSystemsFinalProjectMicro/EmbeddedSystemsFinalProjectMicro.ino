#include <Wire.h>
#include <SwitchControlLibrary.h>

typedef struct messageStruct {
  int8_t id;
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t tilt;
  float analogX;
  float analogY;
} messageStruct;

messageStruct test;


// Define Slave I2C Address
#define SLAVE_ADDR 9

void setup() {
  SwitchControlLibrary();

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);

  // Function to run when data received from master
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // Setup Serial Monitor
  Serial.begin(9600);
  // Serial.println("I2C Slave Demonstration");
}

long timeSince = millis();

void receiveEvent() {
  // Serial.println(millis()-timeSince);
  // timeSince = millis();

  // Read while data received
  byte b[24];
  int i = 0;
  while (Wire.available()) {
    b[i] = Wire.read();
    i++;
  }
  //Serial.println(i);
  // for (i = 0; i < 13; i++){
  //   Serial.print(b[i]);
  //   Serial.print(" ");
  // }
  int8_t leftX = (int8_t)b[0];
  int8_t leftY = (int8_t)b[1];
  int8_t leftZ = (int8_t)b[2];
  int8_t leftTilt = (int8_t)b[3];
  float leftAnalogX;
  unsigned char cLX[4] = { b[4], b[5], b[6], b[7] };
  memcpy(&leftAnalogX, &cLX, sizeof(leftAnalogX));
  float leftAnalogY;
  unsigned char cLY[4] = { b[8], b[9], b[10], b[11] };
  memcpy(&leftAnalogY, &cLY, sizeof(leftAnalogX));

  int8_t rightX = (int8_t)b[12];
  int8_t rightY = (int8_t)b[13];
  int8_t rightZ = (int8_t)b[14];
  int8_t rightTilt = (int8_t)b[15];
  float rightAnalogX;
  unsigned char cRX[4] = { b[16], b[17], b[18], b[19] };
  memcpy(&rightAnalogX, &cRX, sizeof(rightAnalogX));
  float rightAnalogY;
  unsigned char cRY[4] = { b[20], b[21], b[22], b[23] };
  memcpy(&rightAnalogY, &cRY, sizeof(rightAnalogX));

  Serial.print(leftX);
  Serial.print(" ");
  Serial.print(leftY);
  Serial.print(" ");
  Serial.print(leftZ);
  Serial.print(" ");
  Serial.print(leftTilt);
  Serial.print(" ");
  Serial.print((uint8_t)(128+leftAnalogX*128));
  Serial.print(" ");
  Serial.print((uint8_t)(128+leftAnalogY*128));
  Serial.print(" ");

  Serial.print(rightX);
  Serial.print(" ");
  Serial.print(rightY);
  Serial.print(" ");
  Serial.print(rightZ);
  Serial.print(" ");
  Serial.print(rightTilt);
  Serial.print(" ");
  Serial.print((uint8_t)(128+rightAnalogX*128));
  Serial.print(" ");
  Serial.print((uint8_t)(128+rightAnalogY*128));

  Serial.println();

  uint8_t leftAnalogXActual;
  if (128 + leftAnalogX * 128 == 256){
    leftAnalogXActual = 255;
  } else {
    leftAnalogXActual = (uint8_t)(128 + leftAnalogX * 128);
  }
  uint8_t leftAnalogYActual;
  if (128 + leftAnalogY * 128 == 256){
    leftAnalogYActual = 255;
  } else {
    leftAnalogYActual = (uint8_t)(128 + leftAnalogY * 128);
  }
  
  SwitchControlLibrary().MoveLeftStick(leftAnalogXActual, leftAnalogYActual);
  if (leftX == -1 && leftY == -1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::TOP_LEFT);
  } else if (leftX == -1 && leftY == 0) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::TOP);
  } else if (leftX == -1 && leftY == 1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::TOP_RIGHT);
  } else if (leftX == 0 && leftY == -1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::LEFT);
  } else if (leftX == 0 && leftY == 0) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::CENTER);
  } else if (leftX == 0 && leftY == 1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::RIGHT);
  } else if (leftX == 1 && leftY == -1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::BOTTOM_LEFT);
  } else if (leftX == 1 && leftY == 0) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::BOTTOM);
  } else if (leftX == 1 && leftY == 1) {
    SwitchControlLibrary().MoveHat((uint8_t)Hat::BOTTOM_RIGHT);
  }
  if (leftZ == -1) {
    SwitchControlLibrary().PressButtonLClick();
    SwitchControlLibrary().ReleaseButtonMinus();
  } else if (leftZ == 1) {
    SwitchControlLibrary().PressButtonMinus();
    SwitchControlLibrary().ReleaseButtonLClick();
  } else {
    SwitchControlLibrary().ReleaseButtonLClick();
    SwitchControlLibrary().ReleaseButtonMinus();
  }
  if (leftTilt == -1) {
    SwitchControlLibrary().PressButtonL();
    SwitchControlLibrary().ReleaseButtonZL();
  } else if (leftTilt == 1) {
    SwitchControlLibrary().PressButtonZL();
    SwitchControlLibrary().ReleaseButtonL();
  } else {
    SwitchControlLibrary().ReleaseButtonL();
    SwitchControlLibrary().ReleaseButtonZL();
  }

  uint8_t rightAnalogXActual;
  if (128 + rightAnalogX * 128 == 256){
    rightAnalogXActual = 255;
  } else {
    rightAnalogXActual = (uint8_t)(128 + rightAnalogX * 128);
  }
  uint8_t rightAnalogYActual;
  if (128 + rightAnalogY * 128 == 256){
    rightAnalogYActual = 255;
  } else {
    rightAnalogYActual = (uint8_t)(128 + rightAnalogY * 128);
  }

  SwitchControlLibrary().MoveRightStick(rightAnalogXActual,rightAnalogYActual);
  if (rightX == -1) {
    SwitchControlLibrary().PressButtonX();
    SwitchControlLibrary().ReleaseButtonB();
  } else if (rightX == 1) {
    SwitchControlLibrary().PressButtonB();
    SwitchControlLibrary().ReleaseButtonX();
  } else {
    SwitchControlLibrary().ReleaseButtonX();
    SwitchControlLibrary().ReleaseButtonB();
  }
  if (rightY == -1) {
    SwitchControlLibrary().PressButtonY();
    SwitchControlLibrary().ReleaseButtonA();
  } else if (rightY == 1) {
    SwitchControlLibrary().PressButtonA();
    SwitchControlLibrary().ReleaseButtonY();
  } else {
    SwitchControlLibrary().ReleaseButtonY();
    SwitchControlLibrary().ReleaseButtonA();
  }
  if (rightZ == -1) {
    SwitchControlLibrary().PressButtonRClick();
    SwitchControlLibrary().ReleaseButtonPlus();
  } else if (rightZ == 1) {
    SwitchControlLibrary().PressButtonPlus();
    SwitchControlLibrary().ReleaseButtonRClick();
  } else {
    SwitchControlLibrary().ReleaseButtonRClick();
    SwitchControlLibrary().ReleaseButtonPlus();
  }
  if (rightTilt == -1) {
    SwitchControlLibrary().PressButtonR();
    SwitchControlLibrary().ReleaseButtonZR();
  } else if (rightTilt == 1) {
    SwitchControlLibrary().PressButtonZR();
    SwitchControlLibrary().ReleaseButtonR();
  } else {
    SwitchControlLibrary().ReleaseButtonR();
    SwitchControlLibrary().ReleaseButtonZR();
  }

  //SwitchControlLibrary().SendReport();
}

void requestEvent(){
  Wire.write(1);
}


void loop() {}