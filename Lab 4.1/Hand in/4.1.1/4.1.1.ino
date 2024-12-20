/* I had very stupid variable names so asked ChatGPT to change variable names to be more clear. It changed the variable names accordingly
 Asked chatGPT to put everything in proper format so it's readable
 used ChatGPT as a general helper ("Debug this code...."), "How do I convert my javascript packages to my C++ code", "How do I convert my string received message to integer" etc.
  I asked ChatGPT many questions and many iterations so listing them all would be impossible

  Mianly used 01-preview and o1-mini models and results were varying from bad to decent
  */

#define SWITCH_READ_PIN A4 //GPIO 4 used for switch 
#define LED_OUT 5 // Set PIN 5 as LED out

// #include <Adafruit_NeoPixel.h> // not used

bool buttonState; //button result is boolean
const int threshold = 512; // threshold for switch 

void setup() {
  pinMode(SWITCH_READ_PIN, INPUT); // set gpio 4 as input 
  Serial.begin(115200); // serial communication baud 115200
  pinMode(LED_OUT, OUTPUT); //LED port is output

void loop() {
  int analogValue = analogRead(SWITCH_READ_PIN); //analog value read
  buttonState = analogValue > threshold; // change button state if analog value higher than threshold

  if (buttonState){ // if buttonstate is true/high
    digitalWrite(LED_OUT, HIGH);  /// LED turns on 
  }
  else{ // otherwhise
    digitalWrite(LED_OUT, LOW); //led is low
  }
  
  Serial.print("Analog Value: "); //used for readin ganalog value and possible debugging purposes
  Serial.println(analogValue); // print this

  delay(100); // Add a small delay to stabilize readings
}
