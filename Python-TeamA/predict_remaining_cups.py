from tensorflow.keras.models import load_model
import asyncio
import aiomqtt
import sys
import os
import json
import numpy as np


print('loading model....')
ml_model = load_model('trained_model_remaining_cups.keras')
print('Model loaded!')

BROKER_ADDRESS = "192.168.178.21"
PORT = 1883 
ESP_SUB_TOPIC = "bipfinnland/ESP10/State"
NODE_RED_PUB_TOPIC = "bipfinnland/Predict10/RemainingCups"

g_previous_refilled_pressed = False
g_previous_left_pressed = False
g_previous_right_pressed = False

produce_products = [0,0] # small , large

async def on_message(client, topic, payload):
    global g_previous_refilled_pressed, g_previous_left_pressed, g_previous_right_pressed, produce_products, ml_model

    raw_data_string = payload.decode()
    json_doc = json.loads(raw_data_string)
    refilled_pressed = json_doc['Refilled']
    left_pressed = json_doc['ButtonCoffeeLeft']
    right_pressed = json_doc['ButtonCoffeeRight']
    slider = json_doc['SliderPosition']
    refilled_event = (refilled_pressed != g_previous_refilled_pressed and refilled_pressed==True)
    left_coffee_event = (left_pressed != g_previous_left_pressed and left_pressed==True)
    right_coffee_event = (right_pressed != g_previous_right_pressed and right_pressed==True)
    changed = False

    if refilled_event:
        print(f"Refilled!")
        produce_products = [0,0]
        changed = True

    if left_coffee_event:
        print(f"Left!")
        if(slider == "Left"):
            produce_products[0] += 1
            changed = True
        elif(slider == "Right"):
            produce_products[1] += 1
            changed = True

    if right_coffee_event:
        print(f"Right!")
        if(slider == "Left"):
            produce_products[0] += 2
            changed = True
        elif(slider == "Right"):
            produce_products[1] += 2
            changed = True

    if changed:
        number_of_small_cups = round(ml_model.predict(np.array([produce_products]))[0][0])
        if number_of_small_cups < 0:
            number_of_small_cups = 0
        number_of_large_cups = number_of_small_cups // 2
        number_of_large_cups_remaining = number_of_small_cups % 2
        json_doc_pub = {
            "small cups":number_of_small_cups,
            "large cups":number_of_large_cups,
            "small cups remaining": number_of_large_cups_remaining
        }
        print('Products produced: ', produce_products, 'Remaining small cups:', json_doc_pub)

        await client.publish(NODE_RED_PUB_TOPIC, json.dumps(json_doc_pub))

    g_previous_refilled_pressed = refilled_pressed
    g_previous_left_pressed = left_pressed
    g_previous_right_pressed = right_pressed

async def main():
    async with aiomqtt.Client(BROKER_ADDRESS, PORT) as client:
        await client.subscribe(ESP_SUB_TOPIC)
        async for message in client.messages:
            await on_message(client, message.topic, message.payload)

if sys.platform.lower() == "win32" or os.name.lower() == "nt":
        from asyncio import set_event_loop_policy, WindowsSelectorEventLoopPolicy
        set_event_loop_policy(WindowsSelectorEventLoopPolicy())
asyncio.run(main())