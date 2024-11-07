import serial
import time

# Define the serial port and baud rate
serial_port = 'COM13'  # Change to your port
baud_rate = 9600

# Open the serial port
ser = serial.Serial(serial_port, baud_rate, timeout=1)

while True:
    if ser.in_waiting > 0:
        raw_data = ser.readline().decode('utf-8').strip()
        # Open the file for writing (this will overwrite existing content)
        with open('C:/Users/Sanjith/Desktop/Mepexpo/data.txt', 'w') as file:
            file.write(raw_data + '\n')
            file.flush()  # Ensure data is written to the file
    time.sleep(1)  # Adjust the delay as needed
