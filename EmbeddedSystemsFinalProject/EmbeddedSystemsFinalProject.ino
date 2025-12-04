// -------------------- TO DO LIST --------------------
// 1. Add joystick function
// 2. Add motion functions
// 3. Figure out mutual exclusivity (what actions can be done while other actions are being done, ideally nothing is ex)
// 3. ESP-NOW
// 4. yippee yeah wahoo

#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <esp_now.h>
#include <WiFi.h>

#define BOARD_ID 1 // 0 for left board, 1 for right board

#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

uint8_t receiverAddress[] = { 0x48, 0xCA, 0x43, 0x2E, 0xCA, 0xEC };
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
esp_now_peer_info_t peerInfo;

float jOffset;
float iRot, jRot, kRot;
float xAccel, yAccel, zAccel;
int calibrateCycle;
int8_t currentXPosition, currentYPosition, currentZPosition, currentTiltPosition;
float currentAnalogXPosition, currentAnalogYPosition;
long timeSinceXMovement, timeSinceYMovement, timeSinceZMovement;
int xMovementCount, yMovementCount, zMovementCount;
#define NEUTRAL 0
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define FORWARD -1
#define BACKWARD 1
#define TILT_LEFT -1
#define TILT_RIGHT 1

#define ANALOG_DEADZONE 0.2

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(esp_now_send_cb_t(onDataSent));

  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Adafruit BNO08x test!");

  // Try to initialize!
  if (!bno08x.begin_I2C()) {
    //if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte UART buffer!
    //if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x Found!");

  setReports();

  Serial.println("Reading events");
  delay(100);
}

void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    Serial.println("Could not enable game vector");
  }
  if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION)) {
    Serial.println("Could not enable linear acceleration");
  }
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    digitalWrite(LED_RED, LOW);
  } else {
    digitalWrite(LED_RED, HIGH);
  }
}

