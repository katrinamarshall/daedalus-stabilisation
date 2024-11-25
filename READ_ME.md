## Requirements
```

```

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
- Blue: SDA (A4 on Nano, )
- Yellow: SCL (A5 on Nano, )

## Motor Control

### Pinout

Each high-low pair of the 6pwm has to be A and B channels of the same FlexTimer and to the same submodule. Channel A is for High side and Channel B is for Low side
- UL, UH --> 3, 2 (FlexPWM4, Submodule 2)
- VL, VH --> 8, 7 (FlexPWM1, Submodule 3)
- WL, WH --> 6, 9 (FlexPWM2, Sumbolude 2)

Pole pair number for Brushless Gimbal motor is 7