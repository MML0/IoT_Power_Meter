#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PZEM004Tv30.h>
#include <Wire.h>

PZEM004Tv30 pzem(8, 9); // Software Serial pin 8 (RX) & 9 (TX)

#define DEVICE_NAME "device A"

// URLs
const char* logURLBase = "http://mml-dev.ir/?log=";
const char* postURL = "http://10.10.10.10:8090/login.xml";

// POST parameters for intenet login
const char* referer = "http://10.10.10.10:8090/httpclient.html";
const int mode = 191;
const char* username = "0110344685";
const char* userPassword = "Mmlmml123";
const int producttype = 0;

// Backend API URL to log data
const char* LogpostURL = "http://127.0.0.1:3000/backend/data.php";

String collectSensorData(String deviceName) {
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    // Logging data with device name
    Serial.printf("[%s] Voltage: %.2f V, Current: %.2f A, Power: %.2f W\n", deviceName.c_str(), voltage, current, power);
    Serial.printf("[%s] Energy: %.2f kWh, Frequency: %.2f Hz, Power Factor: %.2f\n", deviceName.c_str(), energy, frequency, pf);
    
    // Creating JSON payload
    String jsonData = "{";
    jsonData += "\"name\":\"" + deviceName + "\",";
    jsonData += "\"voltage\":" + String(voltage, 2) + ",";
    jsonData += "\"current\":" + String(current, 2) + ",";
    jsonData += "\"power\":" + String(power, 2) + ",";
    jsonData += "\"energy\":" + String(energy, 2) + ",";
    jsonData += "\"frequency\":" + String(frequency, 2) + ",";
    jsonData += "\"power_factor\":" + String(pf, 2);
    jsonData += "}";

    return jsonData;
}
// Function to generate random sensor data
String generateData(const String& name) {
    float voltage = 220.5 * (1 + (random(-10, 10) / 100.0));   // ±10% variation
    float current = 1.25 * (1 + (random(-2, 2) / 100.0));      // ±2% variation
    float power = 275.6 * (1 + (random(-5, 5) / 100.0));       // ±5% variation
    float energy = 3.5 * (1 + (random(-3, 3) / 100.0));        // ±3% variation
    float frequency = 50.0 * (1 + (random(-1, 1) / 100.0));    // ±1% variation
    float powerFactor = 0.95 * (1 + (random(-5, 5) / 1000.0)); // ±0.5% variation

    String data = "{";
    data += "\"name\":\"" + name + "\",";
    data += "\"voltage\":" + String(voltage, 3) + ",";
    data += "\"current\":" + String(current, 3) + ",";
    data += "\"power\":" + String(power, 3) + ",";
    data += "\"energy\":" + String(energy, 3) + ",";
    data += "\"frequency\":" + String(frequency, 3) + ",";
    data += "\"power_factor\":" + String(powerFactor, 3);
    data += "}";

    return data;
}

// Function to send data to the backend
void sendData(String jsonData) {
    WiFiClient client;
    HTTPClient http;
    
    Serial.println("Sending data: " + jsonData);
    
    http.begin(client, LogpostURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
        Serial.print("Server Response: ");
        Serial.println(http.getString());
    } else {
        Serial.printf("Error in POST request. HTTP Response code: %d\n", httpResponseCode);
    }

    http.end();
}

