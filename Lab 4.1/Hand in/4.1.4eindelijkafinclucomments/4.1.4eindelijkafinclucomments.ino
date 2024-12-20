/*
 * Web Motor Control with PWM and Direction Integration in AP Mode
 * Controls a DC motor's PWM Frequency, Duty Cycle, and Direction via a Web Interface

 I had very stupid variable names so asked ChatGPT to change variable names to be more clear. It changed the variable names accordingly
 Asked chatGPT to put everything in proper format so it's readable
 used ChatGPT as a general helper ("Debug this code...."), "How do I convert my javascript packages to my C++ code", "How do I convert my string received message to integer" etc.
  I asked ChatGPT many questions and many iterations so listing them all would be impossible

  Mianly used 01-preview and o1-mini models and results were varying from bad to decent
  */

#include <WiFi.h> // include wifi library
#include <WebServer.h> //include webserver library for web request

// Define Pins
#define MOTOR_PWM_CHANNEL        0             // LEDC channel for motor PWM
#define MOTOR_PWM_PIN            5             // motor PWM signal
#define MOTOR_PWM_RESOLUTION_BITS 8           // resolution set to 8 so it works for 1000hz
#define MOTOR_PWM_RESOLUTION     ((1 << MOTOR_PWM_RESOLUTION_BITS) - 1)
#define MOTOR_PWM_FREQ_HZ        1000          // Initial PWM frequency in Hz (adjust as needed)
#define DC_FORWARD_PIN           6             // GPIO pin for forward direction
#define DC_BACKWARD_PIN          7             // GPIO pin for reverse direction


const char* ssid     = "Steynswifi";     // my wifi
const char* password = "hihahohe";       // still my password don't steal it please
IPAddress local_IP(192, 168, 1, 134);        // my own IP for AP
IPAddress gateway(192, 168, 1, 134);         // gateway
IPAddress subnet(255, 255, 255, 0);          // mask
WebServer server(80);                        //port80 for webserver 

void handleRoot();
void handleSetDutyCycle();
void handleSetDirection();
void ledcAnalogWriteCustom(uint8_t channel, uint32_t value);

float currentFrequency = MOTOR_PWM_FREQ_HZ;  //set initial frequency using the frequency defined
int currentDutyCycle = 0;                     // set initial duty cycle to 0
int motorDirection = 0;                       // it is said to be stopped in the beginning

void setup() { 
  Serial.begin(115200); //set serial comm
  while (!Serial) {
    ; //wait for serial port to connect so we can always check and debug
  }

  pinMode(DC_FORWARD_PIN, OUTPUT); // different pins set for H bridge
  pinMode(DC_BACKWARD_PIN, OUTPUT); // backward pin set for H bridge
  
  digitalWrite(DC_FORWARD_PIN, LOW); // needs to be stoped in beginning for stop
  digitalWrite(DC_BACKWARD_PIN, LOW); // it needs to be stopped in beginngin for stop
//PWM setup for motor
  ledcAttach(MOTOR_PWM_PIN, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RESOLUTION_BITS); // attach LEDC to pin for PWM
  ledcAnalogWriteCustom(MOTOR_PWM_CHANNEL, currentDutyCycle); //it needs to be low PWM in beginning
  Serial.println("Motor control on"); // debugging
  Serial.println("setting up AP"); //debugging settuing upAP

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP Config Failed"); // if it fails, say this
  }
  if (WiFi.softAP(ssid, password)) { // if the AP has started
    Serial.println("Access Point Started");
    Serial.print("AP SSID: ");
    Serial.println(ssid);
    Serial.print("AP Password: ");
    Serial.println(password);
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP()); // print all above to show that it works and what the IP adress is
  } else {
    Serial.println("AP not started"); //show AP not started
      }

  server.on("/", handleRoot); // setup the root for URL
  server.on("/setDutyCycle", handleSetDutyCycle); // setupt the route for duty cycle
  server.on("/setDirection", handleSetDirection); // setup direction route

  // Start the server
  server.begin(); // begin the server
  Serial.println("HTTP server started in AP mode"); // say that it has started so user can go to webpage
}

