import time

import serial

# Replace 'COM3' with the port where your Arduino is connected
# For Linux, it might look like '/dev/ttyUSB0' or '/dev/ttyACM0'
ser = serial.Serial('COM25', 9600)  # Open serial port at 9600 baud

# Give some time for the serial connection to establish
time.sleep(2)

try:
    while True:
        if ser.in_waiting > 0:  # Check if there's any data waiting to be read
            data = ser.readline().decode('utf-8').rstrip()  # Read the data and decode it
            print(data)  # Display the data
except KeyboardInterrupt:
    print("Program interrupted")
finally:
    ser.close()  # Close the serial port when done
