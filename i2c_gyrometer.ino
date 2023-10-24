#include <Wire.h>
const int sda = 21;
const int scl = 22;
int address = 0x68;
//define int for Gyrometer
byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;
byte GYRO_YOUT_H = 0;
byte GYRO_YOUT_L = 0;
byte GYRO_ZOUT_H = 0;
byte GYRO_ZOUT_L = 0;
int16_t GYRO_X_RAW = 0;
int16_t GYRO_Y_RAW = 0;
int16_t GYRO_Z_RAW = 0;
int16_t gyroX_Per_S = 0;
int16_t gyroY_Per_S = 0;
int16_t gyroZ_Per_S = 0;
float elapsedTime = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;
float oldGyroAngleX = 0.0;
float newGyroAngleX = 0.0;
float oldGyroAngleY = 0.0;
float newGyroAngleY = 0.0;
float oldGyroAngleZ = 0.0;
float newGyroAngleZ = 0.0;
//calibration
float calibrationOffsetX = 0.0;
float calibrationOffsetY = 0.0;
float calibrationOffsetZ = 0.0;


void setup(){
    Wire.begin(sda,scl); //to call the SDA and SCL
    Serial.begin(115200);
    Serial.println("Calibration Start!");
    for (int i = 0; i < 100; i ++) { //calibrate
        readGyrometer();
        calibrationOffsetX += gyroX_Per_S;
        calibrationOffsetY += gyroY_Per_S;
        calibrationOffsetZ += gyroZ_Per_S;
        delay(50);
    }
    // Calculate the average calibration offsets
    calibrationOffsetX /= 100.0;
    calibrationOffsetY /= 100.0;
    calibrationOffsetZ /= 100.0;

    Serial.println("Calibration End");
    Serial.println(calibrationOffsetX);
    Serial.println(calibrationOffsetY);
    Serial.println(calibrationOffsetZ);
    
    oldGyroAngleX = 0.0;
    oldGyroAngleY = 0.0;
    oldGyroAngleZ = 0.0;

    }

void loop(){
  readGyrometer();
  printAngle();
  delay(100);

}

void readGyrometer(){
  Wire.beginTransmission(address);
  Wire.write(0x43);
  Wire.endTransmission();
  // read answer to request from 43
  Wire.requestFrom(address, 6);
  GYRO_XOUT_H = Wire.read();
  GYRO_XOUT_L = Wire.read();
  GYRO_YOUT_H = Wire.read();
  GYRO_YOUT_L = Wire.read();
  GYRO_ZOUT_H = Wire.read();
  GYRO_ZOUT_L = Wire.read();

  GYRO_X_RAW = GYRO_XOUT_H << 8 | GYRO_XOUT_L;
  GYRO_Y_RAW = GYRO_YOUT_H << 8 | GYRO_YOUT_L;
  GYRO_Z_RAW = GYRO_ZOUT_H << 8 | GYRO_ZOUT_L;

  gyroX_Per_S = GYRO_X_RAW/131.0; 
  gyroY_Per_S = GYRO_Y_RAW/131.0;
  gyroZ_Per_S = GYRO_Z_RAW/131.0;

  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;
  previousTime = currentTime;

  newGyroAngleX = oldGyroAngleX + (gyroX_Per_S - calibrationOffsetX) * elapsedTime;
  newGyroAngleY = oldGyroAngleY + (gyroY_Per_S - calibrationOffsetY) * elapsedTime;
  newGyroAngleZ = oldGyroAngleZ + (gyroZ_Per_S - calibrationOffsetZ) * elapsedTime;

  oldGyroAngleX = newGyroAngleX;
  oldGyroAngleY = newGyroAngleY;
  oldGyroAngleZ = newGyroAngleZ;

}

void printAngle(){
  Serial.print("Angle X-axis:");
  Serial.print(newGyroAngleX);
  Serial.print(",");
  Serial.print ("Angle Y-axis:");
  Serial.print(newGyroAngleY);
  Serial.print(",");
  Serial.print("Angle Z-axis:");
  Serial.println(newGyroAngleZ);
}