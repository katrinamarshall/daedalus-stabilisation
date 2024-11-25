#include "ICM_20948.h"

// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1

#define CALIBRATED

enum mode { GYR, ACC, MAG };

ICM_20948_I2C myICM;

mode my_mode = MAG;

float bX, bY, bZ;

//************************* Setup function
// Initialise objects
// Set full-scale range
// Set low-pass filters
// Calibration loops for 
void setup()
{

  #ifdef CALIBRATED
  bX = 1.061161 -102.978246;
  bY = 1.105093 -5.961543;
  bZ = -20.283150 + 138.576085;
  #else
  bX = 0;
  bY = 0;
  bZ = 0;
  #endif

  Serial.begin(115200);
  while (!Serial) {};
  Wire.begin();
  Wire.setClock(400000);

  bool initialized = false;
  while (!initialized)
  {
    myICM.begin(Wire, AD0_VAL);
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Initialised failed. Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }

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

  // The next few configuration functions accept a bit-mask of sensors for which the settings should be applied.

  // Set Gyro and Accelerometer to a particular sample mode
  // options: ICM_20948_Sample_Mode_Continuous
  //          ICM_20948_Sample_Mode_Cycled
  myICM.setSampleMode((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), ICM_20948_Sample_Mode_Continuous);
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("setSampleMode returned: "));
    Serial.println(myICM.statusString());
  }
  
  // Set full scale ranges for both acc and gyr
  ICM_20948_fss_t myFSS; // This uses a "Full Scale Settings" structure that can contain values for all configurable sensors

  myFSS.a = gpm8; // (ICM_20948_ACCEL_CONFIG_FS_SEL_e)
                  // gpm2
                  // gpm4
                  // gpm8
                  // gpm16

  myFSS.g = dps250; // (ICM_20948_GYRO_CONFIG_1_FS_SEL_e)
                    // dps250
                    // dps500
                    // dps1000
                    // dps2000

  myICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("setFullScale returned: "));
    Serial.println(myICM.statusString());
  }

  // Set up Digital Low-Pass Filter configuration
  ICM_20948_dlpcfg_t myDLPcfg;    // Similar to FSS, this uses a configuration structure for the desired sensors
  myDLPcfg.a = acc_d473bw_n499bw; // (ICM_20948_ACCEL_CONFIG_DLPCFG_e)
                                  // acc_d246bw_n265bw      - means 3db bandwidth is 246 hz and nyquist bandwidth is 265 hz
                                  // acc_d111bw4_n136bw
                                  // acc_d50bw4_n68bw8
                                  // acc_d23bw9_n34bw4
                                  // acc_d11bw5_n17bw
                                  // acc_d5bw7_n8bw3        - means 3 db bandwidth is 5.7 hz and nyquist bandwidth is 8.3 hz
                                  // acc_d473bw_n499bw

  myDLPcfg.g = gyr_d361bw4_n376bw5; // (ICM_20948_GYRO_CONFIG_1_DLPCFG_e)
                                    // gyr_d196bw6_n229bw8
                                    // gyr_d151bw8_n187bw6
                                    // gyr_d119bw5_n154bw3
                                    // gyr_d51bw2_n73bw3
                                    // gyr_d23bw9_n35bw9
                                    // gyr_d11bw6_n17bw8
                                    // gyr_d5bw7_n8bw9
                                    // gyr_d361bw4_n376bw5

  myICM.setDLPFcfg((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myDLPcfg);
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("setDLPcfg returned: "));
    Serial.println(myICM.statusString());
  }

  // Choose whether or not to use DLPF
  // Here we're also showing another way to access the status values, and that it is OK to supply individual sensor masks to these functions
  ICM_20948_Status_e accDLPEnableStat = myICM.enableDLPF(ICM_20948_Internal_Acc, false);
  ICM_20948_Status_e gyrDLPEnableStat = myICM.enableDLPF(ICM_20948_Internal_Gyr, false);

  // Choose whether or not to start the magnetometer
  myICM.startupMagnetometer();
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("startupMagnetometer returned: "));
    Serial.println(myICM.statusString());
  }

}

void loop()
{
  if (myICM.dataReady())
  {
    myICM.getAGMT();
    printScaled(&myICM);     
    delay(125);
  }
  else
  {
    delay(500);
  }
}

//***************** Print Scaled and Adjusted Values ******************

// Units are mg, uT and dps
void printScaled(ICM_20948_I2C *sensor)
{

  float Xoff, Yoff, Zoff;
  float Xcal, Ycal, Zcal;

  if (my_mode == ACC) 
  {
    Xoff = sensor->accX() - bX;
    Yoff = sensor->accY() - bY;
    Zoff = sensor->accZ() - bZ;
  }
  else if (my_mode == MAG) 
  {
    Xoff = sensor->magX()/0.15 - bX;
    Yoff = sensor->magY()/0.15 - bY;
    Zoff = sensor->magZ()/0.15 - bZ;
  }
  else if (my_mode == GYR)
  {
    Xoff = sensor->gyrX() - bX;
    Yoff = sensor->gyrY() - bY;
    Zoff = sensor->gyrZ() - bZ;
  }
  else
  {
    Xoff = 0;
    Yoff = 0;
    Zoff = 0;
  }

  Xcal = (1.047016 * 1.138207) *Xoff + (-0.000554 *-0.001459) *Yoff + (0.000624*0.001230) *Zoff;
  Ycal = (-0.000554 * -0.001459) *Xoff + (1.028758*1.125808) *Yoff + (-0.006440*0.001982) *Zoff;
  Zcal = (0.000624*0.001230) *Xoff + (-0.006440 *0.001982) *Yoff + (1.043026*1.124574) *Zoff;

#ifdef CALIBRATED
Serial.print(Xcal, 10); Serial.print(" "); Serial.print(Ycal, 10); Serial.print(" "); Serial.println(Zcal, 10);
#else
// Serial.print("Raw:"); Serial.print("0,0,0,0,0,0,"); Serial.print(int(Xoff)); Serial.print(","); Serial.print(int(Yoff)); Serial.print(","); Serial.println(int(Zcal));
// Serial.print("Uni:"); Serial.print("0,0,0,0,0,0,"); Serial.print(Xoff); Serial.print(","); Serial.print(Yoff); Serial.print(","); Serial.println(Zcal);
Serial.print(Xoff, 10); Serial.print(" "); Serial.print(Yoff, 10); Serial.print(" "); Serial.println(Zoff, 10);
#endif
}

