import random
import time
from paho.mqtt import client as mqtt_client

broker = 'broker.emqx.io'
port = 1883
topic = "Aartikanwar/sensors/status/D4:8A:FC:AA:C9:78"
client_id = f'subscribe-{random.randint(0, 1000)}'

# Function to connect to the MQTT broker
def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print(f"Failed to connect, return code {rc}")

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

# Function to subscribe to the topic
def subscribe(client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(topic)  # Subscribe to the topic
    client.on_message = on_message  # Set callback for receiving messages

# Main function to run the MQTT client
def run():
    client = connect_mqtt()  # Connect to the broker
    client.loop_start()  # Start the client loop to handle messages
    subscribe(client)  # Subscribe to the topic to receive messages
    try:
        while True:
            time.sleep(10)  # Keep the program running to listen for incoming messages
    except KeyboardInterrupt:
        print("Disconnected from MQTT Broker.")
        client.loop_stop()  # Stop the loop when exiting

if __name__ == '__main__':
    run()
