#include <DHT.h>

#define DHTPIN 2            // Pin for DHT11
#define DHTTYPE DHT11       // DHT 11
#define heartRatePin A0     // Pin for heart rate sensor
#define bodyTempPin A1      // Pin for NTC MF72 10D20 sensor
const int resistorValue = 10000; // Known resistor value in ohms (10kΩ in voltage divider)

// Steinhart-Hart coefficients for the thermistor (adjust if necessary)
const float A = 0.001129148;
const float B = 0.000234125;
const float C = 0.0000000876741;

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Read temperature and humidity from DHT11
  float tempC = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read heart rate sensor
  float pulseRate = readHeartRate();

  // Read body temperature from NTC MF72 10D20 sensor
  float bodyTemp = readBodyTemp();

  // Print the results
  Serial.print("Temp: ");
  Serial.print(tempC, 2);
  Serial.print("°C | Humidity: ");
  Serial.print(humidity, 2);
  Serial.print("% | Pulse: ");
  Serial.print(pulseRate, 2);
  Serial.print(" BPM | Body Temp: ");
  if (bodyTemp == -999.0) {
    Serial.print("Error");
  } else {
    Serial.print(bodyTemp, 2);
  }
  Serial.println("°C");

  delay(6000); // Delay between readings (6 seconds)
}

float readHeartRate() {
  int heartRateValue = analogRead(heartRatePin);
  float voltage = heartRateValue * (5.0 / 1023.0);
  
  // Simple pulse rate calculation (replace with your specific calibration)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               SHYAM.NAIR.
  float pulseRate = map(heartRateValue, 0, 1023, 40, 200); // Example mapping, adjust as needed
  
  return pulseRate;
}

float readBodyTemp() {
  int ntcValue = analogRead(bodyTempPin);
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
  steinhart = 1 / (A + (B * steinhart) + (C * steinhart * steinhart * steinhart)); // Steinhart-Hart formula
  
  // Convert Kelvin to Celsius
  float temperatureC =  273.15 - steinhart;

  return temperatureC;
}
