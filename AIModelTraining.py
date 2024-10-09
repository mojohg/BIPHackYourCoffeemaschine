import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, accuracy_score
import joblib  # Bibliothek zum Speichern und Laden von Modellen

# 1. Lade die CSV-Datei (Trainingsdatensatz)
data = pd.read_csv('training_data.csv')
data.drop(columns=['cycle_duration', 'time_to_first_peak','rms_current', 'max_peak','variance_current', 'mean_current'], inplace=True)

print(data.columns)
# 2. Datenvorbereitung: Labels in der ersten Spalte
y = data.iloc[:, 0]    # Zielvariable (erste Spalte)
X = data.iloc[:, 1:]   # Features (alle anderen Spalten)

# 3. Teile die Daten in Trainings- und Testdaten
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)

# 4. Optional: Skaliere die Daten
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

# 5. Trainiere das Modell (Random Forest Klassifikator)
model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# 6. Teste das Modell und zeige die Bewertungsmetriken
y_pred = model.predict(X_test)
print("Klassifikationsbericht:")
print(classification_report(y_test, y_pred))

# 7. Zeige die Genauigkeit des Modells an
accuracy = accuracy_score(y_test, y_pred)
print(f"Genauigkeit des Modells: {accuracy * 100:.2f}%")

# 8. Speichere das trainierte Modell
joblib.dump(model, 'random_forest_model.pkl')
print("Das Modell wurde erfolgreich gespeichert.")

# 9. Speichere den Scaler ebenfalls, um ihn für neue Daten zu verwenden
joblib.dump(scaler, 'scaler.pkl')
print("Der Scaler wurde ebenfalls erfolgreich gespeichert.")
