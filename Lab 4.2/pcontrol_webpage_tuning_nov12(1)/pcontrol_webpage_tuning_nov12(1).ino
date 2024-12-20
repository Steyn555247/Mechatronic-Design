//this code is created with the aid of ChatGPT and W3Schools, I also modified part of the html lines from lab 4.1.3b
#define LEDC_CHANNEL 0 // use first of 6 channels, same configurations as 4.1.2
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1<<LEDC_RESOLUTION_BITS)-1)
#include <WiFi.h>
#include <WebServer.h>
//Stan Han
const char* ssid = "Stan"; // SSID for the Access Point
const char* password = "123456789";       // Password for the Access Point

WebServer server(80); // Create a web server on port 80

// Define output pins
int slider1Value = 50; 
int slider2Value = 50;
float slider3Value = 20.0; 
float slider4Value = 20.0;

// Motor control pins for SN754410
const int motorPin1_1 = 1;   // Motor 1 H-Bridge pin 1 (direction)
const int motorPin2_1 = 4;   // Motor 1 H-Bridge pin 2 (direction)
const int motorPWM_1 = 5;    // Motor 1 PWM pin (speed control)

const int motorPin1_2 = 8;   // Motor 2 H-Bridge pin 1 (direction)
const int motorPin2_2 = 9;   // Motor 2 H-Bridge pin 2 (direction)
const int motorPWM_2 = 10;    // Motor 2 PWM pin (speed control)
int pwmValue_1=0;
int pwmValue_2=0;
// the increased amount of duty cycle
int u_1;
int u_2;

// Encoder pins
const int encoderPin_1 = 18;  // Encoder signal A for Motor 1
const int encoderPin_2 = 19;  // Encoder signal A for Motor 2

// Variables for counting encoder pulses
float encoderTicks_1 = 0;  // Encoder pulses for Motor 1
float encoderTicks_2 = 0;  // Encoder pulses for Motor 2
unsigned long previousMillis = 0; // Time for speed calculation
unsigned long currentMillis = 0;
float motorSpeed_1 = 0.0;  // Motor 1 speed in RPM
float motorSpeed_2 = 0.0;  // Motor 2 speed in RPM
unsigned long timeInterval = 250; // Interval to calculate speed (1 second)
float n=1000/timeInterval;

// Proportional control parameters (tune these values as needed)
float Kp_1;  // Proportional gain for Motor 1
float Kp_2;  // Proportional gain for Motor 2


// Proportional control variables
float error_1 = 0.0;
float error_2 = 0.0;

// HTML content
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Stan Han_Lab 4.2_Motors Control</title>
    <script>
        function updateSliders() {
            var slider1 = document.getElementById("slider1").value;
            var slider2 = document.getElementById("slider2").value;
            var slider3 = document.getElementById("slider3").value;
            var slider4 = document.getElementById("slider4").value;
            document.getElementById("value1").innerText = slider1;
            document.getElementById("value2").innerText = slider2;
            document.getElementById("value3").innerText = slider3;
            document.getElementById("value4").innerText = slider4;

            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/update?slider1=" + slider1 + "&slider2=" + slider2 + "&slider3=" + slider3 + "&slider4=" + slider4, true);
            xhr.send();
        }
    </script>
</head>
<body>
    <h1>Group22_Lab 4.2_Motors Control</h1>
    <label for="slider1">Motor1 Speed:</label>
    <input type="range" id="slider1" min="0" max="100" value="50" onchange="updateSliders()">
    <span id="value1">50</span><br>
    
    <label for="slider2">Motor2 Speed:</label>
    <input type="range" id="slider2" min="0" max="100" value="50" onchange="updateSliders()">
    <span id="value2">50</span><br>
    
    <label for="slider3">Kp_1:</label>
    <input type="range" id="slider3" min="0" max="100" value="20" onchange="updateSliders()">
    <span id="value3">20</span><br>
    
    <label for="slider4">Kp_2:</label>
    <input type="range" id="slider4" min="0" max="100" value="20" onchange="updateSliders()">
    <span id="value4">20</span><br>

    <button onclick="fetch('/fwd1/on')">Forward1</button>
    <button onclick="fetch('/bwd1/on')">Backward1</button>
    <button onclick="fetch('/stop1/on')">STOP1</button><br><br>

    <button onclick="fetch('/fwd2/on')">Forward2</button>
    <button onclick="fetch('/bwd2/on')">Backward2</button>
    <button onclick="fetch('/stop2/on')">STOP2</button><br><br>
   
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void handleUpdate()
 {
    if (server.hasArg("slider1")) {
        slider1Value = server.arg("slider1").toInt();
    }//reads the slider value
    if (server.hasArg("slider2")) {
        slider2Value = server.arg("slider2").toInt();
    }//reads the slider value
    if (server.hasArg("slider3")) {
        slider3Value = server.arg("slider3").toInt();
        Serial.print("\nSlider 3:");
        Serial.print(slider3Value);
    }//reads the slider value
    if (server.hasArg("slider4")) {
        slider4Value = server.arg("slider4").toInt();
        Serial.print("\nSlider 4:");
        Serial.print(slider4Value);
    }//reads the slider value
    

    // Print the slider values to the Serial Monitor, debugging purposes
    
    server.send(200, "text/plain", "Values updated");
    
}

void IRAM_ATTR encoderISR_1() {
  encoderTicks_1++;  // Increment encoder tick count on each pulse for Motor 1
}

void IRAM_ATTR encoderISR_2() {
  encoderTicks_2++;  // Increment encoder tick count on each pulse for Motor 2
}

void ledcAnalogWrite1(uint8_t pin, uint32_t value, uint32_t valueMax)
{
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(motorPWM_1, duty); // write duty to LEDC
}

