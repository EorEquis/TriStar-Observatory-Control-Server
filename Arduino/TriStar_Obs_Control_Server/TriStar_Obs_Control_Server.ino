/*
TriStar Obs Control Server

Arduino aREST server that handles weather/safety/roof info

Created : 2023-03-21  Eor
  // Starting with several example programs smashed together.  We'll get there.
Modified : 
*/

// Includes
  // Libraries
    #include <Ethernet.h>
    #include <EthernetUdp.h>  
    #include <ArduinoJson.h>
    #include <RTClib.h>
    #include <SD.h>
    #include <SPI.h>

  // Sketch files
    #include "Config.h"
    #include "sekrits.h"
    #include "Globals.h"   


void setup() {

// We seem to be unhappy if the switch isn't up when ethernet tries to come up.
// Since they share a power supply, we wait for the switch to be up and running.

  #ifdef DELAY_FOR_SWITCH
    delay(10000);
  #endif
                    
//  Confirm hardware initialization if debugging 
    
  #ifdef DEBUG
    Serial.begin(9600);
      while (!Serial) {
        ; // wait for serial port to connect.
      }
  #endif

  // see if the card is present and can be initialized:
  #ifdef DEBUG
    Serial.print("Initializing SD card...");
  #endif

  if (!SD.begin(chipSelect)) {
    #ifdef DEBUG
      Serial.println("Card failed, or not present");
    #endif
    // don't do anything more:
    while (1);
  }

  #ifdef DEBUG
    Serial.println("card initialized.");
  #endif


// Ethernet Setup
  // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
      eth.setTimeout(5000);

  // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware || Ethernet.linkStatus() == LinkOFF) {
      #ifdef DEBUG
        Serial.println("Ethernet not present or disconnected");
      #endif        
      // don't do anything more:
      while (1);
    }

  // start the server
    server.begin();
    Udp.begin(localPort);    
    #ifdef DEBUG
      Serial.print("server is at ");
      Serial.println(Ethernet.localIP());
    #endif

// RTC Setup

  if (! rtc.begin()) {
    #ifdef DEBUG
      Serial.println("Couldn't find RTC");
    #endif
    // don't do anything more:
    while (1);
  }
  Serial.println("RTC Initialized");
  
  setRTCTime();
  

  // First AI poll
    aiJSON = readAllSkyAI(AIHost, AIPath);
    #ifdef DEBUG
      Serial.println(aiJSON["classification"].as<String>());
      Serial.println(aiJSON["confidence"].as<float>());
    #endif

}


void loop() {
  // Get the time from RTC
    DateTime now = rtc.now();
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the cyrrent date and time from RTC
            client.print("Unix Time ");
            client.println(now.unixtime());
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }


}
