 /*I had very stupid variable names so asked ChatGPT to change variable names to be more clear. It changed the variable names accordingly
 Asked chatGPT to put everything in proper format so it's readable
 used ChatGPT as a general helper ("Debug this code...."), "How do I convert my javascript packages to my C++ code", "How do I convert my string received message to integer" etc.
  I asked ChatGPT many questions and many iterations so listing them all would be impossible

  Mianly used 01-preview and o1-mini models and results were varying from bad to decent
  */

#define LEDC_CHANNEL 0 // set PWM channel to 0
#define LEDC_RESOLUTION_BITS 14 // PWM set to 14bits (since 3Hz) for 16384 brightness level
#define LEDC_RESOLUTION ((1<<LEDC_RESOLUTION_BITS)-1)// calculate max pwm based on resolution
#define LEDC_FREQ_HZ 30 // set pwm to 30hz
#define LED_PIN 5 // define pin 5 as output
#define POTT_READ 4 // potentiometer input on pin 4

// Function to write an analog value to an LED using PWM on a specified channel and resolution
void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 16384) {
uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax; // calc duty cyclbe on input and max value
ledcWrite(LED_PIN, duty); // write duty to LED pin
}

void setup() { // Setup timer and attach timer to a led pin 
ledcAttach(LED_PIN, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS); // attach LEDPIN to frequency and resolution bits
pinMode(4, INPUT); //set POTTread on input (bit 4)
Serial.begin(115200); //communication serial begin

}

void loop() {
static int brightness = 0; // how bright the LED is in the beginning
int mv; // set up variable for reading pott input and mapp it for duty cycle

mv = map(analogRead(POTT_READ), 0, 4095, 0, 16384); // map potentiometer 0-4095 to 16384 
Serial.println(mv); // print htat value for debugging


ledcAnalogWrite(LEDC_CHANNEL, mv); // write the duty cycle calculated with mv to the analog write funciton

}