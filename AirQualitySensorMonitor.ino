#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>

// Pin assignments for LCD and other components
const int rs = D5, en = D4, d4 = D3, d5 = D2, d6 = D1, d7 = D0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Initialize the LCD with the specified pin assignments

const int aqsensor = A0;  // Output of MQ135 connected to A0 pin of ESP8266

// LED and buzzer pin assignments
int gled = D6;  // Green LED connected to pin D6 (indicates good air quality)
int rled = D7;  // Red LED connected to pin D7 (indicates poor air quality)
int buz = D8;   // Buzzer connected to pin D8 (alerts for dangerous air quality)

// API and WiFi credentials
String apiKey = "YOUR_API_KEY";  // Enter your ThingSpeak Write API key here
const char *ssid = "YOUR_SSID";   // Enter your WiFi Name
const char *pass = "YOUR_PASSWORD"; // Enter your WiFi Password
const char *server = "api.thingspeak.com"; // ThingSpeak server address
WiFiClient client; // Create a WiFi client for making requests

// Define air quality thresholds for notifications
int lowThreshold = 130;  // Default low threshold for air quality (normal)
int highThreshold = 500; // Default high threshold for air quality (danger)

void setup() {
    // Set pin modes for LEDs and buzzer
    pinMode(gled, OUTPUT);   // Set green LED as output for normal air quality indication
    pinMode(aqsensor, INPUT); // Set MQ135 sensor pin as input to read air quality
    pinMode(rled, OUTPUT);    // Set red LED as output for danger indication
    pinMode(buz, OUTPUT);     // Set buzzer pin as output for alerts

    Serial.begin(115200);     // Start serial communication for debugging
    lcd.begin(16, 2);         // Initialize the LCD with 16 columns and 2 rows

    // Connect to WiFi
    WiFi.begin(ssid, pass);    // Begin WiFi connection with provided credentials
    while (WiFi.status() != WL_CONNECTED) { // Wait until WiFi is connected
        delay(1000); // Wait for 1 second before checking the connection status again
        Serial.print("."); // Print a dot for each second of waiting
    }
    Serial.println("WiFi connected"); // Notify when the WiFi connection is successful
}

void loop() {
    // Check for user input via serial to set thresholds dynamically
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // Read user input until a newline
        // Check if the input starts with "low:" to set the low threshold
        if (input.startsWith("low:")) {
            lowThreshold = input.substring(4).toInt(); // Update low threshold
            Serial.print("Low threshold set to: "); // Print confirmation message
            Serial.println(lowThreshold);
        } 
        // Check if the input starts with "high:" to set the high threshold
        else if (input.startsWith("high:")) {
            highThreshold = input.substring(5).toInt(); // Update high threshold
            Serial.print("High threshold set to: "); // Print confirmation message
            Serial.println(highThreshold);
        }
    }

    // Read air quality level from MQ135 sensor
    int ppm = analogRead(aqsensor); // Read analog value from sensor

    // Display the air quality level on the LCD
    lcd.setCursor(0, 0);             // Set cursor to the first row
    lcd.print("Air Quality: ");       // Print message on LCD
    lcd.print(ppm);                   // Print the air quality value
    delay(1000);                      // Wait for a second before the next update

    // Check air quality against defined thresholds
    if (ppm <= lowThreshold) { // If air quality is normal
        digitalWrite(gled, LOW);   // Turn off green LED
        digitalWrite(rled, LOW);    // Turn off red LED
        digitalWrite(buz, LOW);     // Turn off buzzer
        lcd.setCursor(0, 1);        // Move cursor to the second row
        lcd.print("AQ Level Normal"); // Display normal air quality message
    } 
    else if (ppm > lowThreshold && ppm < highThreshold) { // If air quality is medium
        digitalWrite(gled, HIGH);  // Turn on green LED
        digitalWrite(rled, LOW);    // Turn off red LED
        digitalWrite(buz, LOW);     // Turn off buzzer
        lcd.setCursor(0, 1);        // Move cursor to the second row
        lcd.print("AQ Level Medium"); // Display medium air quality message
    } 
    else { // If air quality is dangerous
        lcd.setCursor(0, 1);         // Move cursor to the second row
        lcd.print("AQ Level Danger!"); // Display danger message
        digitalWrite(gled, LOW);     // Turn off green LED
        digitalWrite(rled, HIGH);     // Turn on red LED
        digitalWrite(buz, HIGH);      // Turn on buzzer to alert
    }

    delay(1000); // Wait before the next reading
}
