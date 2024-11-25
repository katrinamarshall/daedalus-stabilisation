#include <SimpleFOC.h>
#include <ICM_20948.h>

// The value of the last bit of the I2C address for the IMU
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1

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
  
}

float error_accum = 0;
float prev_error = 0;

void loop() {

  motor.loopFOC();
  
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

      Serial.print(F("Quat:"));
      Serial.print(q1, 2);
      Serial.print(", ");
      Serial.print(q2, 2);
      Serial.print(", ");
      Serial.print(q3, 2);
      Serial.print(", ");
      Serial.println(q1, 2);

      double qw = q0; // See issue #145 - thank you @Gord1
      double qx = q2;
      double qy = q1;
      double qz = -q3;

      // yaw (z-axis rotation)
      double t3 = +2.0 * (qw * qz + qx * qy);
      double t4 = +1.0 - 2.0 * (qy * qy + qz * qz);
      double yaw = atan2(t3, t4) * 180.0 / PI;
      
      Serial.print(F(" Yaw:"));
      Serial.println(yaw, 1);

      float error = (desired_yaw - yaw);
      Serial.print(F("Error:"));
      Serial.println(error, 2);

      float error_rate = error - prev_error;
      prev_error = error;
      Serial.print(F("Error derivative:"));
      Serial.println(error_rate, 2);

      target_velocity = -(Kp*error + Kd*error_rate); 
      Serial.print(F("Target velocity:"));
      Serial.println(target_velocity, 2);

    }
  }

  if (myICM.status != ICM_20948_Stat_FIFOMoreDataAvail) // If more data is available then we should read it right away - and not delay
  {
    delay(10);
  }

  motor.move(target_velocity);

}