#include <SPI.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <EduIntro.h> // Include the EduIntro library for DHT11

#define DHTPIN D7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // Type of the DHT sensor
#define BUZZER_SIGNAL_PIN 2 // Digital pin connected to the buzzer signal
#define BUZZER_POWER_PIN 13 // Digital pin connected to the buzzer power

char ssid[] = "WiFi-13EA2A";    // your network SSID (name)
char pass[] = "43038930";       // your network password

WiFiClient client;

unsigned long myChannelNumber = 2508187; // Replace with your ThingSpeak channel number
const char *myWriteAPIKey = "UYA8L4PRE4M0UVHM"; // Replace with your ThingSpeak write API key

DHT11 dht(DHTPIN); // Create DHT11 object

void setup()
{
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0")
  {
    Serial.println("Please upgrade the firmware");
  }

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client); // Initialize ThingSpeak

  pinMode(BUZZER_SIGNAL_PIN, OUTPUT); // Set the buzzer signal pin as output
  pinMode(BUZZER_POWER_PIN, OUTPUT); // Set the buzzer power pin as output
  digitalWrite(BUZZER_POWER_PIN, HIGH); // Turn on the buzzer
}

void loop()
{
  dht.update(); // Update sensor data

  int temperature = dht.readCelsius(); // Read temperature from DHT sensor in Celsius
  int humidity = dht.readHumidity();   // Read humidity from DHT sensor

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // Write to ThingSpeak
  ThingSpeak.setField(1, temperature); // Set temperature field
  ThingSpeak.setField(2, humidity);    // Set humidity field

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Write to ThingSpeak

  if (x == 200)
  {
    Serial.println("Channel update successful");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code: " + String(x));
  }

  if (temperature > 20)
  {
    // Generate a tone on the buzzer signal pin
    tone(BUZZER_SIGNAL_PIN, 1000); // Play a tone
    delay(1000); // Wait for 1 second
    noTone(BUZZER_SIGNAL_PIN); // Stop the tone
  }

  delay(5000); 
}
