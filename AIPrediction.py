import pandas as pd
import joblib  # Zum Laden des gespeicherten Modells

# 1. Lade das gespeicherte Modell
model = joblib.load('random_forest_model.pkl')
print("Das Modell wurde erfolgreich geladen.")

# 2. Lade den gespeicherten Scaler
scaler = joblib.load('scaler.pkl')
print("Der Scaler wurde erfolgreich geladen.")

# 3. Lade den neuen Datensatz, den du klassifizieren möchtest
new_data = pd.read_csv('oneProduct.csv')

# 4. Wende die gleiche Skalierung auf den neuen Datensatz an
X_new = new_data.iloc[:, :]
X_new_scaled = scaler.transform(X_new)

# 5. Klassifiziere die neuen Daten
new_predictions = model.predict(X_new_scaled)

# 6. Zeige die Vorhersagen an
print("Vorhersagen für den neuen Datensatz:")
print(new_predictions)
