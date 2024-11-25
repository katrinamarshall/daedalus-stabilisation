'''
Script to plot magnetometer x y z data from a txt file

Written to work with the imu_calibration sketch after converting this stream to a txt file using serial-2-txt.py

The plot of the magnetometer data will show if the calibration parameters in the imu_calibration script are accurate
When calibrated the three plots should appear as circles aligned at zero with equal radius:

https://thecavepearlproject.org/2015/05/22/calibrating-any-compass-or-accelerometer-for-arduino/

'''

import pandas as pd
import matplotlib.pyplot as plt
from math import sqrt

# Function to read the data from a space-delimited text file
def read_data(file_path):
    # Read the data using pandas
    data = pd.read_csv(file_path, delimiter=' ', header=None, names=['X', 'Y', 'Z'])
    return data

# Function to calculate and print the average vector magnitude
def calculate_averages(data):
    norms = [sqrt(row['X']**2 + row['Y']**2 + row['Z']**2) for _, row in data.iterrows()]
    avg = sum(norms) / len(norms)
    print(f"Average magnitude: {avg}")

# Function to plot all 2D projections (XY, XZ, YZ) on the same figure with equal scaling
def plot_projections(data):
    plt.figure(figsize=(10, 8))

    # Plot XY projection
    plt.scatter(data['X'], data['Y'], color='purple', s=10, label='XY Projection')
    
    # Plot XZ projection
    plt.scatter(data['X'], data['Z'], color='orange', s=10, label='XZ Projection')
    
    # Plot YZ projection
    plt.scatter(data['Y'], data['Z'], color='cyan', s=10, label='YZ Projection')

    # Set equal scaling for the axes
    plt.axis('equal')
    plt.xlabel('X and Y Axes')
    plt.ylabel('Y and Z Axes')
    plt.title('Mag data 2D projections')
    plt.legend()
    plt.grid()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # Replace 'mag_calibrated.txt' with the path to your text file
    file_path = 'mag_calibrated2.txt'
    
    # Read the data
    data = read_data(file_path)
    
    # Calculate and print averages
    calculate_averages(data)
    
    # Plot all 2D projections on the same figure with equal scaling
    plot_projections(data)
