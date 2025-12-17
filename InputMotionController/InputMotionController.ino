#include <switch_ESP32.h>
#include <esp_now.h>
#include <WiFi.h>

// sets up gamepad input
NSGamepad Gamepad;

// sets up the message structure for receiving data via ESP-NOW
typedef struct messageStruct {
  int8_t id;
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t tilt;
  float analogX;
  float analogY;
} messageStruct;
messageStruct messageData;
messageStruct leftBoard;
messageStruct rightBoard;
messageStruct boards[2] = { leftBoard, rightBoard };

// callback function that will be executed when data is received
void onDataReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // breaks message into pieces to be assigned to variables
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&messageData, incomingData, sizeof(messageData));

  // convert to array
  boards[messageData.id].x = messageData.x;
  boards[messageData.id].y = messageData.y;
  boards[messageData.id].z = messageData.z;
  boards[messageData.id].tilt = messageData.tilt;
  boards[messageData.id].analogX = messageData.analogX;
  boards[messageData.id].analogY = messageData.analogY;
}

void setup() {
  // initialize serial & gamepad & USB
  Serial.begin(115200);
  Gamepad.begin();
  USB.begin();

  // initializes ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataReceived));
}

void loop() {
  // converts board data array to variables
  int8_t leftX = boards[0].x;
  int8_t leftY = boards[0].y;
  int8_t leftZ = boards[0].z;
  int8_t leftTilt = boards[0].tilt;
  float leftAnalogX = boards[0].analogX;
  float leftAnalogY = boards[0].analogY;

  int8_t rightX = boards[1].x;
  int8_t rightY = boards[1].y;
  int8_t rightZ = boards[1].z;
  int8_t rightTilt = boards[1].tilt;
  float rightAnalogX = boards[1].analogX;
  float rightAnalogY = boards[1].analogY;

  // converts -1.00 through 1.00 to 0 through 255
  uint8_t leftAnalogXActual;
  if (128 + leftAnalogX * 128 == 256) {
    leftAnalogXActual = 255;
  } else {
    leftAnalogXActual = (uint8_t)(128 + leftAnalogX * 128);
  }
  uint8_t leftAnalogYActual;
  if (128 + leftAnalogY * 128 == 256) {
    leftAnalogYActual = 255;
  } else {
    leftAnalogYActual = (uint8_t)(128 + leftAnalogY * 128);
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

  // input functions
  if (leftX == -1 && leftY == -1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_UP_LEFT);
  } else if (leftX == -1 && leftY == 0) {
    Gamepad.dPad(NSGAMEPAD_DPAD_UP);
  } else if (leftX == -1 && leftY == 1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_UP_RIGHT);
  } else if (leftX == 0 && leftY == -1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_LEFT);
  } else if (leftX == 0 && leftY == 0) {
    Gamepad.dPad(NSGAMEPAD_DPAD_CENTERED);
  } else if (leftX == 0 && leftY == 1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_RIGHT);
  } else if (leftX == 1 && leftY == -1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_DOWN_LEFT);
  } else if (leftX == 1 && leftY == 0) {
    Gamepad.dPad(NSGAMEPAD_DPAD_DOWN);
  } else if (leftX == 1 && leftY == 1) {
    Gamepad.dPad(NSGAMEPAD_DPAD_DOWN_RIGHT);
  }
  if (leftZ == -1) {
    Gamepad.press(NSButton_LeftStick);
    Gamepad.release(NSButton_Minus);
  } else if (leftZ == 1) {
    Gamepad.press(NSButton_Minus);
    Gamepad.release(NSButton_LeftStick);
  } else {
    Gamepad.release(NSButton_LeftStick);
    Gamepad.release(NSButton_Minus);
  }
  if (leftTilt == -1) {
    Gamepad.press(NSButton_LeftThrottle);
    Gamepad.release(NSButton_LeftTrigger);
  } else if (leftTilt == 1) {
    Gamepad.press(NSButton_LeftTrigger);
    Gamepad.release(NSButton_LeftThrottle);
  } else {
    Gamepad.release(NSButton_LeftTrigger);
    Gamepad.release(NSButton_LeftThrottle);
  }
  if (rightX == -1) {
    Gamepad.press(NSButton_X);
    Gamepad.release(NSButton_B);
  } else if (rightX == 1) {
    Gamepad.press(NSButton_B);
    Gamepad.release(NSButton_X);
  } else {
    Gamepad.release(NSButton_X);
    Gamepad.release(NSButton_B);
  }
  if (rightY == -1) {
    Gamepad.press(NSButton_Y);
    Gamepad.release(NSButton_A);
  } else if (rightY == 1) {
    Gamepad.press(NSButton_A);
    Gamepad.release(NSButton_Y);
  } else {
    Gamepad.release(NSButton_Y);
    Gamepad.release(NSButton_A);
  }
  if (rightZ == -1) {
    Gamepad.press(NSButton_RightStick);
    Gamepad.release(NSButton_Plus);
  } else if (rightZ == 1) {
    Gamepad.press(NSButton_Plus);
    Gamepad.release(NSButton_RightStick);
  } else {
    Gamepad.release(NSButton_RightStick);
    Gamepad.release(NSButton_Plus);
  }
  if (rightTilt == -1) {
    Gamepad.press(NSButton_RightThrottle);
    Gamepad.release(NSButton_RightThrottle);
  } else if (rightTilt == 1) {
    Gamepad.press(NSButton_RightTrigger);
    Gamepad.release(NSButton_RightThrottle);
  } else {
    Gamepad.release(NSButton_RightTrigger);
    Gamepad.release(NSButton_RightThrottle);
  }
  Gamepad.leftXAxis(leftAnalogXActual);
  Gamepad.leftYAxis(leftAnalogYActual);
  Gamepad.rightXAxis(rightAnalogXActual);
  Gamepad.rightYAxis(rightAnalogYActual);

  // sends inputs through
  Gamepad.loop();
}