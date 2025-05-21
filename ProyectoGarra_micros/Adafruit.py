# Import standard python modules.
import sys
import time
import serial
import numpy as np
# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient
from Adafruit_IO import Client, Feed
# holds the count for the feed
run_count = 0
# Set to your Adafruit IO username and key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_USERNAME = "rodrigo_estrad"
ADAFRUIT_IO_KEY = "aio_HznE10cCVMnjLc897oj868huNpPi"
# Set to the ID of the feed to subscribe to for updates.
FEED_ID_receive1 = 'Servo1_TX'
FEED_ID_Send1  =  'Servo1_RX'
FEED_ID_receive2 = 'Servo2_TX'
FEED_ID_Send2  = 'Servo2_RX'
FEED_ID_receive3 = 'Servo3_TX'
FEED_ID_Send3  = 'Servo3_RX'
FEED_ID_receive4 = 'Servo4_TX'
FEED_ID_Send4  = 'Servo4_RX'
FEED_ID_receive5 = 'Servo5_TX'
FEED_ID_Send5  = 'Servo5_RX'
# Define "callback" functions which will be called when certain events
# happen (connected, disconnected, message arrived).
def connected(client):
    """Connected function will be called when the client is connected to
    Adafruit IO.This is a good place to subscribe to feed changes. The client
    parameter passed to this function is the Adafruit IO MQTT client so you
    can make calls against it easily.
    """
    # Subscribe to changes on a feed named Counter.
    print('Subscribing to Feed {0}'.format(FEED_ID_receive1))
    client.subscribe(FEED_ID_receive1) 
    print('Subscribing to Feed {0}'.format(FEED_ID_receive2))
    client.subscribe(FEED_ID_receive2) 
    print('Subscribing to Feed {0}'.format(FEED_ID_receive3))
    client.subscribe(FEED_ID_receive3)
    print('Subscribing to Feed {0}'.format(FEED_ID_receive4))
    client.subscribe(FEED_ID_receive4)
    print('Subscribing to Feed {0}'.format(FEED_ID_receive5))
    client.subscribe(FEED_ID_receive5)

    print('Waiting for feed data...')
def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)
def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print('Feed {0} received new value: {1}'.format(feed_id, payload))
    if (feed_id == FEED_ID_receive1):
            grados1 =  int(np.interp(payload, [0, 180], [0, 255]))
            miarduino.write(bytes(f"A{grados1}T", 'utf-8'))

    elif (feed_id == FEED_ID_receive2):
            grados2 = int(np.interp(payload, [0, 180], [0, 255]))
            miarduino.write(bytes(f"B{grados2}T", 'utf-8'))

    elif (feed_id == FEED_ID_receive3):
            grados3 = int(np.interp(payload, [0, 180], [0, 255]))
            miarduino.write(bytes(f"C{grados3}T", 'utf-8'))

    elif (feed_id == FEED_ID_receive4):
            grados4 = int(np.interp(payload, [0, 180], [0, 255]))
            miarduino.write(bytes(f"D{grados4}T", 'utf-8'))

    elif (feed_id == FEED_ID_receive5):
            grados5 = int(np.interp(payload, [0, 180], [0, 255]))
            miarduino.write(bytes(f"E{grados5}T", 'utf-8'))

    # Publish or "send" message to corresponding feed
    print('Sendind data back: {0}'.format(payload))
    #client.publish(FEED_ID_Send, payload)
# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
miarduino = serial.Serial(port='COM7', baudrate=9600, timeout=0.1)

# Setup the callback functions defined above.
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message
# Connect to the Adafruit IO server.
client.connect()
# The first option is to run a thread in the background so you can continue
# doing things in your program.
client.loop_background()
while True:
        """
        # Uncomment the next 3 lines if you want to constantly send data
        # Adafruit IO is rate-limited for publishing
        # so we'll need a delay for calls to aio.send_data()
        run_count += 1
        print('sending count: ', run_count)
        client.publish(FEED_ID_Send, run_count)
        """
        print('Running "main loop" ')
        time.sleep(3)