void loop() {
  server.handleClient(); // this is the check for the request and converting
}

void handleRoot() { // webpage handler
  Serial.println("Root URL accessed");// sorry that i spend to much time on CSS but as an IPD student i atleast need to make it look somewhat good right
//I used chat gpt to write out the format and the style of the webpage. Prompt "This is the html code, add the CSS elements to this code to make the sliders centered ont he webpage. furthermore just simply make it feel good" use to o1-mini model
  String html = "<!DOCTYPE html>\
<html>\
<head>\
  <title>ESP32 Motor Control</title>\
  <meta charset=\"utf-8\">\
  <style>\
    /* Centering body and setting background color */\
    body { text-align: center; font-family: Arial, sans-serif; background-color: #f0f0f0; }\
    h1 { color: #333; }\
    /* Container styling for sliders */\
    .slider-container { width: 50%; margin: auto; margin-bottom: 30px; }\
    /* Styling for the slider */\
    .slider { -webkit-appearance: none; width: 100%; height: 25px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; border-radius: 10px; }\
    .slider:hover { opacity: 1; }\
    /* Styling for the slider thumb */\
    .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #4CAF50; cursor: pointer; border-radius: 50%; }\
    .slider::-moz-range-thumb { width: 25px; height: 25px; background: #4CAF50; cursor: pointer; border-radius: 50%; }\
    /* Display for frequency and duty cycle values */\
    #valueFrequency, #valueDutyCycle { font-size: 24px; margin-top: 10px; }\
    /* Container styling for direction dropdown */\
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
  if (server.hasArg("value")) { // Check if 'value  ' argument is there
    String valueStr = server.arg("value"); // Get the value as a string
    int value = valueStr.toInt(); // Convertthe string to integer

    // Clamp the value to the PWM resolution range
    value = constrain(value, 0, MOTOR_PWM_RESOLUTION); // set value to avoid range of values

    // Update duty cycle
    currentDutyCycle = value; // set currenDutycycle to value
    ledcAnalogWriteCustom(MOTOR_PWM_CHANNEL, currentDutyCycle); // apply new duty cycle to led using LEDanalogwirte
    Serial.printf("Duty Cycle Set To: %d / %d\n", currentDutyCycle, MOTOR_PWM_RESOLUTION); //send it to serial for debugging

    server.send(200, "text/plain", "OK"); // Respond with OK
  } else {
    server.send(400, "text/plain", "Bad Request"); // Respond with error if no value
  }
}

void handleSetDirection() { // function to set motor direction
  if (server.hasArg("value")) { // Check if 'value' argument is present
    String valueStr = server.arg("value"); // Get the value as a string
    int value = valueStr.toInt(); // Convert to integer

    switch (value) { // create a switch case based ont he values received from webpage
      case 1: // if case == forward
        digitalWrite(DC_FORWARD_PIN, HIGH);  // set forward pin to high
        digitalWrite(DC_BACKWARD_PIN, LOW); // backward pin to low
        motorDirection = 1; // tell motor direction =1
        Serial.println("Direction Set To: Forward"); // print that it is going forward
        break;
      case 2: // if case == backwrad
        digitalWrite(DC_FORWARD_PIN, LOW); // set forward low
        digitalWrite(DC_BACKWARD_PIN, HIGH); // set backward to high
        motorDirection = 2; // change motor direction
        Serial.println("Direction Set To: Reverse"); // print it to the console
        break;
      case 0: // if hte case is 0
      default: // this is the default
        digitalWrite(DC_FORWARD_PIN, LOW); // both are low (no direction going)
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
void ledcAnalogWriteCustom(uint8_t channel, uint32_t value) { // No default argument here
  uint32_t duty = (value * MOTOR_PWM_RESOLUTION) / 256; // Calculate duty cycle 256 for max 8 bit
  ledcWrite(MOTOR_PWM_PIN, duty); // Write duty cycle to LEDC channel
  Serial.printf("LEDC Write - Channel: %d, Duty: %d\n", channel, duty);
}
