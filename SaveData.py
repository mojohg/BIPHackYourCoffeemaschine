#!/usr/bin/env python3
# vim: set fileencoding=utf-8 :
import asyncio
import csv
import json
import os
import sys
import logging
from aiomqtt import Client, MqttError


class DataLogging:
    def __init__(self):
        # MQTT-Broker Details
        self.BROKER = "192.168.178.21"
        # self.BROKER = "test.mosquitto.org"
        self.PORT = 1883
        self.TOPIC_ENERGY = 'bipfinnland/monitoring11/data'
        self.TOPIC_COFFEE = 'bipfinnland/hackyourcoffee11/data'
        self.TOPIC_CONTROL = 'bipfinnland/monitoring11/control'
        self.saving_data = False
        self.ENERGY_DB = 'energy_data.csv'
        self.COFFEE_DB = 'coffee_data.csv'

        # Logging einrichten
        logging.basicConfig(level=logging.INFO)

        # CSV-Dateien initialisieren
        self.initialize_csv(self.ENERGY_DB,
                            ['timestamp', 'device_id', 'voltage', 'current_low', 'current_high', 'power_low',
                             'power_high', 'energy_low', 'energy_high', 'frequency', 'power_factor', 'alarm_status'])
        self.initialize_csv(self.COFFEE_DB, ['timestamp', 'label', 'info'])

    def initialize_csv(self, file_path, headers):
        if not os.path.exists(file_path):
            with open(file_path, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(headers)

    async def handle_energy_message(self, message):
        if self.saving_data:
            payload = json.loads(message.payload.decode('utf-8'))
            timestamp = payload['timestamp']
            device_id = payload['device_id']
            data = payload['data']

            row = [timestamp, device_id]
            row.extend([
                data.get('Voltage'),
                data.get('Current Low'),
                data.get('Current High'),
                data.get('Power Low'),
                data.get('Power High'),
                data.get('Energy Low'),
                data.get('Energy High'),
                data.get('Frequency'),
                data.get('Power Factor'),
                data.get('Alarm Status')
            ])

            with open(self.ENERGY_DB, 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(row)

    async def handle_label_message(self, message):
        payload = json.loads(message.payload.decode('utf-8'))
        timestamp = payload['timestamp']
        label = payload['label']
        print("New Label:", label)

        with open(self.COFFEE_DB, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([timestamp, label])

    async def handle_control_message(self, message):
        payload = json.loads(message.payload.decode('utf-8'))
        control = payload.get("control")
        if control == "Start":
            self.saving_data = True
            print("Data saving started")
        elif control == "End":
            self.saving_data = False
            print("Data saving stopped")

    async def evaluation_message_incoming(self, message, topic):
        if topic == self.TOPIC_CONTROL:
            await self.handle_control_message(message)
        elif topic == self.TOPIC_COFFEE:
            await self.handle_label_message(message)
        elif topic == self.TOPIC_ENERGY:
            await self.handle_energy_message(message)

    async def subscribe_and_listen(self, client, topic):
        await client.subscribe(topic)
        async for message in client.messages:
            await self.evaluation_message_incoming(message, topic)

    async def main(self):
        async with Client(hostname=self.BROKER, port=self.PORT) as client1, \
                   Client(hostname=self.BROKER, port=self.PORT) as client2, \
                   Client(hostname=self.BROKER, port=self.PORT) as client3:
            tasks = [
                self.subscribe_and_listen(client1, self.TOPIC_ENERGY),
                self.subscribe_and_listen(client2, self.TOPIC_COFFEE),
                self.subscribe_and_listen(client3, self.TOPIC_CONTROL)
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
