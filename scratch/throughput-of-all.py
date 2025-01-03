import os
import matplotlib.pyplot as plt
import csv
import numpy as np

# Specify the directory path where the CSV files are located
csv_directory = '/home/amin/ns-3-dev-for-master-project/'

# Change the current working directory
os.chdir(csv_directory)

# Define the distance range and step size
distance_start = 20.0
distance_end = 400.0
distance_step = 10.0

# Define the series names
series_names = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'k']
num_series = len(series_names)

# Define the rate control algorithms
rate_control_algorithms = [ 'Minstrel', 'Ideal', 'Thompson']

# Initialize arrays to store average throughput data for each algorithm
average_throughput_data = np.zeros((len(rate_control_algorithms), int((distance_end - distance_start) / distance_step) + 1))

# Loop over the rate control algorithms
for algorithm_index, algorithm_name in enumerate(rate_control_algorithms):
    # Initialize a NumPy array to store data for the current algorithm
    algorithm_data = np.zeros((num_series, int((distance_end - distance_start) / distance_step) + 1))

    # Loop over the distance range with the specified step size
    for distance_index, distance in enumerate(np.arange(distance_start, distance_end + distance_step, distance_step)):
        # Loop over each series
        for series_index, series_name in enumerate(series_names):
            # Construct the correct path to the CSV file using the current distance value, series name, and algorithm name
            csv_file_name = f'throughput_vs_distance_sta2_{algorithm_name}-{series_name}{distance:.6f}.csv'
            csv_file_path = os.path.join(csv_directory, csv_file_name)

            # Read data from the CSV file
            throughput_values = []

            with open(csv_file_path, 'r') as file:
                reader = csv.reader(file)
                for row in reader:
                    throughput_values.append(float(row[1]))

            # Store the data for each series
            algorithm_data[series_index, distance_index] = np.mean(throughput_values)

    # Compute the average throughput for the current algorithm
    average_throughput_data[algorithm_index] = np.mean(algorithm_data, axis=0)

# Define a dictionary to map algorithm names to desired labels
algorithm_labels = {
    
    'Minstrel': 'Minstrel-Ht',
    'Ideal': 'Idealwifimanger',
    'Thompson': 'Thompson Sampling'
}

# Plot individual lines for each rate control algorithm with dotted lines and visible markers
for algorithm_index, algorithm_name in enumerate(rate_control_algorithms):
    plt.plot(np.arange(distance_start, distance_end + distance_step, distance_step), 
             average_throughput_data[algorithm_index], 
             label=algorithm_labels[algorithm_name], linestyle='dotted', marker='o')

# Add labels and legend to the plot
plt.title(' Throughput vs Distance for Station 2 (Four Rate Control Algorithms)', fontsize=20)
plt.xlabel('Distance (m)', fontsize=16)
plt.ylabel(' Throughput (Mbps)', fontsize=16)
plt.legend()
plt.grid(True)

# Save the plot as an image file (optional)
plt.savefig('average_throughput_vs_distance_sta2_combined.png')

# Display the graph
plt.show()