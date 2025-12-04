#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct messageStruct {
  int8_t id;
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t tilt;
  float analogX;
  float analogY;
} messageStruct;

// Create a messageStruct called messageData
messageStruct messageData;

// Create a structure to hold the readings from each board
messageStruct leftBoard;
messageStruct rightBoard;

// Create an array with all the structures
messageStruct boards[2] = {leftBoard, rightBoard};

// callback function that will be executed when data is received
void onDataReceived(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  // Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.println(macStr);
  memcpy(&messageData, incomingData, sizeof(messageData));
  // Serial.printf("Board ID %u: %u bytes\n", messageData.id, len);
  // Update the structures with the new incoming data
  boards[messageData.id].x = messageData.x;
  boards[messageData.id].y = messageData.y;
  boards[messageData.id].z = messageData.z;
  boards[messageData.id].tilt = messageData.tilt;
  boards[messageData.id].analogX = messageData.analogX;
  boards[messageData.id].analogY = messageData.analogY;
  // Serial.printf("x value: %d \n", boards[messageData.id].x);
  // Serial.printf("y value: %d \n", boards[messageData.id].y);
  // Serial.printf("z value: %d \n", boards[messageData.id].z);
  // Serial.printf("tilt value: %d \n", boards[messageData.id].tilt);
  // Serial.print("analogx value: ");
  // Serial.println(boards[messageData.id].analogX);
  // Serial.print("analogy value: ");
  // Serial.println(boards[messageData.id].analogY);
  

  
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);

  Wire.begin();
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataReceived));
}
 
void loop() {
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
  // Acess the variables for each board
  Wire.beginTransmission(9);
  //Wire.write(0);
  byte inputData[24];
  inputData[0] = leftX;
  inputData[1] = leftY;
  inputData[2] = leftZ;
  inputData[3] = leftTilt;
  unsigned char c[4];
  memcpy(c, &leftAnalogX, 4);
  for (int i = 0; i < 4; i++) {
    inputData[i + 4] = c[i];
  }
  memcpy(c, &leftAnalogY, 4);
  for (int i = 0; i < 4; i++) {
    inputData[i + 8] = c[i];
  }
  inputData[12] = rightX;
  inputData[13] = rightY;
  inputData[14] = rightZ;
  inputData[15] = rightTilt;
  c[4];
  memcpy(c, &rightAnalogX, 4);
  for (int i = 0; i < 4; i++) {
    inputData[i + 16] = c[i];
  }
  memcpy(c, &rightAnalogY, 4);
  for (int i = 0; i < 4; i++) {
    inputData[i + 20] = c[i];
  }
  Wire.write(inputData, sizeof(inputData));
  Wire.endTransmission();

   leftX = (int8_t)inputData[0];
   leftY = (int8_t)inputData[1];
   leftZ = (int8_t)inputData[2];
   leftTilt = (int8_t)inputData[3];
   leftAnalogX;
  unsigned char cLX[4] = { inputData[4], inputData[5], inputData[6], inputData[7] };
  memcpy(&leftAnalogX, &cLX, sizeof(leftAnalogX));
   leftAnalogY;
  unsigned char cLY[4] = { inputData[8], inputData[9], inputData[10], inputData[11] };
  memcpy(&leftAnalogY, &cLY, sizeof(leftAnalogX));

   rightX = (int8_t)inputData[12];
   rightY = (int8_t)inputData[13];
   rightZ = (int8_t)inputData[14];
   rightTilt = (int8_t)inputData[15];
   rightAnalogX;
  unsigned char cRX[4] = { inputData[16], inputData[17], inputData[18], inputData[19] };
  memcpy(&rightAnalogX, &cRX, sizeof(rightAnalogX));
   rightAnalogY;
  unsigned char cRY[4] = { inputData[20], inputData[21], inputData[22], inputData[23] };
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

  // Serial.print(leftX);
  // Serial.print(" ");
  // Serial.print(leftY);
  // Serial.print(" ");
  // Serial.print(leftZ);
  // Serial.print(" ");
  // Serial.print(leftTilt);
  // Serial.print(" ");
  // Serial.print((uint8_t)(128+leftAnalogX*128));
  // Serial.print(" ");
  // Serial.print((uint8_t)(128+leftAnalogY*128));
  // Serial.print(" ");

  // Serial.print(rightX);
  // Serial.print(" ");
  // Serial.print(rightY);
  // Serial.print(" ");
  // Serial.print(rightZ);
  // Serial.print(" ");
  // Serial.print(rightTilt);
  // Serial.print(" ");
  // Serial.print((uint8_t)(128+rightAnalogX*128));
  // Serial.print(" ");
  // Serial.print((uint8_t)(128+rightAnalogY*128));

  Wire.requestFrom(9,1);
  while (Wire.available()){
    Wire.read();
  }
  
}