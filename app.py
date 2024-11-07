from flask import Flask, jsonify, render_template
import re
import threading

app = Flask(__name__)
file_path = 'C:/Users/Sanjith/Desktop/Mepexpo/data.txt'  # Path to your data file
parsed_data = {}  # Initialize parsed_data to ensure it exists

# Function to read the latest data from the file
def read_from_file():
    global parsed_data
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            if lines:
                # Read the last line of the file
                last_line = lines[-1].strip()
                # Update global data variable with parsed data
                parsed_data = parse_sensor_data(last_line)
            else:
                parsed_data = {}
    except IOError as e:
        print(f"Error reading file {file_path}: {e}")

# Function to parse sensor data
def parse_sensor_data(raw_data):
    data = {}
    print(raw_data)
    pattern = r'Bottom:\s*(\d+),\s*Top:\s*(\d+),\s*Temp:\s*([-?\d.]+),\s*Turbidity:\s*(\d+),\s*H₂S:\s*(\d+),\s*Motor:\s*(ON|OFF),\s*RO:\s*(ON|OFF)'
    match = re.match(pattern, raw_data)
    if match:
        if int(match.group(1)) < 550:
            data['bottom'] = 'EMPTY'
        elif int(match.group(1)) > 550 and int(match.group(2)) < 200:
            data['bottom'] = 'HALF'
        elif int(match.group(2)) > 200:
            data['bottom'] = 'FULL'
        data['temperature'] = float(match.group(3))
        data['turbidity'] = int(match.group(4))
        data['h2s'] = int(match.group(5))
        data['motor'] = match.group(6)
        data['ro'] = match.group(7)
    else:
        data = {'error': 'Data format error'}
    print(data)  # Debug print
    return data


# Thread function to periodically read from the file
def file_thread():
    while True:
        read_from_file()
        # Read from the file every 5 seconds
        threading.Event().wait(5)

# Start the file thread
threading.Thread(target=file_thread, daemon=True).start()

@app.route('/data', methods=['GET'])
def get_data():
    if parsed_data:
        return jsonify(parsed_data)
    else:
        return jsonify({'error': 'No data available'}), 404

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(debug=True)
