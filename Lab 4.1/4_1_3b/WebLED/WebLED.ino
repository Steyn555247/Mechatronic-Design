/*
 * WebLED 
 * simple demonstration of TCP turn rgbLED red and green
 * uses AP and DHCP
 * 
 */
#include <WiFi.h> 

const char* ssid = "webLED";
// no password const char* password = "yourpassword";

const char body[] PROGMEM = R"===(
 <!DOCTYPE html>  
 <html><body>        
 <h1> Turn LED <br>
 <a href="/R"> RED</a>.<br>
 <a href="/G"> GREEN</a>.<br>
 </h1>
 </body></html>  
)===";


WiFiServer server(80);   // port 80 is standard for websites

                          
void setup() {
  Serial.begin(115200);         
  Serial.print("Access point"); Serial.println(ssid);
  WiFi.softAP(ssid);
  WiFi.softAP.Config(Ip, IPAddress(192,168,1,134), IPAdress(255,255,255,0));

  Serial.print(" AP IP address"); Serial.println(Ip);
  server.begin();
}

void loop(){
  WiFiClient client = server.accept(); 
  if (client) {
    String currentLine = "";      // incoming data           
    while (client.connected()) {
      if (client.available()) {   // if bytes to read    
        char c = client.read();   // read a byte,     
        if (c == '\n') {
          if (currentLine.length() == 0) {// if blank line     
            client.print(body);
            break;    // exit while loop                                       
          } else {
            if (currentLine.startsWith("GET /R "))
              neopixelWrite(2,64,0,0); // Red                               
            if (currentLine.startsWith("GET /G "))
              neopixelWrite(2,0,64,0); // Green       
            currentLine = "";   // if new line but not blank, clear data       
          }
        } else if (c != '\r') {  // if anything but a CR                       
          currentLine += c;      // add it to the end                          
        }
      }
    }
    client.stop();  // close the connection                                    
  }
}
