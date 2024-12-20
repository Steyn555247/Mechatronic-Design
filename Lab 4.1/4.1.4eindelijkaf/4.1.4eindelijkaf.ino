/*
 * Web Motor Control with PWM and Direction Integration in AP Mode
 * Controls a DC motor's PWM Frequency, Duty Cycle, and Direction via a Web Interface
 */

#include <WiFi.h>
#include <WebServer.h>

// Define Pins
#define MOTOR_PWM_CHANNEL        0             // LEDC channel for motor PWM
#define MOTOR_PWM_PIN            5             // GPIO pin for PWM signal
#define MOTOR_PWM_RESOLUTION_BITS 8           // PWM resolution
#define MOTOR_PWM_RESOLUTION     ((1 << MOTOR_PWM_RESOLUTION_BITS) - 1)
#define MOTOR_PWM_FREQ_HZ        1000          // Initial PWM frequency in Hz (adjust as needed)
#define DC_FORWARD_PIN           6             // GPIO pin for forward direction
#define DC_BACKWARD_PIN          7             // GPIO pin for reverse direction

// Wi-Fi Credentials for AP Mode
const char* ssid     = "Steynswifi";     // AP SSID
const char* password = "hihahohe";       // AP Password (minimum 8 characters)

// Static IP Configuration for AP (Optional)
IPAddress local_IP(192, 168, 1, 134);        // Static IP address for AP
IPAddress gateway(192, 168, 1, 134);         // Gateway (same as AP IP)
IPAddress subnet(255, 255, 255, 0);          // Subnet mask

// Web Server Configuration
WebServer server(80);                        // Create a web server object that listens on port 80

// Function Prototypes
void handleRoot();
void handleSetDutyCycle();
void handleSetDirection();
void ledcAnalogWriteCustom(uint8_t channel, uint32_t value);

// Variables to store current PWM settings and direction
float currentFrequency = MOTOR_PWM_FREQ_HZ;  // Initial frequency in Hz
int currentDutyCycle = 0;                     // Initial duty cycle (0 to MOTOR_PWM_RESOLUTION)
int motorDirection = 0;                       // 0 = Stop, 1 = Forward, 2 = Reverse

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial port to connect. Needed for native USB
  }

  // Setup Direction Control Pins
  pinMode(DC_FORWARD_PIN, OUTPUT);
  pinMode(DC_BACKWARD_PIN, OUTPUT);
  
  // Initialize Direction to Stop
  digitalWrite(DC_FORWARD_PIN, LOW);
  digitalWrite(DC_BACKWARD_PIN, LOW);

  // Setup PWM for Motor
  ledcAttach(MOTOR_PWM_PIN, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RESOLUTION_BITS);
  ledcAnalogWriteCustom(MOTOR_PWM_CHANNEL, currentDutyCycle); // Initialize motor to stop
  Serial.println("Motor Control initialized");

  // Connect as Access Point
  Serial.println("Setting up Access Point...");

  // Configure static IP for AP
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP Config Failed");
  }

  // Start AP
  if (WiFi.softAP(ssid, password)) {
    Serial.println("Access Point Started");
    Serial.print("AP SSID: ");
    Serial.println(ssid);
    Serial.print("AP Password: ");
    Serial.println(password);
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to start Access Point!");
    // Optionally, you can attempt to restart or enter an error state
  }

  // Setup Web Server Routes
  server.on("/", handleRoot);
  server.on("/setDutyCycle", handleSetDutyCycle);
  server.on("/setDirection", handleSetDirection);

  // Start the server
  server.begin();
  Serial.println("HTTP server started in AP mode");
}

void loop() {
  // Handle Web Server Clients
  server.handleClient();
}

