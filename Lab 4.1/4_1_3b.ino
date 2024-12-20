#define LEDC_CHANNEL 0 // Use first of 6 channels
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define LEDC_FREQ_HZ 30 // Example frequency, adjust as needed
#define LED_PIN 5
#define POTT_READ 4

#include <WiFi.h>
#include <WiFiUdp.h>
#include <html510.h>
#include <Mywebsite.h>
HTML510Server h(80);

// Replace with your network credentials
const char* ssid = "TP-Link_8A8C";       // Insert SSID here
const char* password = "12488674";       // Insert Password here

/*
IPAddress MyIP(192, 168, 1, 134);     // Static IP address for this device
IPAddress TargetIP(192, 168, 1, 162); // Target IP address (other board)
IPAddress Gateway(192, 168, 1, 1);    // Your network's gateway
IPAddress Subnet(255, 255, 255, 0);   // Subnet mask

// UDP Configuration
WiFiUDP udp;
const unsigned int ReceiverPort = 4242; // Port to listen on
const unsigned int SenderPort = 3111;    // Port to send to

const int UDP_PACKET_SIZE = 100; // Allow packets up to 100 bytes
byte packetBuffer[UDP_PACKET_SIZE]; // Buffer to store incoming packets
char udpBuffer[UDP_PACKET_SIZE]; // Buffer for outgoing messages

// Function Prototypes
void handleUDPReceiver();
void fncUdpSend(uint32_t value);
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax);

// Timing for sending UDP packets
const int threshold = 100; // Minimum change to trigger sending
int lastPotValue = -1;    // Initialize with an invalid value
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 30; // 1 second
*/

void setup() { // Setup timer and attach timer to a LED pin (no pinMode needed)
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial port to connect. Needed for native USB
  }

  // Setup LED Control
  ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS); // Retained as per your request
  Serial.println("LED Control initialized");

  // Setup Potentiometer Input
  pinMode(POTT_READ, INPUT);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  serial.print("Use this URL to connect: H")
}

void loop() {
  unsigned long currentMillis = millis();

  // Read potentiometer value first
  int potValue = analogRead(POTT_READ); // Read raw analog value (0-4095 for ESP32)

  // Check if the potValue has changed significantly and if sendInterval has passed
  if (abs(potValue - lastPotValue) > threshold && (currentMillis - lastSendTime > sendInterval)) {
    // Map the potentiometer value to a range (e.g., 0-16383 for LEDC_RESOLUTION=16383)
    uint32_t mappedValue = map(potValue, 0, 4095, 0, LEDC_RESOLUTION);

    // Send the mapped value via UDP
    fncUdpSend(mappedValue);

    // Update timing and lastPotValue
    lastSendTime = currentMillis;
    lastPotValue = potValue;
  }

  // Handle incoming UDP packets
  handleUDPReceiver();

  delay(10); // Short delay to prevent overwhelming the loop
}

////////////////// LEDC Analog Write Function
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax) { // No default argument here
  uint32_t duty = LEDC_RESOLUTION * min((uint32_t)value, (uint32_t)valueMax) / valueMax; // Calculate exact duty cycle based on input
  ledcWrite(channel, duty); // Write duty to LEDC
}

/////////////////// UDP Receiver Function
void handleUDPReceiver() { // Receive and process incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read the packet into packetBuffer
    int len = udp.read(packetBuffer, UDP_PACKET_SIZE - 1);
    if (len > 0) {
      packetBuffer[len] = '\0'; // Null-terminate the string
    }
    Serial.printf("Received UDP packet: %s\n", packetBuffer);

    // Convert the received string to an integer
    int receivedValue = atoi((char*)packetBuffer);
    Serial.print("Parsed Value: ");
    Serial.println(receivedValue);

    // Set LED duty cycle based on received value
    ledcAnalogWrite(LEDC_CHANNEL, receivedValue, 16384); // Assuming valueMax is 16384
    Serial.print("LED Duty Cycle Set To: ");
    Serial.println(receivedValue);
  }
}

//////////////////// UDP Sender Function
void fncUdpSend(uint32_t value) { // Send the potentiometer value
  // Prepare the UDP message as a string
  snprintf(udpBuffer, UDP_PACKET_SIZE, "%lu", value); // Convert unsigned long to string
  
  // Send the UDP packet to TargetIP on SenderPort
  udp.beginPacket(TargetIP, SenderPort);
  udp.write((uint8_t*)udpBuffer, strlen(udpBuffer));
  udp.endPacket();
  Serial.print("Sent UDP packet: ");
  Serial.println(udpBuffer);
}
