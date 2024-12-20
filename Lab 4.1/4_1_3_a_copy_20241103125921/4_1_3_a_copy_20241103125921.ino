#define LEDC_CHANNEL 0 // Use first of 6 channels of pwm
#define LEDC_RESOLUTION_BITS 14 // set resolution to 14 bits for PWM
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1) // calc max value of PWM
#define LEDC_FREQ_HZ 30 // Example frequency
#define LED_PIN 5 // define pin 5 for LEDOUT
#define POTT_READ 4 // define pin4 for potentiometer

#include <WiFi.h>
#include <WiFiUdp.h>


const char* ssid = "TP-Link_8A8C";       // wifi ssid
const char* password = "12488674";// passsword


IPAddress MyIP(192, 168, 1, 134);     // Static IP address to my ESP
IPAddress TargetIP(192, 168, 1, 138); // Target of Matt's ESP (or jasons, I tried with both and odn't know which i saved)
IPAddress Gateway(192, 168, 1, 1);    // gateway for network
IPAddress Subnet(255, 255, 255, 0);   // Subnet mask

// UDP Configuration
WiFiUDP udp; // create the UDP instnace
const unsigned int ReceiverPort = 4242; //My receiving port
const unsigned int SenderPort = 4200;    //the output port

const int UDP_PACKET_SIZE = 100; //max packet size 100
byte packetBuffer[UDP_PACKET_SIZE]; //buffer to store UDP packets that come in
char udpBuffer[UDP_PACKET_SIZE]; //buffer to store outgoing udp messages


void handleUDPReceiver(); // function to handle incoming udp message
void fncUdpSend(uint32_t value); // function to send pot value over udp
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax); // function to analog PWM write

// Timing for sending UDP packets
const int threshold = 100; // to not overload and constantly send
int lastPotValue = -1;    // ""
unsigned long lastSendTime = 0; //""
const unsigned long sendInterval = 30; // ""

void setup() { // Setup timer and attach timer to a LED pin (no pinMode needed)
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial port to connect
  }

  // Setup LED Control
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS); //attach pwm timer to LEDpin 
  Serial.println("LED Control initialized"); // serial print everything cause debugging

  pinMode(POTT_READ, INPUT); // pott read on input 

  Serial.print("Connecting to ");//debugging print
  Serial.println(ssid);//""
  

  if (!WiFi.config(MyIP, Gateway, Subnet)) { // attempt to configure the static IP
    Serial.println("static IP Failed to configure"); // if it doesn't work, print error message
  }
  
  WiFi.begin(ssid, password); // start the wifi connection

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) { // loop untill wifi connects
    delay(10);
    Serial.print("."); // show when it's done
  }
  Serial.println("\nWiFi connected"); // confirmation when it works
  Serial.print("IP Address: "); // print hte ip adress
  Serial.println(WiFi.localIP()); //""

  // Start UDP
  udp.begin(ReceiverPort); // start listening for incoming UDP packets on the specific part
  Serial.print("UDP Receiver started on port ");
  Serial.println(ReceiverPort); //confirm the UDP listener is setup on the port
}

void loop() {
  unsigned long currentMillis = millis(); // get time in milliseconds
  int potValue = analogRead(POTT_READ); // Read pottentiometer data
  if (abs(potValue - lastPotValue) > threshold && (currentMillis - lastSendTime > sendInterval)) { // if the potentiometer has changed enough and if the interval has passed
    uint32_t mappedValue = map(potValue, 0, 4095, 0, LEDC_RESOLUTION); // map the pott value to the LEDC_Resolution

    fncUdpSend(mappedValue); // send thi value over UDP
    lastSendTime = currentMillis; // set the time for next time
    lastPotValue = potValue; // set pot value to last for checking if changed
  }

  handleUDPReceiver(); // see if i also get incoming udp packets

  delay(10); // Short delay to prevent overwhelm
}

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax) { // fucntion to write PWM values to LED
  uint32_t duty = LEDC_RESOLUTION * min((uint32_t)value, (uint32_t)valueMax) / valueMax; // Calculate  duty cycle as calc of max value
  ledcWrite(LED_PIN, duty); // Write duty to LEDC
}

void handleUDPReceiver() { // Receive incoming UDP
  int packetSize = udp.parsePacket(); //check if the packet is available
  if (packetSize) { // if there is 
    int len = udp.read(packetBuffer, UDP_PACKET_SIZE - 1); // read the packet into the buffer
    if (len > 0) { // if there was any data was read into packetbuffer
      packetBuffer[len] = '\0'; // adds 0 to end of data in packetbuffer
    }
    Serial.printf("Received UDP packet: %s\n", packetBuffer); // print it in a nice line

    int receivedValue = atoi((char*)packetBuffer); // convert string to an integer
    Serial.print("Parsed Value: ");
    Serial.println(receivedValue); // print received value
    ledcAnalogWrite(LEDC_CHANNEL, receivedValue, 16384); // use the received value to write to the LED
    Serial.print("LED Duty Cycle Set To: ");
    Serial.println(receivedValue); // print for debugging
  }
}

void fncUdpSend(uint32_t value) { // Send the potentiometer value via UDP

  snprintf(udpBuffer, UDP_PACKET_SIZE, "%lu", value); // value of the potentiometer being fomratted as string
  
  udp.beginPacket(TargetIP, SenderPort); // set target ip as sender port
  udp.write((uint8_t*)udpBuffer, strlen(udpBuffer));// write the string to the buffer
  udp.endPacket();// end the packet
  Serial.print("Sent UDP packet: ");
  Serial.println(udpBuffer); // debugging purposes
}
