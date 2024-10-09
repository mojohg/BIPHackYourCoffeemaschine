import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.integrate import simpson  # For calculating the area under the curve
from scipy.signal import find_peaks  # For detecting peaks
from scipy.signal import savgol_filter  # Savitzky-Golay Filter

# Load the entire dataset
coffee_file_path = 'coffee_data.csv'
coffee_data = pd.read_csv(coffee_file_path)

# Convert timestamp to datetime and compute time differences between rows
coffee_data['timestamp'] = pd.to_datetime(coffee_data['timestamp'])

# Load the entire dataset
file_path = 'energy_data.csv'
energy_data = pd.read_csv(file_path)

# Convert timestamp to datetime and compute time differences between rows
energy_data['timestamp'] = pd.to_datetime(energy_data['timestamp'])
energy_data['time_diff'] = energy_data['timestamp'].diff().dt.total_seconds()

# Define a threshold for detecting product boundaries (e.g., 5 seconds of inactivity)
product_boundary_threshold = 5

# Add a product number identifier by detecting where the time gap exceeds the threshold
energy_data['product_id'] = (energy_data['time_diff'] > product_boundary_threshold).cumsum()

# Initialize output file for features
output_file = 'training_data.csv'

# Iterate over each product (grouped by product_id) and extract features
for product_id, product_data in energy_data.groupby('product_id'):
    # Extract the relevant time and current data for this product
    time_data = (product_data['timestamp'] - product_data['timestamp'].min()).dt.total_seconds().values
    current_data = product_data['current_low'].values

    # --- Part 1: Savitzky-Golay Filter for Smoothing ---
    window_size = 25
    poly_degree = 2
    smoothed_data_sg = savgol_filter(current_data, window_size, poly_degree)

    # --- Feature Extraction ---
    area_under_curve = simpson(smoothed_data_sg, x=time_data)
    cycle_duration = time_data[-1] - time_data[0]
    mean_current = np.mean(smoothed_data_sg)
    variance_current = np.var(smoothed_data_sg)
    peaks, _ = find_peaks(smoothed_data_sg, height=0.1)
    peak_values = smoothed_data_sg[peaks]
    time_to_first_peak = time_data[peaks[0]] if len(peaks) > 0 else None
    rms_current = np.sqrt(np.mean(smoothed_data_sg ** 2))

    # Prompt the user for a product label
    start_time = product_data['timestamp'].min()
    time_diff = abs(coffee_data['timestamp'] - start_time)
    nearest_row = coffee_data.loc[time_diff.idxmin()]
    product_label = nearest_row["label"]
    print("Found product", product_id, "with label", product_label)

    # Create a dictionary of the features
    data_dict = {
        'product_label': product_label,
        'area_under_curve': area_under_curve,
        'cycle_duration': cycle_duration,
        'mean_current': mean_current,
        'variance_current': variance_current,
        'max_peak': np.max(peak_values) if len(peak_values) > 0 else None,
        'time_to_first_peak': time_to_first_peak,
        'rms_current': rms_current
    }

    # Convert the dictionary to a DataFrame
    df = pd.DataFrame([data_dict])

    # Check if the file exists, and either create or append to it
    if not os.path.exists(output_file):
        df.to_csv(output_file, index=False)
        print(f"Created new file: {output_file}")
    else:
        df.to_csv(output_file, mode='a', header=False, index=False)
        print(f"Appended data for product {product_id} to: {output_file}")

print("Feature extraction and labeling completed!")
