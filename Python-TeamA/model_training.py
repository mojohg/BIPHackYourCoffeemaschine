import pandas as pd
from sklearn.model_selection import train_test_split

import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers, callbacks
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

dataPath = '../Data/sample_size.xlsx' 

################ load the data:
headers = ('small cup', 'large cup')

df_excel = pd.read_excel(dataPath, sheet_name='Tabelle1', header=0, names=headers)
input_data = pd.DataFrame(columns=['iA', 'iB'])
output_data2 = pd.DataFrame(columns=['oE'])

df_ml_data_index = 0
for index, row in df_excel.iterrows():
    # print('ROW: ', row['small single cup'], row['small double cup'], row['large single cup'], row['large double cup'])
    for iA in range(0, row['small cup']+1):
        for iB in range(0, row['large cup']+1):
            oA = row['small cup'] - iA
            oB = row['large cup'] - iB
            oE = oA*1 + oB*2
            # print('   Add: ', iA, iB, ' | ', oA, oB, ' = ', oE)

            input_data.loc[df_ml_data_index] = [iA, iB] 
            output_data2.loc[df_ml_data_index] = [oE] 
            df_ml_data_index += 1

# print(input_data)
# print(output_data)
####################### split data into training and validation sets:
input_train, input_test, output_train, output_test = train_test_split(input_data, output_data2, test_size=0.3, random_state=42)
####################### create a model:
model = keras.Sequential([
    layers.Dense(64, activation='relu', input_shape=(input_train.shape[1],)),
    layers.Dense(64, activation='relu'),
    layers.Dense(64, activation='relu'),
    layers.Dense(output_train.shape[1], activation='linear'),
])
model.compile(optimizer='adam', loss='mean_squared_error', metrics=['mean_squared_error'])
####################### train a model:
early_stopping = callbacks.EarlyStopping(
    min_delta=0.001, # minimium amount of change to count as an improvement
    patience=20, # how many epochs to wait before stopping
    restore_best_weights=True,
)
model.fit(input_train, output_train, epochs=200, batch_size=32, callbacks=[early_stopping], validation_split=0.2)
####################### test the model:
loss, mse = model.evaluate(input_test, output_test)
print(f"Mean Squared Error: {mse}")
print(f"Loss: {loss}")
output_pred = model.predict(input_test)
df_output_pred = pd.DataFrame(output_pred, columns=['oE']).round().astype(int)
# df_output_pred = pd.DataFrame(output_pred, columns=['oA', 'oB']).round().astype(int)
df_output_pred[df_output_pred < 0] = 0
df_output_test = output_test
df_output_test.index = range(df_output_test.shape[0])
df_compare_pred = pd.concat([df_output_test, df_output_pred], axis=1)

print(df_compare_pred)
# print(output_test, output_pred)

####################### prediction test:
input_test2 = input_test.iloc[0].values.reshape(1, -1)
output_pred2 = model.predict(input_test2)
# print(type(input_test2))
print('Single prediction test:', input_test2, output_pred2)

####################### save model:
model.save('trained_model_remaining_cups.keras')




    