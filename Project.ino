#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HardwareSerial.h>

// OLED display parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT11 parameters
#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// BMP180 instance
Adafruit_BMP085 bmp;

// Create a hardware serial instance for the GSM module
HardwareSerial gsmSerial(2); // Using UART2 (TX2 = GPIO17, RX2 = GPIO16)

const int baudRate = 115200; // Baud rate for GSM module

// SMS and ThingSpeak details
const char phone_number[] = "+919131426607"; // Replace with the recipient's phone number
const char* server = "api.thingspeak.com";   // ThingSpeak server URL
const char* apiKey = "YHC0Y5R19Y1YNYB8";     // Replace with your ThingSpeak API Key

void setup() {
  // Start serial communication with the GSM module
  gsmSerial.begin(baudRate, SERIAL_8N1, 32, 33); // RX = GPIO16, TX = GPIO17
  
  // Start serial communication with the Serial Monitor
  Serial.begin(115200);
  
  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize the DHT11 sensor
  dht.begin();

  // Initialize the BMP180 sensor
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
    while (1) {}
  }
  
  // Wait for the GSM module to initialize
  delay(3000);
  
  // Send initialization commands to the GSM module
  if (!sendATCommand("AT")) return;
  if (!sendATCommand("AT+CMGF=1")) return; // Set SMS to Text Mode
  if (!sendATCommand("AT+CSCS=\"GSM\"")) return; // Set character set to GSM
}

void loop() {
  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;

  // Check if any reads failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("Failed to read from sensors!"));
    return;
  }

  // Get comfort level
  String comfortLevel = getComfortLevel(temperature, humidity, pressure);

  // Display sensor values on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Sensor Readings:"));
  display.print(F("Temp: "));
  display.print(temperature);
  display.println(F(" C"));
  display.print(F("Humidity: "));
  display.print(humidity);
  display.println(F(" %"));
  display.print(F("Pressure: "));
  display.print(pressure);
  display.println(F(" hPa"));
  display.display();

  // Send SMS if the comfort level is "Uncomfortable"
  if (comfortLevel == "Uncomfortable") {
    sendSMS(phone_number, temperature, humidity, pressure, comfortLevel);
  }

  // Send data to ThingSpeak
  sendDataToThingSpeak(temperature, humidity, pressure, comfortLevel);

  // Wait before sending the next reading
  delay(60000); // Send every 60 seconds
}

String getComfortLevel(float temp, float hum, float pressure) {
  // Define comfort thresholds
  bool tempOK = (temp >= 20 && temp <= 24);
  bool humOK = (hum >= 40 && hum <= 60);
  bool pressureOK = (pressure >= 980 && pressure <= 1050); // Typical sea-level pressure range
  
  int score = 0;
  if (tempOK) score++;
  if (humOK) score++;
  if (pressureOK) score++;
  
  if (score == 3) {
    return "Very Comfortable";
  } else if (score >= 2) {
    return "Comfortable";
  } else {
    return "Uncomfortable";
  }
}

bool sendATCommand(const char* command) {
  gsmSerial.println(command);
  delay(100);
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) { // 2 seconds timeout
    if (gsmSerial.available()) {
      String response = gsmSerial.readString();
      Serial.println(response);
      if (response.indexOf("OK") != -1) {
        return true; // Command succeeded
      }
      break;
    }
  }
  Serial.println("Command failed or timed out");
  return false; // Command failed
}

void sendSMS(const char* number, float temp, float hum, float pressure, String comfortLevel) {
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(1000);

  // Create the message string
  String message = "Comfort Alert:\n";
  message += "Temp: " + String(temp) + " C\n";
  message += "Humidity: " + String(hum) + "%\n";
  message += "Pressure: " + String(pressure) + " hPa\n";
  message += "Comfort Level: " + comfortLevel;
  
  gsmSerial.print(message);
  delay(100);
  
  gsmSerial.write(26); // Ctrl+Z to send the message
  delay(5000); // Wait for SMS confirmation
}

void sendDataToThingSpeak(float temp, float hum, float pressure, String comfortLevel) {
  gsmSerial.println("AT+CGATT=1"); // Attach to GPRS
  delay(2000);

  gsmSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // Set the bearer profile
  delay(1000);

  gsmSerial.println("AT+SAPBR=3,1,\"APN\",\"your-apn\""); // Set your APN
  delay(1000);

  gsmSerial.println("AT+SAPBR=1,1"); // Open the bearer connection
  delay(2000);

  gsmSerial.println("AT+HTTPINIT"); // Initialize HTTP service
  delay(1000);

  gsmSerial.println("AT+HTTPPARA=\"CID\",1"); // Set the HTTP connection ID
  delay(1000);

  gsmSerial.print("AT+HTTPPARA=\"URL\",\"http://");
  gsmSerial.print(server);
  gsmSerial.print("/update?api_key=");
  gsmSerial.print(apiKey);
  gsmSerial.print("&field1=");
  gsmSerial.print(temp);
  gsmSerial.print("&field2=");
  gsmSerial.print(hum);
  gsmSerial.print("&field3=");
  gsmSerial.print(pressure);
  gsmSerial.print("&field4=");
  gsmSerial.print(comfortLevel);
  gsmSerial.println("\"");
  delay(1000);

  gsmSerial.println("AT+HTTPACTION=1"); // Send the HTTP POST request
  delay(5000);

  gsmSerial.println("AT+HTTPTERM"); // Close the HTTP service
  delay(1000);

  gsmSerial.println("AT+SAPBR=0,1"); // Close the bearer connection
  delay(1000);

  Serial.println("Data sent to ThingSpeak!");
}