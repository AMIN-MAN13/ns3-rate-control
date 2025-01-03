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
num_series = 10

# Define the rate control algorithms
rate_control_algorithms = [ 'Minstrel', 'Ideal', 'Thompson']

# Initialize a list to store all throughput values
all_throughput_values = []

# Loop over the distance range with the specified step size
for distance in np.arange(distance_start, distance_end + distance_step, distance_step):
    # Loop over each series
    for series_name in series_names:
        # Loop over each rate control algorithm
        for algorithm_name in rate_control_algorithms:
            # Construct the correct path to the CSV file for throughput using the current distance value, series name, and algorithm name
            csv_file_name = f'packet_loss_vs_distance_sta2_{algorithm_name}-{series_name}{distance:.6f}.csv'
            csv_file_path = os.path.join(csv_directory, csv_file_name)

            # Read data from the CSV file
            with open(csv_file_path, 'r') as file:
                reader = csv.reader(file)
                for row in reader:
                    all_throughput_values.append(float(row[1]))

# Plot histogram with different classes for throughput
plt.figure(figsize=(10, 6))
hist, bins, _ = plt.hist(all_throughput_values, bins=20, edgecolor='black', alpha=0.7)
plt.xlabel('Packet loss (%)', fontsize=24)
plt.ylabel('Number of occurrences', fontsize=24)
plt.title('Histogram of Packet loss (Idealwifimanger , Minstrel-Ht , Thompson sampling)', fontsize=28)
plt.grid(True)

# Annotate each bin with its count
for i in range(len(hist)):
    plt.text(bins[i], hist[i], str(int(hist[i])), ha='center', va='bottom')

plt.show()
