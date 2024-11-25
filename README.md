## TODO
- Verify the DC motor works correctly (can use the test script in the `reaction-wheel-control` folder)
    - Verify the `reaction-wheel-control` script works correctly
    - Tune the controller gain (might need to implement some low-pass filtering for the IMU data, it looks a little bit noisy)
- Design the reaction wheel disk
- Design the mount for the gimbal IMU
- Add limit switches to the gimbal motor

## Requirements
From the Arduino Library Manager:
- ICM2048 Sparkfun Library
- SimpleFOC library

## IMU

### IMU Calibration
To manually calibrate the magnetometer and accelorometer, the same process can be followed.

1. Upload the imu_calibration script to an Arduino (or equivalent), make sure the calibrated flag is commented out and set which sensor you want to calibrate

        // #def CALIBRATED
        
        mode my_mode = ACC; // Or GYR or MAG

2. Open a terminal and run the python script `serial-2-csv.py`. Make sure you change the `serial_port = 'COM5'` to the correct COM port for the Arduino. Note that the output file name is hardcoded and will overwrite every time the script is called.

3. Slowly rotate the IMU in all directions, especially when calibrating the accelerometer it is important that the motion is slow, ideally the chip is stationary at each reading. `ctrl-C` to end the script.

4. Run the `imu-calibration-plots.py` script to verify the data is reasonable. Check the file names are correct. This also calculates an average to be input into Magneto.

5. Open [Magneto](https://sailboatinstruments.blogspot.com/2011/09/improved-magnetometer-calibration-part.html) and upload the file with the uncalibrated data. 

    The norm should be whatever the average was found to be in the previous script. 

    Press calibrate to generate the new calibration coefficients and offsets

6. Change the `bX`, `bY` and `bZ` values in the Arduino script and the multiplication factors in the `printScaled` function. Note the calibration equation on Magneto to check which factors should be swapped.

7. To verify, complete this process again, now uncomment the `#define CALIBRATED` to verify the offsets.

### Additional Magnetometer Plotting
To verify the magnetometer has been calibrated correctly for soft and hard iron distortions, it can also be useful to see the plots of the xyz data on a spatial rather than temporal plot. This can be done with the `mag-calibration-plotting.py` script.
Before calibration the three circles are likely elliptical (soft iron distortions) and have a zero error (hard iron distortion). After calibration all three should be of the same radius, aligned at zero and circular.

### IMU pinout
- Blue: SDA (A4 on Nano, 18 on Teensy 4.1)
- Yellow: SCL (A5 on Nano, 19 on Teensy 4.1)

## Brushless DC (Gimbal) Motor Control

Motor driver: https://core-electronics.com.au/sparkfun-brushless-motor-driver-3-phase-tmc6300.html

Motor: https://core-electronics.com.au/three-phase-brushless-gimbal-stabilizer-motor.html

### Motor Pinout

Each high-low pair of the 6pwm has to be A and B channels of the same FlexTimer and to the same submodule. Channel A is for High side and Channel B is for Low side
- UL, UH --> 29, 28
- VL, VH --> 8, 7
- WL, WH --> 6, 9

Pole pair number for Brushless Gimbal motor is 7

### Gimbal Control
`gimbal-control` implements the control of this motor with the IMU. Currently it only uses the magnetometer data (not using DMP functions) from the IMU to find the error between true North and the current orientation of the motor/IMU. 

To use the script, magnetometer calibration needs to be done with the full set up running to get the correct calibration parameters and valid data. The process is outlined above and the new calibration parameters need to be manually copied into the `gimbal-control` script. The declination for your current location also needs to be set to correctly convert the magnetometer data into yaw: https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#declination

Some simplifications are made here, just noting them down in case they potentially cause some issues:
- Yaw conversion assumes the IMU is perfectly flat in the xy plane 
- The motor is assumed to have enough torque to accelerate its load to a velocity of +-1 from 0 with no ramping, if the motor is loaded ramping might need to be implemented

## Brushed DC (reaction wheel) Motor Control

Motor: https://core-electronics.com.au/12v-dc-motor-350rpm-w-encoder-12kg-cm.html

Driver: https://core-electronics.com.au/makerverse-motor-driver-2-channel.html

### Motor Pinout
Control requires PWM and a digital I/O pin. At the moment these are defined as:
- PWM --> 36
- DIR --> 33

### Reaction Wheel Control
The reaction wheel control no longer tries to actively rotate Daedalus to a target yaw angle, instead it tries to dampen any rapid rotation through proportional control with change in yaw.

This has not been tested with a motor, only with LEDs to show the PWM and DIR states. 