void loop() {
  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
    jOffset = 0;
    calibrateCycle = 0;
    // digitalWrite(LED_GREEN,LOW);
    // delay(1000);
    // digitalWrite(LED_GREEN,HIGH);
    calibrate();
  }
  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  if (calibrateCycle > 100) {
    switch (sensorValue.sensorId) {
      case SH2_LINEAR_ACCELERATION:
        xAccel = sensorValue.un.linearAcceleration.x;
        yAccel = sensorValue.un.linearAcceleration.y;
        zAccel = sensorValue.un.linearAcceleration.z;

        if (xAccel > 4) {
          xMovementCount++;
        } else if (xAccel < -4) {
          xMovementCount--;
        } else if (xMovementCount != 0) {
          if (xMovementCount >= 7) {
            long now = millis();
            if (now - timeSinceXMovement < 350) {
              if (currentXPosition != DOWN) {
                currentXPosition++;
              }
            }
            timeSinceXMovement = now;
          }
          if (xMovementCount <= -7) {
            long now = millis();
            if (now - timeSinceXMovement < 350) {
              if (currentXPosition != UP) {
                currentXPosition--;
              }
            }
            timeSinceXMovement = now;
          }
          xMovementCount = 0;
        }
        if (yAccel > 4) {
          yMovementCount++;
        } else if (yAccel < -4) {
          yMovementCount--;
        } else if (yMovementCount != 0) {
          if (yMovementCount >= 7) {
            long now = millis();
            if (now - timeSinceYMovement < 350) {
              if (currentYPosition != RIGHT) {
                currentYPosition++;
              }
            }
            timeSinceYMovement = now;
          }
          if (yMovementCount <= -7) {
            long now = millis();
            if (now - timeSinceYMovement < 350) {
              if (currentYPosition != LEFT) {
                currentYPosition--;
              }
            }
            timeSinceYMovement = now;
          }
          yMovementCount = 0;
        }
        if (zAccel > 4) {
          zMovementCount++;
        } else if (zAccel < -4) {
          zMovementCount--;
        } else if (zMovementCount != 0) {
          if (zMovementCount >= 7) {
            long now = millis();
            if (now - timeSinceZMovement < 350) {
              if (currentZPosition != BACKWARD) {
                currentZPosition++;
              }
            }
            timeSinceZMovement = now;
          }
          if (zMovementCount <= -7) {
            long now = millis();
            if (now - timeSinceZMovement < 350) {
              if (currentZPosition != FORWARD) {
                currentZPosition--;
              }
            }
            timeSinceZMovement = now;
          }
          zMovementCount = 0;
        }

        

        break;
      case SH2_GAME_ROTATION_VECTOR:
        iRot = sensorValue.un.gameRotationVector.i;
        jRot = sensorValue.un.gameRotationVector.j - jOffset;
        kRot = sensorValue.un.gameRotationVector.k;

        if (abs(iRot) < 0.05 && abs(jRot) < 0.05 && abs(kRot) < 0.05) {
          digitalWrite(LED_BUILTIN, HIGH);
        } else {
          digitalWrite(LED_BUILTIN, LOW);
        }

        if (iRot > 0.15 && kRot < -0.15 && abs(jRot) < 0.15) {
          currentTiltPosition = TILT_RIGHT;
        } else if (iRot < -0.15 && kRot > 0.15 && abs(jRot) < 0.15) {
          currentTiltPosition = TILT_LEFT;
        } else {
          currentTiltPosition = NEUTRAL;
        }
        if (iRot > 0.15 && kRot > -0.05) {
          if (iRot > 0.3) {
            currentAnalogXPosition = 1;
          } else {
            currentAnalogXPosition = ANALOG_DEADZONE + (iRot - 0.15) * (1 - ANALOG_DEADZONE) / (0.3 - 0.15);
          }
        } else if (iRot < -0.15 && kRot < 0.05) {
          if (iRot < -0.3) {
            currentAnalogXPosition = -1;
          } else {
            currentAnalogXPosition = -ANALOG_DEADZONE + (iRot + 0.15) * (ANALOG_DEADZONE - 1) / (0.15 - 0.3);
          }
        } else {
          currentAnalogXPosition = 0;
        }
        if (jRot > 0.15) {
          if (jRot > 0.4) {
            currentAnalogYPosition = -1;
          } else {
            currentAnalogYPosition = -ANALOG_DEADZONE + (jRot - 0.15) * (ANALOG_DEADZONE - 1) / (0.4-0.15);
          }
        } else if (jRot < -0.1) {
          if (jRot < -0.17) {
            currentAnalogYPosition = 1;
          } else {
            currentAnalogYPosition = ANALOG_DEADZONE + (jRot + 0.1) * (1 - ANALOG_DEADZONE) / (0.1-0.17);
          }
        } else {
          currentAnalogYPosition = 0;
        }

        break;
    }

    messageData.id = BOARD_ID;
    messageData.x = currentXPosition;
    messageData.y = currentYPosition;
    messageData.z = currentZPosition;
    messageData.tilt = currentTiltPosition;
    messageData.analogX = currentAnalogXPosition;
    messageData.analogY = currentAnalogYPosition;

    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&messageData, sizeof(messageData));

    if (result == ESP_OK) {
      digitalWrite(LED_RED, HIGH);
    } else {
      digitalWrite(LED_RED, LOW);
    }

  } else {
    calibrate();
    if (calibrateCycle > 100) {
      jOffset /= calibrateCycle;
      digitalWrite(LED_BLUE, HIGH);
    }
  }
}

void calibrate() {
  currentXPosition = 0;
  currentYPosition = 0;
  currentZPosition = 0;
  currentTiltPosition = 0;
  currentAnalogXPosition = 0;
  currentAnalogYPosition = 0;
  if (!bno08x.getSensorEvent(&sensorValue)) {
    calibrate();
    return;
  }
  calibrateCycle++;
  switch (sensorValue.sensorId) {
    case SH2_GAME_ROTATION_VECTOR:
      digitalWrite(LED_BLUE, LOW);
      //iOffset += sensorValue.un.gameRotationVector.i;
      jOffset += sensorValue.un.gameRotationVector.j;
      //kOffset += sensorValue.un.gameRotationVector.k;
      break;

      // current calibration only allows for play while controller is stricty vertical (intended use case), fix when rest is finished
  }
}