// Handler for root URL
void handleRoot() {
  Serial.println("Root URL accessed");
  String html = "<!DOCTYPE html>\
  <html>\
  <head>\
    <title>ESP32 Motor Control</title>\
    <meta charset=\"utf-8\">\
    <style>\
      body { text-align: center; font-family: Arial, sans-serif; background-color: #f0f0f0; }\
      h1 { color: #333; }\
      .slider-container { width: 50%; margin: auto; margin-bottom: 30px; }\
      .slider { -webkit-appearance: none; width: 100%; height: 25px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; border-radius: 10px; }\
      .slider:hover { opacity: 1; }\
      .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #4CAF50; cursor: pointer; border-radius: 50%; }\
      .slider::-moz-range-thumb { width: 25px; height: 25px; background: #4CAF50; cursor: pointer; border-radius: 50%; }\
      #valueFrequency, #valueDutyCycle { font-size: 24px; margin-top: 10px; }\
      .direction-container { width: 50%; margin: auto; margin-bottom: 30px; }\
      select { width: 50%; padding: 10px; font-size: 16px; }\
    </style>\
  </head>\
  <body>\
    <h1>ESP32 Web Motor Control</h1>\
    <div class=\"slider-container\">\
      <h2>PWM Frequency (Hz)</h2>\
      <input type=\"range\" min=\"100\" max=\"10000\" step=\"100\" value=\"" + String(currentFrequency, 0) + "\" class=\"slider\" id=\"freqSlider\" onchange=\"updateFrequency(this.value)\">\
      <div id=\"valueFrequency\">Frequency: " + String(currentFrequency, 0) + " Hz</div>\
    </div>\
    <div class=\"slider-container\">\
      <h2>Duty Cycle</h2>\
      <input type=\"range\" min=\"0\" max=\"" + String(MOTOR_PWM_RESOLUTION) + "\" value=\"" + String(currentDutyCycle) + "\" class=\"slider\" id=\"dutySlider\" onchange=\"updateDutyCycle(this.value)\">\
      <div id=\"valueDutyCycle\">Duty Cycle: " + String(currentDutyCycle) + " / " + String(MOTOR_PWM_RESOLUTION) + "</div>\
    </div>\
    <div class=\"direction-container\">\
      <h2>Direction</h2>\
      <select id=\"directionSelect\" onchange=\"updateDirection(this.value)\">\
        <option value=\"0\"" + (motorDirection == 0 ? " selected" : "") + ">Stop</option>\
        <option value=\"1\"" + (motorDirection == 1 ? " selected" : "") + ">Forward</option>\
        <option value=\"2\"" + (motorDirection == 2 ? " selected" : "") + ">Reverse</option>\
      </select>\
    </div>\
    <script>\
      function updateFrequency(value) {\
        document.getElementById('valueFrequency').innerHTML = 'Frequency: ' + value + ' Hz';\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/setFrequency?value=' + value, true);\
        xhr.send();\
      }\
      function updateDutyCycle(value) {\
        document.getElementById('valueDutyCycle').innerHTML = 'Duty Cycle: ' + value + ' / " + String(MOTOR_PWM_RESOLUTION) + "';\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/setDutyCycle?value=' + value, true);\
        xhr.send();\
      }\
      function updateDirection(value) {\
        var directionText = 'Stop';\
        if(value == '1') { directionText = 'Forward'; }\
        if(value == '2') { directionText = 'Reverse'; }\
        console.log('Direction: ' + directionText);\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/setDirection?value=' + value, true);\
        xhr.send();\
      }\
    </script>\
  </body>\
  </html>";

  server.send(200, "text/html", html);
}

/// Handler to set duty cycle
void handleSetDutyCycle() {
  if (server.hasArg("value")) { // Check if 'value' argument is present
    String valueStr = server.arg("value"); // Get the value as a string
    int value = valueStr.toInt(); // Convert to integer

    // Clamp the value to the PWM resolution range
    value = constrain(value, 0, MOTOR_PWM_RESOLUTION);

    // Update duty cycle
    currentDutyCycle = value;
    ledcAnalogWriteCustom(MOTOR_PWM_CHANNEL, currentDutyCycle);
    Serial.printf("Duty Cycle Set To: %d / %d\n", currentDutyCycle, MOTOR_PWM_RESOLUTION);

    server.send(200, "text/plain", "OK"); // Respond with OK
  } else {
    server.send(400, "text/plain", "Bad Request"); // Respond with error if no value
  }
}

// Handler to set motor direction
void handleSetDirection() {
  if (server.hasArg("value")) { // Check if 'value' argument is present
    String valueStr = server.arg("value"); // Get the value as a string
    int value = valueStr.toInt(); // Convert to integer

    // Set direction based on value
    // 0 = Stop, 1 = Forward, 2 = Reverse
    switch (value) {
      case 1: // Forward
        digitalWrite(DC_FORWARD_PIN, HIGH);
        digitalWrite(DC_BACKWARD_PIN, LOW);
        motorDirection = 1;
        Serial.println("Direction Set To: Forward");
        break;
      case 2: // Reverse
        digitalWrite(DC_FORWARD_PIN, LOW);
        digitalWrite(DC_BACKWARD_PIN, HIGH);
        motorDirection = 2;
        Serial.println("Direction Set To: Reverse");
        break;
      case 0: // Stop
      default:
        digitalWrite(DC_FORWARD_PIN, LOW);
        digitalWrite(DC_BACKWARD_PIN, LOW);
        motorDirection = 0;
        Serial.println("Direction Set To: Stop");
        break;
    }

    server.send(200, "text/plain", "OK"); // Respond with OK
  } else {
    server.send(400, "text/plain", "Bad Request"); // Respond with error if no value
  }
}
////////////////// Custom LEDC Analog Write Function
void ledcAnalogWriteCustom(uint8_t channel, uint32_t value) { // No default argument here
  uint32_t duty = (value * MOTOR_PWM_RESOLUTION) / 256; // Calculate duty cycle
  ledcWrite(MOTOR_PWM_PIN, duty); // Write duty cycle to LEDC channel
  Serial.printf("LEDC Write - Channel: %d, Duty: %d\n", channel, duty);
}
