import pandas as pd
import joblib  # Zum Laden des gespeicherten Modells
import asyncio
import csv
import json
import os
import sys
import logging
from aiomqtt import Client, MqttError
import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import simpson  # For calculating the area under the curve
from scipy.signal import find_peaks  # For detecting peaks
from scipy.signal import savgol_filter  # Savitzky-Golay Filter

class DataLogging:
    def __init__(self):
        # MQTT-Broker Details
        self.BROKER = "192.168.178.21"
        self.PORT = 1883
        self.TOPIC_ENERGY = 'bipfinnland/monitoring11/data'
        self.TOPIC_AI = 'bipfinnland/hackyourcoffee11/ai'
        self.MODEL_PATH = 'random_forest_model.pkl'
        self.SCALER_PATH = 'scaler.pkl'
        self.current_data = pd.DataFrame(columns=['timestamp', 'Voltage', 'Current Low', 'Current High', 'Power Low', 'Power High', 'Energy Low', 'Energy High', 'Frequency', 'Power Factor', 'Alarm Status'])
        self.state = False

        # Logging einrichten
        logging.basicConfig(level=logging.INFO)

        # 1. Lade das gespeicherte Modell
        self.model = joblib.load(self.MODEL_PATH)
        # 2. Lade den gespeicherten Scaler
        self.scaler = joblib.load(self.SCALER_PATH)


    async def handle_energy_message(self, message):
        payload = json.loads(message.payload.decode('utf-8'))
        data = payload['data']
        data['timestamp'] = payload['timestamp']

        if (self.state == False and data.get('Current Low') > 0.05):
            self.state = True
            self.current_data = self.current_data[0:0]
            self.current_data.loc[len(self.current_data)] = data
            print("Start tracking!")
        elif(self.state and  data.get('Current Low') < 0.05):
            self.current_data.loc[len(self.current_data)] = data
            self.state = False
            print("End tracking!")
            self.current_data['timestamp'] = pd.to_datetime(self.current_data['timestamp'])
            print(self.current_data.shape[0],"Datenpunkte gespeichert.")

            print(self.current_data.head())

            features = self.extract_features()
            print(features)

            # features.drop(columns=['cycle_duration', 'time_to_first_peak','rms_current', 'max_peak','variance_current', 'mean_current'], inplace=True)
            features = features['area_under_curve'].reshape(-1, 1)

            new_predictions = self.predict_data(features)
            print("Vorhersagen für den neuen Datensatz:")
            print(new_predictions)
            payload = json.dumps({'prediction': new_predictions[0] })
            await self.client1.publish(self.TOPIC_AI, payload)


        elif(self.state):
            self.current_data.loc[len(self.current_data)] = data

    def predict_data(self, features):
        X_new_scaled = self.scaler.transform(features)
        new_predictions = self.model.predict(X_new_scaled)
        return new_predictions

    def extract_features(self):
        # Extract the relevant time and current data for this product
        time_data = (self.current_data['timestamp'] - self.current_data['timestamp'].min()).dt.total_seconds().values
        current_data = self.current_data['Current Low'].values

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

        # Create a dictionary of the features
        data_dict = {
            'area_under_curve': area_under_curve,
            'cycle_duration': cycle_duration,
            'mean_current': mean_current,
            'variance_current': variance_current,
            'max_peak': np.max(peak_values) if len(peak_values) > 0 else None,
            'time_to_first_peak': time_to_first_peak,
            'rms_current': rms_current
        }
        return data_dict

    async def evaluation_message_incoming(self, message, topic):
        if topic == self.TOPIC_ENERGY:
            await self.handle_energy_message(message)

    async def subscribe_and_listen(self, client, topic):
        await client.subscribe(topic)
        async for message in client.messages:
            await self.evaluation_message_incoming(message, topic)

    async def main(self):
        async with Client(hostname=self.BROKER, port=self.PORT) as self.client1:
            tasks = [
                self.subscribe_and_listen(self.client1, self.TOPIC_ENERGY)
            ]
            await asyncio.gather(*tasks)

if __name__ == "__main__":
    # Wenn das Betriebssystem Windows ist, die Event Loop Policy ändern
    if sys.platform.lower() == "win32" or os.name.lower() == "nt":
        from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
        set_event_loop_policy(WindowsSelectorEventLoopPolicy())

    # Instanziierung der DataLogging-Klasse und Ausführung der main-Koroutine
    data_logging = DataLogging()
    asyncio.run(data_logging.main())


