import pandas as pd
import matplotlib.pyplot as plt

# CSV-Datei laden
energy_data = pd.read_csv('energy_data.csv')

# Konvertiere Zeitstempel in Datetime-Objekte mit UTC-Zeitzone
energy_data['timestamp'] = pd.to_datetime(energy_data['timestamp'], utc=True)

# Stromverlauf extrahieren (current_low oder current_high je nach Bedarf)
current_low = energy_data['current_low']
timestamps = energy_data['timestamp']

# Plot erstellen
plt.figure(figsize=(14, 7))

# Plot für current_low
plt.scatter(timestamps, current_low, label='Current Low (A)', color='blue', s=1)

# Diagrammbeschriftungen und Titel hinzufügen
plt.xlabel('Time')
plt.ylabel('Current (A)')
plt.title('Current Low over Time')
plt.legend()

# Achsen und Grid anpassen
plt.xticks(rotation=45)
plt.grid(True)

# Diagramm anzeigen
plt.tight_layout()
plt.show()
