'''
Helper script to read data from Serial and save to a txt file
- Configuration block below requires the COM port, Baud rate and output file name to be set
'''

import serial
import csv
import time

# Configuration
serial_port = 'COM8'  # Change this to your Arduino's COM port
baud_rate = 115200       # Make sure this matches the Arduino's baud rate
output_file = 'mag_calibrated2.txt'  # Output text file name

def read_serial_data():
    try:
        # Open the serial port
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            print(f"Reading data from {serial_port} at {baud_rate} baud...")
            time.sleep(2)  # Wait for the connection to be established
            
            with open(output_file, mode='w', newline='') as csvfile:
                csv_writer = csv.writer(csvfile)
                
                while True:
                    # Read a line from the serial port
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        # Write data to CSV
                        csv_writer.writerow([line])
                        print(f"Logged: {line}")
                        
                    # Add a small delay to prevent overwhelming the output
                    time.sleep(0.1)

    except KeyboardInterrupt:
        print("Exiting...")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    read_serial_data()
