/*I had very stupid variable names so asked ChatGPT to change variable names to be more clear. It changed the variable names accordingly
 Asked chatGPT to put everything in proper format so it's readable
 used ChatGPT as a general helper ("Debug this code...."), "How do I convert my javascript packages to my C++ code", "How do I convert my string received message to integer" etc.
 I asked ChatGPT many questions and iterations so listing them all would be impossible
 
  Mianly used 01-preview and o1-mini models and results were varying from bad to decent
 */

#include <WiFi.h>
#include <WebServer.h>

#define LEDC_CHANNEL 0 // Use first of channelf or PWM
#define LEDC_RESOLUTION_BITS 14 // 14 bits for resolution (isnce 3 h)
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1) // set max resolution based on the bits
#define LEDC_FREQ_HZ 3 // set initial PWM
#define LED_PIN 5 // output LED
//#define POTT_READ 4 // Pottentiometer read

//Use AP mode since it was way easier
const char* ssid     = "Steynswifi";// AP SSID
const char* password = "hihahohe"; // AP Password please don't steal my password for everything ;-P

IPAddress local_IP(192, 168, 1, 134); // static IP adress AP
IPAddress gateway(192, 168, 1, 134);//Gateway (same as AP IP)
IPAddress subnet(255, 255, 255, 0); //Subnet mask

// Web Server Configuration
WebServer server(80);//Create a web server object that listens on port 80


void handleRoot(); // handle URL
void handleSetFrequency(); // set the frequency and change it
void handleSetDutyCycle(); // same but dutycyle
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax); // function for PWM writing

// Variables to store current frequency and duty cycle
float currentFrequency = 5.0;// Initial frequency in Hz
int currentDutyCycle = 0;// Initial duty cycle (0 to LEDC_RESOLUTION)

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial port to connect
  }

  // Setup LED Control
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS); // attach LEDpin to freq and resolution

  ledcAnalogWrite(LEDC_CHANNEL, currentDutyCycle, LEDC_RESOLUTION); // Initialize LED to off
  Serial.println("LED Control initialized"); // debugging

  Serial.println("Setting up Access Point...");

  // Configure static IP for AP
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) { // attempt static ip configuration
    Serial.println("AP Config Failed"); // if it dodint' work print this
  }

  // Start AP
  if (WiFi.softAP(ssid, password)) { // start AP with SSID and password
    Serial.println("Access Point Started");
    Serial.print("AP SSID: ");
    Serial.println(ssid); //pirnt the SSID of the PA if it works
    Serial.print("AP Password: ");
    Serial.println(password); //print the password
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP()); //print IP adress for the AP
  } else {
    Serial.println("Failed to start Access Point!"); // print error if it didn't work
  }
  server.on("/", handleRoot); // sset the handler for URL Root
  server.on("/setFrequency", handleSetFrequency);// set handler for frequency adjustment
  server.on("/setDutyCycle", handleSetDutyCycle); // set it for duty cycle adjustment

  server.begin(); // start webserver
  Serial.println("HTTP server started in AP mode"); //confirm if it works
}

void loop() {
  server.handleClient(); //check for requests
}

