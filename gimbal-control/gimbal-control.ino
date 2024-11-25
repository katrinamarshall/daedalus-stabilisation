#include <SimpleFOC.h>
#include <ICM_20948.h>

// The value of the last bit of the I2C address for the IMU
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1

ICM_20948_I2C myICM;
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(29, 28, 8,7, 6,9);

float target_velocity;
float desired_yaw = 0; 
float declination = 0;

float bX, bY, bZ;

void setup() {

  // Magnetometer Calibration Zero Offsets
  bX = 1.061161 -102.978246;
  bY = 1.105093 -5.961543;
  bZ = -20.283150 + 138.576085;

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

  // Here we are doing a SW reset to make sure the device starts in a known state
  myICM.swReset();
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("Software Reset returned: "));
    Serial.println(myICM.statusString());
  }
  delay(250);

  // Now wake the sensor up
  myICM.sleep(false);
  myICM.lowPower(false);

  myICM.startupMagnetometer();
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("startupMagnetometer returned: "));
    Serial.println(myICM.statusString());
  }

  // ******* SETUP MOTOR
  driver.voltage_power_supply = 7.4;
  driver.voltage_limit = 7.4;
  driver.init();

  motor.linkDriver(&driver);
  motor.voltage_limit = 3;

  motor.controller = MotionControlType::velocity_openloop;

  // init motor hardware
  motor.init();
  motor.target=0.0;

  motor.initFOC();

  Serial.println("Motor ready!");
  target_velocity = 0; 
  _delay(1000);
}

float error_accum = 0;
float prev_error = 0;
float Xoff, Yoff, Zoff, Xcal, Ycal, Zcal; 

void loop() {

  motor.loopFOC();
  myICM.getAGMT();

  Xoff = myICM.magX()/0.15 - bX;
  Yoff = myICM.magY()/0.15 - bY;
  Zoff = myICM.magZ()/0.15 - bZ;

  Xcal = (1.047016 * 1.138207) *Xoff + (-0.000554 *-0.001459) *Yoff + (0.000624*0.001230) *Zoff;
  Ycal = (-0.000554 * -0.001459) *Xoff + (1.028758*1.125808) *Yoff + (-0.006440*0.001982) *Zoff;
  Zcal = (0.000624*0.001230) *Xoff + (-0.006440 *0.001982) *Yoff + (1.043026*1.124574) *Zoff;

  // yaw (z-axis rotation)
  double yaw = atan2(Xcal, Ycal) * 180.0 / PI - declination;
    
  Serial.print(F(" Yaw:"));
  Serial.println(yaw, 1);

  float error = (desired_yaw - yaw);
  Serial.print(F("Error:"));
  Serial.println(error, 2);

  // Check the direction of rotation, might need to swap signs for velocity depending on the mounting of the IMU
  if (error > 20)
    target_velocity = 1;
  else if (error < -20)
    target_velocity = -1;
  else
    target_velocity = 0;

  Serial.print(F("Target velocity:"));
  Serial.println(target_velocity, 2);

  if (myICM.status != ICM_20948_Stat_FIFOMoreDataAvail) // If more data is available then we should read it right away - and not delay
  {
    delay(10);
  }

  motor.move(target_velocity);

}
