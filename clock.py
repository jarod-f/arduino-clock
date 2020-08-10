import time
import datetime
import serial

# Initialize serial USB connection
ser = serial.Serial('COM5', 9600)

# We wait so that the arduino has a moment to do all its setup
print("Initializing...")
time.sleep(1)
print("Initialized.")

while True:
    # Get the current time
    now = datetime.datetime.now()
    current_time = now.strftime("%I:%M:%S")

    # Send the current time to the arduino
    ser.write(bytes(current_time, encoding="utf-8"))
    print("\"" + current_time + "\" was sent to arduino.")
    
    # Wait so we don't send more info to the arduino than it can process
    time.sleep(1)