#include <OneWire.h>
#include <DallasTemperature.h>

// Define pin assignments
const int bottomSensorVccPin = 2; // Pin to control VCC of the bottom sensor
const int bottomSensorPin = A1;   // Analog pin for bottom water depth sensor
const int topSensorPin = A2;      // Analog pin for top water depth sensor
const int motorRelayPin = 10;
const int roRelayPin = 5;
const int tempSensorPin = 7;
const int turbiditySensorPin = A0;
const int h2sSensorPin = A3;      // Analog pin connected to H₂S sensor
bool roOn = false;
bool motorOn = false;

// Thresholds
int BOTTOM_SENSOR_THRESHOLD = 550; // Example threshold for bottom sensor
int TOP_SENSOR_THRESHOLD = 200;    // Example threshold for top sensor
int turbidityThreshold = 500;      // Example threshold for turbidity
int h2sThreshold = 300;            // Example threshold for H₂S

// Initialize sensors
OneWire oneWire(tempSensorPin);
DallasTemperature sensors(&oneWire);

void setup() {
    pinMode(motorRelayPin, OUTPUT);
    pinMode(roRelayPin, OUTPUT);
    pinMode(bottomSensorVccPin, OUTPUT); // Set VCC control pin as output
    digitalWrite(bottomSensorVccPin, LOW); // Initially turn off the sensor

    sensors.begin();
    Serial.begin(9600); // Initialize serial communication for debugging

    // Initial states
    digitalWrite(motorRelayPin, HIGH); // Set to HIGH to keep the motor off initially
    digitalWrite(roRelayPin, HIGH);    // Set to HIGH to keep the RO off initially
}

int readBottomSensor() {
    digitalWrite(bottomSensorVccPin, HIGH);  // Power on the bottom sensor
    delay(100); // Wait for sensor stabilization
    int value = analogRead(bottomSensorPin); // Read the sensor value
    digitalWrite(bottomSensorVccPin, LOW);   // Power off the sensor
    return value;
}

void loop() {
    // Read sensor values
    int bottomSensorValue = readBottomSensor();
    int topSensorValue = analogRead(topSensorPin);
    sensors.requestTemperatures();
    float waterTemp = sensors.getTempCByIndex(0);
    int turbidityValue = analogRead(turbiditySensorPin);
    int h2sValue = analogRead(h2sSensorPin);

    // Control motor based on water level
    motorOn = false;
    if (bottomSensorValue < BOTTOM_SENSOR_THRESHOLD) {
        while (true) {
            topSensorValue = analogRead(topSensorPin);
            bottomSensorValue = readBottomSensor(); // Read the top sensor value
            Serial.print("Bottom: ");
            Serial.print(bottomSensorValue);
            Serial.print(", Top: ");
            Serial.print(topSensorValue);
            Serial.print(", Temp: ");
            Serial.print(waterTemp);
            Serial.print(", Turbidity: ");
            Serial.print(turbidityValue);
            Serial.print(", H₂S: ");
            Serial.print(h2sValue);
            Serial.print(", Motor: ");
            Serial.print(motorOn ? "ON" : "OFF");
            Serial.print(", RO: ");
            Serial.println(roOn ? "ON" : "OFF");
            if (topSensorValue >= TOP_SENSOR_THRESHOLD) {
                digitalWrite(motorRelayPin, HIGH); // Turn off motor (active-low relay)
                motorOn = false;
                break; // Exit the loop once the tank is full
            } else {
                digitalWrite(motorRelayPin, LOW);
                motorOn = true;
                delay(5000);
            }
        }
    } else {
        digitalWrite(motorRelayPin, HIGH);
        motorOn = false;
    }

    // Control RO based on temperature, turbidity, H2S
    roOn = false;
    if (waterTemp > 40 || turbidityValue > turbidityThreshold || h2sValue > h2sThreshold) {
        digitalWrite(roRelayPin, HIGH); // Turn off RO (active-low relay)
        roOn = false;
    } else {
        digitalWrite(roRelayPin, LOW); // Turn on RO (active-low relay)
        roOn = true;
    }

    // Print data to serial for debugging
    Serial.print("Bottom: ");
    Serial.print(bottomSensorValue);
    Serial.print(", Top: ");
    Serial.print(topSensorValue);
    Serial.print(", Temp: ");
    Serial.print(waterTemp);
    Serial.print(", Turbidity: ");
    Serial.print(turbidityValue);
    Serial.print(", H₂S: ");
    Serial.print(h2sValue);
    Serial.print(", Motor: ");
    Serial.print(motorOn ? "ON" : "OFF");
    Serial.print(", RO: ");
    Serial.println(roOn ? "ON" : "OFF");

    delay(1000); // Delay for sensor reading
}
