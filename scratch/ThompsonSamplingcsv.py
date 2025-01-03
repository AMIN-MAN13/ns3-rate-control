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
series_names = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'k']
num_series = 9

# Initialize a NumPy array to store data for each series
series_data = np.zeros((num_series, int((distance_end - distance_start) / distance_step) + 1, 2))

# Loop over the distance range with the specified step size
for distance_index, distance in enumerate(np.arange(distance_start, distance_end + distance_step, distance_step)):
    # Loop over each series
    for series_index, series_name in enumerate(series_names):
        # Construct the correct path to the CSV file using the current distance value and series name
        csv_file_name = f'throughput_vs_distance_sta2_ThompsonSampling-{series_name}{distance:.6f}.csv'
        csv_file_path = os.path.join(csv_directory, csv_file_name)

        # Read data from the CSV file
        throughput_values = []

        with open(csv_file_path, 'r') as file:
            reader = csv.reader(file)
            for row in reader:
                throughput_values.append(float(row[1]))

        # Store the data for each series
        series_data[series_index, distance_index, 0] = distance
        series_data[series_index, distance_index, 1] = np.mean(throughput_values)

# Plot individual lines for each series
for series_index in range(num_series):
    distances = series_data[series_index, :, 0]
    throughput_values = series_data[series_index, :, 1]
    plt.plot(distances, throughput_values, label=f'Series {series_index + 1}')

# Calculate and plot the average line
average_values = np.mean(series_data[:, :, 1], axis=0)
distance_values = series_data[0, :, 0]  # Assuming distances are the same for all series

plt.plot(distance_values, average_values, label='Average', linewidth=2)

# Add labels and legend to the plot
plt.title('Throughput vs Distance for Station 2(ThompsonSampling)')
plt.xlabel('Distance (m)')
plt.ylabel('Throughput (Mbps)')
plt.legend()
plt.grid(True)

# Save the plot as an image file (optional)
plt.savefig('throughput_vs_distance_sta2_Minstrel-Ht_combined.png')

# Display the graph
plt.show()