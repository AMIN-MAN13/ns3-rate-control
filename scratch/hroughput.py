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
series_names = [ '0','1', '2', '3', '4', '5', '6', '7',"8", '9']
num_series = 10

# Initialize a NumPy array to store data for each series
series_data = np.zeros((num_series, int((distance_end - distance_start) / distance_step) + 1, 2))

# Loop over the distance range with the specified step size
for distance_index, distance in enumerate(np.arange(distance_start, distance_end + distance_step, distance_step)):
# Plot individual lines for each series with dotted lines and visible markers
 plotted_mcs = set()  # To keep track of which MCS have been plotted
for series_index in range(num_series):
    distances = []
    throughput_values = []

    # Loop over each distance point
    for distance_index, distance in enumerate(np.arange(distance_start, distance_end + distance_step, distance_step)):
        # Construct the path to the CSV file for the current series and distance
        csv_file_name = f'packet_loss_vs_distance_sta2_Constant-{series_names[series_index]}{distance:.6f}.csv'
        csv_file_path = os.path.join(csv_directory, csv_file_name)

        # Check if the CSV file exists
        if os.path.isfile(csv_file_path):
            # Read data from the CSV file
            with open(csv_file_path, 'r') as file:
                reader = csv.reader(file)
                for row in reader:
                    throughput_values.append(float(row[1]))
                    distances.append(distance)

    # Plot the data for the current series if available and not already plotted
    if len(distances) > 0 and series_index not in plotted_mcs:
        plt.plot(distances, throughput_values, label=f'MCS {series_index}', linestyle='dotted', marker='o')
        plotted_mcs.add(series_index)  # Add the current MCS to the set of plotted MCS

# Add labels and legend to the plot
plt.title('Throughput vs Distance for Station 2 (Constant-rate(Difference MCS))', fontsize=28)
plt.xlabel('Distance (m)', fontsize=24)
plt.ylabel('Throughput (Mbps)', fontsize=24)
plt.legend()
plt.grid(True)

# Save the plot as an image file (optional)
plt.savefig('throughput_vs_distance_sta2_Minstrel-Ht_combined.png')

# Display the graph
plt.show()