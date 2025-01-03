import os
import matplotlib.pyplot as plt
import csv
import numpy as np

# Specify the directory path where the CSV files are located
csv_directory = '/home/amin/Videos'

# Change the current working directory
os.chdir(csv_directory)

# Define the distance range and step size
distance_start = 20.0
distance_end = 400.0
distance_step = 10.0

# Define the series names
series_names = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
num_series = 10

# Initialize a list to store all throughput values
all_throughput_values = []

# Loop over the distance range with the specified step size
for distance in np.arange(distance_start, distance_end + distance_step, distance_step):
    # Loop over each series
    for series_name in series_names:
        # Construct the correct path to the CSV file for throughput using the current distance value and series name
        csv_file_name = f'packet_loss_vs_distance_sta2_Constant-{series_name}{distance:.6f}.csv'
        csv_file_path = os.path.join(csv_directory, csv_file_name)

        # Read data from the CSV file
        with open(csv_file_path, 'r') as file:
            reader = csv.reader(file)
            for row in reader:
                all_throughput_values.append(float(row[1]))

# Plot histogram with different classes for throughput
plt.figure(figsize=(10, 6))
hist, bins, _ = plt.hist(all_throughput_values, bins=20, edgecolor='black', alpha=0.7)
plt.xlabel('Packet loss (%)',fontsize=24)
plt.ylabel('number of occurrences',fontsize=24)
plt.title('Histogram of Packet loss (Constant-rate(Difference MCS))',fontsize=28)
plt.grid(True)

# Annotate each bin with its count
for i in range(len(hist)):
    plt.text(bins[i], hist[i], str(int(hist[i])), ha='center', va='bottom')

plt.show()
