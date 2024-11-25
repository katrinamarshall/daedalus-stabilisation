/*
This sketch implements a reaction wheel which is designed to minimise rapid changes in the yaw angle of Daedalus

It uses the DMP functions from the ICM20948 chip which fuse the accelerometer and gyroscope data into a Quaternion without any on-board processing required.
Make sure

*/

#include <ICM_20948.h>

// The value of the last bit of the I2C address for the IMU
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1
#define PWMPin 36
#define directionPin 33

ICM_20948_I2C myICM;

void setup() {

  // use monitoring with serial 
  Serial.begin(115200);

  // ******* SETUP IMU
  Wire.begin();
  Wire.setClock(400000);

  myICM.begin(Wire, AD0_VAL);
  Serial.print(F("Initialization of the sensor returned: "));
  Serial.println(myICM.statusString());

  while(myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.println(F("Trying again..."));
    delay(500);
  }

  Serial.println(F("Device connected!"));

  bool success = true;
  success &= (myICM.initializeDMP() == ICM_20948_Stat_Ok);
  success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_GAME_ROTATION_VECTOR) == ICM_20948_Stat_Ok);// INV_ICM20948_SENSOR_ROTATION_VECTOR) == ICM_20948_Stat_Ok);
  success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Quat6, 0) == ICM_20948_Stat_Ok);
  success &= (myICM.enableFIFO() == ICM_20948_Stat_Ok);
  success &= (myICM.enableDMP() == ICM_20948_Stat_Ok);
  success &= (myICM.resetDMP() == ICM_20948_Stat_Ok);
  success &= (myICM.resetFIFO() == ICM_20948_Stat_Ok);

  if (success)
  {
    Serial.println(F("DMP enabled!"));
  }
  else
  {
    Serial.println(F("Enable DMP failed!"));
    Serial.println(F("Please check that you have uncommented line 29 (#define ICM_20948_USE_DMP) in ICM_20948_C.h..."));
    while (1)
      ; // Do nothing more
  }

  // ******* SETUP MOTOR
  pinMode(PWMPin, OUTPUT);
  pinMode(directionPin, OUTPUT);

}

float target_velocity = 0;
float prev_yaw = 0;
float Kp = 0.1;

void loop() {
  
  // Convert 6DOF vector to Euler angles
  icm_20948_DMP_data_t data;
  myICM.readDMPdataFromFIFO(&data);
  if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)) // Was valid data available?
  {
    if ((data.header & DMP_header_bitmap_Quat6) > 0)
    {
      // Scale to +/- 1
      double q1 = ((double)data.Quat6.Data.Q1) / 1073741824.0; // Convert to double. Divide by 2^30
      double q2 = ((double)data.Quat6.Data.Q2) / 1073741824.0; // Convert to double. Divide by 2^30
      double q3 = ((double)data.Quat6.Data.Q3) / 1073741824.0; // Convert to double. Divide by 2^30

      double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

      double qw = q0;
      double qx = q2;
      double qy = q1;
      double qz = -q3;

      // yaw (z-axis rotation)
      double t3 = +2.0 * (qw * qz + qx * qy);
      double t4 = +1.0 - 2.0 * (qy * qy + qz * qz);
      double yaw = atan2(t3, t4) * 180.0 / PI;
      
      Serial.print(F(" Yaw:"));
      Serial.println(yaw, 1);

      float delta_yaw = yaw - prev_yaw;
      prev_yaw = yaw;
      Serial.print(F("Yaw derivative:"));
      Serial.println(delta_yaw, 2);

      if (abs(delta_yaw) > 1)
        target_velocity = target_velocity + Kp*delta_yaw; // Check orientation of IMU to motor, might need to swap signs

      Serial.print(F("Target velocity:"));
      Serial.println(target_velocity, 2);

    }
  }

  if (myICM.status != ICM_20948_Stat_FIFOMoreDataAvail) // If more data is available then we should read it right away - and not delay
  {
    delay(10);
  }

  if (target_velocity > 0) 
  {
    digitalWrite(directionPin, 1);
    analogWrite(PWMPin, target_velocity);
  }
  else if (target_velocity < 0)
  {
    digitalWrite(directionPin, 0);
    analogWrite(PWMPin, -target_velocity);
  }
  else
  {
    analogWrite(PWMPin, 0);
  }

}