void ledcAnalogWrite2(uint8_t pin, uint32_t value, uint32_t valueMax)
{
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(motorPWM_2, duty); // write duty to LEDC
}

void setup() {
  Serial.begin(460800);
  
  
  // Set up Wi-Fi in AP mode
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started: " + String(ssid));

  // Serve HTML page
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  

  // Start the server
  server.begin();
  // Initialize motor control pins for Motor 1
  pinMode(motorPin1_1, OUTPUT);
  pinMode(motorPin2_1, OUTPUT);
  pinMode(motorPWM_1, OUTPUT);

  // Initialize motor control pins for Motor 2
  pinMode(motorPin1_2, OUTPUT);
  pinMode(motorPin2_2, OUTPUT);
  pinMode(motorPWM_2, OUTPUT);

  // Set up encoder input pins
  pinMode(encoderPin_1, INPUT_PULLUP);
  pinMode(encoderPin_2, INPUT_PULLUP);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderPin_1), encoderISR_1, RISING);  // Trigger on rising edge for Motor 1
  attachInterrupt(digitalPinToInterrupt(encoderPin_2), encoderISR_2, RISING);  // Trigger on rising edge for Motor 2

  // Start serial communication
  ledcAttach(motorPWM_1, 30, LEDC_RESOLUTION_BITS);
  ledcAttach(motorPWM_2, 30, LEDC_RESOLUTION_BITS);

  //motor1 forward
  server.on("/fwd1/on", []() {
    digitalWrite(motorPin1_1,HIGH);
    digitalWrite(motorPin2_1,LOW);
    server.send(200, "text/plain", "LED 1 ON");
  });

  //motor2 forward
  server.on("/fwd2/on", []() {
    digitalWrite(motorPin1_2,HIGH);
    digitalWrite(motorPin2_2,LOW);
    server.send(200, "text/plain", "LED 2 ON");
  });

 //motor1 backward, just need to make the two output values are opposite to that of the forward situation
  server.on("/bwd1/on", []() {
    digitalWrite(motorPin1_1,LOW);
    digitalWrite(motorPin2_1,HIGH);
    server.send(200, "text/plain", "LED 1 OFF");
  });

  //motor2 backward, just need to make the two output values are opposite to that of the forward situation
  server.on("/bwd2/on", []() {
    digitalWrite(motorPin1_2,LOW);
    digitalWrite(motorPin2_2,HIGH);
    server.send(200, "text/plain", "LED 2 OFF");
  });

//motor stop, two outputs are low
  server.on("/stop1/on", []() {
    digitalWrite(motorPin1_1,LOW);
    digitalWrite(motorPin2_1,LOW);
    server.send(200, "text/plain", "LED 1 OFF");
  });

  //motor stop, two outputs are low
  server.on("/stop2/on", []() {
    digitalWrite(motorPin1_2,LOW);
    digitalWrite(motorPin2_2,LOW);
    server.send(200, "text/plain", "LED 2 OFF");
  });
}

void loop() {
  server.handleClient();

  currentMillis = millis();
  Kp_1 = slider3Value/10;
  Kp_2 = slider4Value/10;

  // Calculate speed every 1/4 second (250ms)
  if (currentMillis - previousMillis >= timeInterval) {
    previousMillis = currentMillis;

    // Calculate motor speeds in counts per 1/4 sec
    motorSpeed_1 = (encoderTicks_1*n) ;
    motorSpeed_2 = (encoderTicks_2*n) ;

    // Proportional control for Motor 1
    error_1 = slider1Value - motorSpeed_1;  // Calculate the error for Motor 1
    u_1 = Kp_1 * error_1;  // Apply proportional control and constrain the value
    pwmValue_1+= u_1;
    if(pwmValue_1>100)
    {
      pwmValue_1=100;
      }
    else if(pwmValue_1<0)
    {
      pwmValue_1=0;
    }
    //p_1=constrain(pwmValue_1,0,100);
  

    // Proportional control for Motor 2
    error_2 = slider2Value - motorSpeed_2;  // Calculate the error for Motor 1
    u_2 = Kp_2 * error_2;  // Apply proportional control and constrain the value
    pwmValue_2+= u_2;
    if(pwmValue_2>100)
    {
      pwmValue_2=100;
      }
    else if(pwmValue_2<0)
    {
      pwmValue_2=0;
    }

    // Reset encoder tick counters for the next interval
    encoderTicks_1 = 0;
    encoderTicks_2 = 0;

  }
  // Output in serial monitor
    Serial.print("Motor 1 Speed: ");
    Serial.print(motorSpeed_1);  // counts per interval
    Serial.println(" count per second, ");
    //Serial.print("u_1: ");
    //Serial.print(u_1);  // counts per interval
    Serial.println(" , ");
    Serial.print("pwm_1: ");
    Serial.print(pwmValue_1);  // counts per interval
    Serial.println(" , ");

    /*Serial.print("Motor 2 Speed: ");
    Serial.print(motorSpeed_2);  // counts per interval
    Serial.println(" count per second, ");
    Serial.print("u_2: ");
    Serial.print(u_2);  // counts per interval
    Serial.println(" , ");
    Serial.print("pwm_2: ");
    Serial.print(pwmValue_2);  // counts per interval
    Serial.println(" , ");*/

    /*Serial.print("\nSlider 3:");
    Serial.print(Kp_1);
    Serial.print("\nSlider 4:");
    Serial.print(Kp_2);*/
     // Apply PWM to motors
    ledcAnalogWrite1(LEDC_CHANNEL,pwmValue_1,100);
    ledcAnalogWrite2(LEDC_CHANNEL,pwmValue_2,100);
    delay(10);
  
}