// Handler for root URL
void handleRoot() { // sorry that i spend to much time on CSS but as an IPD student i atleast need to make it look somewhat good right
//I used chat gpt to write out the format and the style of the webpage. Prompt "This is the html code, add the CSS elements to this code to make the sliders centered ont he webpage. furthermore just simply make it feel good" use to o1-mini model
  Serial.println("Root URL accessed");
String html = "<!DOCTYPE html>\
<html>\
<head>\
  <title>ESP32 LED Control</title>\
  <meta charset=\"utf-8\">\
  <style>\
    body { text-align: center; font-family: Arial, sans-serif; background-color: #f0f0f0; }\
    h1 { color: #333; }\
    .slider-container { width: 50%; margin: auto; }\
    .slider { -webkit-appearance: none; width: 100%; height: 25px; background: #d3d3d3; outline: none; opacity: 0.7; transition: opacity .2s; border-radius: 10px; }\
    .slider:hover { opacity: 1; }\
    .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #2835c7; cursor: pointer; border-radius: 50%; }\
    .slider::-moz-range-thumb { width: 25px; height: 25px; background: #4CAF50; cursor: pointer; border-radius: 50%; }\
    #valueFrequency, #valueDutyCycle { font-size: 24px; margin-top: 10px; }\
  </style>\
</head>\
<body>\
  <h1>ESP32 Web LED Control</h1>\
  <div class=\"slider-container\">\
    <h2>PWM Frequency (Hz)</h2>\
    <input type=\"range\" min=\"3\" max=\"20\" step=\"0.1\" value=\"" + String(currentFrequency, 1) + "\" class=\"slider\" id=\"freqSlider\" onchange=\"updateFrequency(this.value)\">\
    <div id=\"valueFrequency\">Frequency: " + String(currentFrequency, 1) + " Hz</div>\
  </div>\
  <div class=\"slider-container\">\
    <h2>Duty Cycle</h2>\
    <input type=\"range\" min=\"0\" max=\"" + String(LEDC_RESOLUTION) + "\" value=\"" + String(currentDutyCycle) + "\" class=\"slider\" id=\"dutySlider\" onchange=\"updateDutyCycle(this.value)\">\
    <div id=\"valueDutyCycle\">Duty Cycle: " + String(currentDutyCycle) + " / " + String(LEDC_RESOLUTION) + "</div>\
  </div>\
  <script>\
    function updateFrequency(value) {\
      document.getElementById('valueFrequency').innerHTML = 'Frequency: ' + value + ' Hz';\
      var xhr = new XMLHttpRequest();\
      xhr.open('GET', '/setFrequency?value=' + value, true);\
      xhr.send();\
    }\
    function updateDutyCycle(value) {\
      document.getElementById('valueDutyCycle').innerHTML = 'Duty Cycle: ' + value + ' / " + String(LEDC_RESOLUTION) + "';\
      var xhr = new XMLHttpRequest();\
      xhr.open('GET', '/setDutyCycle?value=' + value, true);\
      xhr.send();\
    }\
  </script>\
</body>\
</html>";


  server.send(200, "text/html", html);
}

//funtion to set handle frequency
void handleSetFrequency() {
  if (server.hasArg("value")) { //check if value as an arugment is there
    String valueStr = server.arg("value"); // Get the value as a string
    float value = valueStr.toFloat(); //convert string to flaot

    value = constrain(value, 3.0, 20.0); //restrict freq from 3 to 20

    if (abs(value - currentFrequency) >= 0.1) { // Threshold to prevent unneceassry updates
      currentFrequency = value; // set current frequency to that value
      Serial.printf("debugging value is ", value); //debug
      ledcChangeFrequency(LED_PIN, value, 14); // use ledcChangeFrequency to change frequency
      ledcAnalogWrite(LEDC_CHANNEL, currentDutyCycle, LEDC_RESOLUTION); // Re-apply current duty cycle
      Serial.printf("Frequency Set To: %.1f Hz\n", currentFrequency); // print it to serial
    }

    server.send(200, "text/plain", "OK"); // Respond with OK
  } else {
    server.send(400, "text/plain", "Bad Request"); // Respond with error if no value
  }
}

// Handler to set duty cycle
void handleSetDutyCycle() {
  if (server.hasArg("value")) { // Check if 'value' argument is present
    String valueStr = server.arg("value"); // Get the value as a string
    int value = valueStr.toInt(); // Convert this string to integer

    value = constrain(value, 0, LEDC_RESOLUTION); // set the value to avoid out of range values

    currentDutyCycle = value; // set the dutycycle to that value
    ledcAnalogWrite(LEDC_CHANNEL, currentDutyCycle, LEDC_RESOLUTION); // apply the new duty cycle to the LED using pwm
    Serial.printf("Duty Cycle Set To: %d / %d\n", currentDutyCycle, LEDC_RESOLUTION); // set it to serial for debugging

    server.send(200, "text/plain", "OK"); // Respond with OK
  } else {
    server.send(400, "text/plain", "Bad Request"); // Respond with error if no value
  }
}
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax) { // ledcanalogwrite to convert value into dutycycle
  uint32_t duty = (value * LEDC_RESOLUTION) / valueMax; // Calculate duty cycle
  ledcWrite(LED_PIN, duty); // Write duty cycle to LEDC channel
  Serial.printf("LEDC Write - Channel: %d, Duty: %d\n", channel, duty); // serial print for the debugging stuf
}