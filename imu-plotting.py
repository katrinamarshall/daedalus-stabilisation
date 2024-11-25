import pandas as pd
import matplotlib.pyplot as plt
from math import sqrt

# Function to read the data from a space-delimited text file
def read_data(file_path):
    # Read the data using pandas
    data = pd.read_csv(file_path, delimiter=' ', header=None, names=['X', 'Y', 'Z'])
    return data

# Function to calculate and print the averages
def calculate_averages(data):
    norms = []
    for index, row in data.iterrows():
        norms.append(sqrt( float(row['X'])**2 + float(row['Y'])**2 + float(row['Z'])**2 ))

    avg = sum(norms) / len(norms)
    
    print(f"Average: {avg}")

# Function to plot the data
def plot_data(data, data_type):
    fig, ax = plt.subplots(figsize=(10, 6))

    # Plot the data based on the type
    ax.plot(data.index, data['X'], label=f'{data_type} X', color='r')
    ax.plot(data.index, data['Y'], label=f'{data_type} Y', color='g')
    ax.plot(data.index, data['Z'], label=f'{data_type} Z', color='b')
    
    ax.set_xlabel('Sample Index')
    ax.set_ylabel(f'{data_type}')
    ax.legend()
    ax.grid()

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # Replace 'acc_uncalibrated.txt' with the path to your text file
    file_path = 'mag_calibrated.txt'
    
    # Choose the label for the data type
    data_type = 'Magnetic Field (uT)'
                # 'Acceleration (mg)' 
                # 'Rotation (dps)' 
                # 'Magnetic Field (uT)' 

    # Read the data
    data = read_data(file_path)
    
    # Calculate and print averages
    calculate_averages(data)
    
    # Plot the data
    plot_data(data, data_type)
