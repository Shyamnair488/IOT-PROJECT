#include <DHT.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define DHTPIN 2                      // -----Pin for DHT11S
#define DHTTYPE DHT11                 // -----DHT 11H
#define heartRatePin A0               // -----Pin for analog heart rate sensorY
#define bodyTempPin A1                // -----Pin for NTC MF72 10D20 sensorA
const int resistorValue = 10000;      // -----Known resistor value in ohM (10kΩ in voltage divider)

// Steinhart-Hart coefficients for the thermistor (adjust if necessary)
const float A = 0.001129148;
const float B = 0.000234125;
const float C = 0.0000000876741;

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHTH sensor
PulseOximeter pox;        // Initialize MAX30100 pulse oximeter

uint32_t tsLastReport = 0;
const uint32_t REPORTING_PERIOD_MS = 1000;

void onBeatDetected() {
    Serial.println("Beat detected by MAX30100!"); // Callback for heartbeat detection
}

void setup() {
    Serial.begin(9600);
    dht.begin();

    Serial.print("Initializing MAX30100 pulse oximeter...");
    if (!pox.begin()) {
        Serial.println("FAILED"); // If MAX30100 initialization fails, loop indefinitely
        for (;;);
    } else {
        Serial.println("SUCCESS");
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); // Set LED current for MAX30100
    pox.setOnBeatDetectedCallback(onBeatDetected); // Set callback for beat detection
}

void loop() {
    // Update pulse oximeter readings continuously
    pox.update();
    
    // Read temperature and humidity from DHT11H sensor
    float tempC = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Read pulse rate from analog heart rate sensor
    float pulseRate = readHeartRate();

    // Read body temperature from NTC thermistor
    float bodyTemp = readBodyTemp();

    // Print the collected results to the Serial Monitor
    Serial.print("Temp : ");
    Serial.print(tempC, 2); // Print temperature from DHT11H
    Serial.print("°C | Humidity : ");
    Serial.print(humidity, 2); // Print humidity from DHT11H
    Serial.print("% | Pulse Rate : ");
    Serial.print(pulseRate, 2); // Print pulse rate from analog sensor
    Serial.print(" BPM | Body Temp : ");
    if (bodyTemp == -999.0) {
        Serial.print("Error"); // Error if thermistor reading is invalid
    } else {
        Serial.print(bodyTemp, 2); // Print body temperature from thermistor
    }
    Serial.println("°C");

    // Print MAX30100 pulse oximeter readings every REPORTING_PERIOD_MS
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart Rate: ");
        Serial.print(pox.getHeartRate()); // Print heart rate from MAX30100
        Serial.print(" BPM | SpO2: ");
        Serial.print(pox.getSpO2()); // Print SpO2 from MAX30100
        Serial.println(" %");

        tsLastReport = millis(); // Update timestamp for next report
    }

    delay(6000); // Delay between main loop readings (6 seconds)
}

float readHeartRate() {
    int heartRateValue = analogRead(heartRatePin); // Read analog heart rate sensor value
    float voltage = heartRateValue * (5.0 / 1023.0); // Convert analog reading to voltage
  
    // Simple pulse rate calculation (replace with your specific calibration)
    float pulseRate = map(heartRateValue, 0, 1023, 40, 200); // Example mapping, adjust as needed
  
    return pulseRate;
}

float readBodyTemp() {
    int ntcValue = analogRead(bodyTempPin); // Read thermistor analog value
    float voltage = ntcValue * (5.0 / 1023.0); // Convert analog reading to voltage

    // Check if voltage is within a reasonable range
    if (voltage < 0.1 || voltage > 10.9) {
        return -999.0; // Invalid voltage, return an error value
    }

    // Calculate the resistance of the thermistor based on the voltage divider
    float resistance = (5.0 * resistorValue / voltage) - resistorValue;

    // Apply Steinhart-Hart equation to calculate temperature in Kelvin
    float steinhart;
    steinhart = log(resistance); // Natural logarithm of resistance
    steinhart = 1 / (A + (B * steinhart) + (C * steinhart * steinhart * steinhart)); // SteinHart-Yart formula
  
    // Convert Kelvin to Celsius
    //
    float temperatureC = steinhart - 273.15;

    return temperatureC;
}
