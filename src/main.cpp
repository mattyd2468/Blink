#include "C:\Users\Matty\OneDrive\Documents\PlatformIO\Projects\Blink\.pio\libdeps\esp32dev\DHT sensor library\DHT.h"
#include "WiFi.h"
#include <HTTPClient.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to DHPIN4
// Connect pin 3 (on the right) of the sensor to GROUND
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

//Setup connection to my hotspot
const char *ssid = "Matt";
const char *password = "password1";

const int plantID = 3;
const int tempSensorID = 5;
const int humSensorID = 11;

//Where my API is deployed
String serverName = "http://plantmonitor-env.eba-p2eqtm5y.eu-west-2.elasticbeanstalk.com";

void setup()
{
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void loop()
{
  // Wait a few seconds between measurements.
  delay(15000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  //Read humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //Log temp and humidity
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  //build Readings object for temperature
  //build Readings object for humidity

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http1;
    HTTPClient http2;

    String serverPath = serverName + "/addReading";
    http1.begin(serverPath.c_str());
    http2.begin(serverPath.c_str());
    http1.addHeader("Content-Type", "application/json");
    http2.addHeader("Content-Type", "application/json");

    /////////////////////////POST TEMP/////////////////////////////
    String tempBody = "{\"plantLookupId\": ";
    tempBody += plantID;
    tempBody += " ,	\"sensorLookupId\": ";
    tempBody += tempSensorID;
    tempBody += ",	\"reading\": ";
    tempBody += t;
    tempBody += " }";

    String humBody = "{\"plantLookupId\": ";
    humBody += plantID;
    humBody += " ,	\"sensorLookupId\": ";
    humBody += humSensorID;
    humBody += ",	\"reading\": ";
    humBody += h;
    humBody += " }";

    int httpResponseCodeTemp = http1.POST(tempBody);
    int httpResponseCodeHum = http2.POST(humBody);

    if (httpResponseCodeTemp > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCodeTemp);
      String payload = http1.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Temp error code: ");
      Serial.println(httpResponseCodeTemp);
    }

     if (httpResponseCodeHum > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCodeHum);
      String payload = http2.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Hum error code: ");
      Serial.println(httpResponseCodeHum);
    }

    // Free resources
    http1.end();
    http2.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}