String performGetRequest() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://mml-dev.ir");  // ✅ Use client + URL

  Serial.println("Performing GET request to mml-dev.ir...");
  int httpResponseCode = http.GET();
  String payload = "";
  if (httpResponseCode > 0) {
    Serial.printf("GET Response code: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.println("Response payload:");
    Serial.println(payload);
  } else {
    Serial.printf("Error in GET request. HTTP Response code: %d\n", httpResponseCode);
  }

  http.end();
  return payload;
}
String performPostRequest() {
  HTTPClient http;
  WiFiClient client;
  http.begin(client, postURL);  // ✅ fixed

  http.addHeader("User-Agent", "ESP8266");
  http.addHeader("Referer", referer);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  unsigned long timestamp = millis() / 1000;

  String postData = "mode=" + String(mode) +
                    "&username=" + String(username) +
                    "&password=" + String(userPassword) +
                    "&a=" + String(timestamp) +
                    "&producttype=" + String(producttype);
  String payload = "";
  int httpResponseCode = http.POST(postData);
  if (httpResponseCode > 0) {
    Serial.printf("POST Response code: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("Error in POST request. HTTP Response code: %d\n", httpResponseCode);
  }

  http.end();
  return payload;
}
bool loginToInternet(int maxRetries = 3, int delayBetweenRetries = 3000) {
  int attempt = 0;
  bool success = false;

  while (attempt < maxRetries && !success) {
    attempt++;
    Serial.printf("🔄 Login attempt %d...\n", attempt);

    WiFiClient client;
    HTTPClient http;
    http.begin(client, postURL);  // Use client as required in ESP8266

    http.addHeader("User-Agent", "ESP8266");
    http.addHeader("Referer", referer);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    unsigned long timestamp = millis() / 1000;

    String postData = "mode=" + String(mode) +
                      "&username=" + String(username) +
                      "&password=" + String(userPassword) +
                      "&a=" + String(timestamp) +
                      "&producttype=" + String(producttype);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      Serial.printf("POST Response code: %d\n", httpResponseCode);
      String payload = http.getString();
      Serial.println("Response payload:");
      Serial.println(payload);

      if (payload.indexOf("<status><![CDATA[LIVE]]></status>") != -1) {
        Serial.println("✅ Login successful.");
        logEvent("Login successful on attempt " + String(attempt));
        success = true;
      } else if (payload.indexOf("LOGIN_FAILED") != -1) {
        Serial.println("❌ Login failed. Credentials may be wrong.");
        logEvent("Login failed: Invalid credentials.");
      } else {
        Serial.println("⚠️ Unexpected login response.");
        logEvent("Login failed: Unexpected response.");
      }

    } else {
      Serial.printf("❌ HTTP error. Code: %d\n", httpResponseCode);
      logEvent("Login failed: HTTP error " + String(httpResponseCode));
    }

    http.end();

    if (!success && attempt < maxRetries) {
      Serial.println("Retrying in 3 seconds...");
      delay(delayBetweenRetries);
    }
  }

  if (!success) {
    Serial.println("❌ All login attempts failed.");
    logEvent("All login attempts failed.");
    delay(1000);
    ESP.restart();
  }
  // return success==true ; // XD
}
bool logEvent(String message) {
  Serial.println("Log: " + message);

  String encodedMessage = "";
  for (int i = 0; i < message.length(); i++) {
    if (isalnum(message[i]) || message[i] == '-' || message[i] == '_') {
      encodedMessage += message[i];
    } else if (message[i] == ' ') {
      encodedMessage += '+';
    } else {
      char hex[4];
      sprintf(hex, "%%%02X", message[i]);
      encodedMessage += hex;
    }
  }

  WiFiClient client;
  HTTPClient http;
  http.begin(client, String(logURLBase) + encodedMessage);  // ✅ fixed


  int httpCode = http.GET();
  String payload = http.getString();
  http.end();

  return payload == "done!!!";
}
void setup() {
  Serial.begin(115200);
  delay(1000);

  while (true) {
    Serial.println("Scanning for WiFi networks...");
    int numNetworks = WiFi.scanNetworks();

    if (numNetworks == 0) {
      Serial.println("No networks found. Retrying in 5 seconds...");
      delay(100);
      continue;
    }

    String targetSSID = "";
    for (int i = 0; i < numNetworks; ++i) {
      String ssid = WiFi.SSID(i);
      String ssidLower = ssid;
      ssidLower.toLowerCase();

      if ((ssidLower.indexOf("khu") != -1 || ssidLower.indexOf("kharazmi") != -1 || ssidLower.indexOf("university") != -1) && WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        targetSSID = ssid;
        break;
      }
    }

    if (targetSSID == "") {
      Serial.println("No open 'khu' or 'kharazmi' network found. Retrying in 5 seconds...");
      delay(100);
      continue;
    }

    Serial.println("Trying to connect to SSID: " + targetSSID);
    WiFi.begin(targetSSID.c_str(), "");

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 30) {
      delay(1000);
      Serial.print(".");
      retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to Wi-Fi: " + targetSSID);
      logEvent("Connected to: " + targetSSID);
      break;
    } else {
      Serial.println("\nFailed to connect. Retrying in 5 seconds...");
      delay(100);
    }
  }

  // Perform POST
  // performPostRequest();
  logEvent("not loged in");

  loginToInternet(4, 1000);  // Try 5 times, wait 4 seconds between tries

  logEvent("test 1 esp 8266");
  delay(4000);
  logEvent("test 2 esp 8266");

  // Perform GET
  performGetRequest();
}

void loop() {
  static unsigned long lastLogTime = 0;

  if (millis() - lastLogTime >= 10000) {
    lastLogTime = millis();

    // Check Wi-Fi connection
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("❌ Lost Wi-Fi connection. Restarting...");
      logEvent("Wi-Fi disconnected. Restarting ESP...");
      delay(2000);
      ESP.restart();
    }

    // Log signal strength
    String currentSSID = WiFi.SSID();
    int rssi = WiFi.RSSI();
    // Serial.printf("📶 Connected to: %s | RSSI: %d dBm\n", currentSSID.c_str(), rssi);
    String msg = "📶 Connected to: " + currentSSID + " | RSSI: " + String(rssi) + " dBm";
    logEvent(msg);

    
    // String jsonData = collectSensorData("DEVICE_A");
    // sendData(jsonData);
    
    String deviceNames[] = {"Device A", "Device B", "Device C"};

    for (int i = 0; i < 3; i++) {
        String jsonData = generateData(deviceNames[i]);
        sendData(jsonData);
        delay(3000); // Small delay between requests
    }

    // delay(1000); // Wait 10s before logging new data
  }
}
