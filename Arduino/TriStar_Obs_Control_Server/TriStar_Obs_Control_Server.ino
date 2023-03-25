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
    #include <aREST.h>
    #include <avr/wdt.h>  
    #include <EthernetUdp.h>  
    #include <ArduinoJson.h>
    #include <RTClib.h>
    #include <SPI.h>

  // Sketch files
    #include "Config.h"
    #include "sekrits.h"
    #include "Globals.h"   

  // Create aREST instance
    aREST rest = aREST();
    int roof_command(String command);    // This will eventually be O, C, H for open, close, halt
    
void setup() {

// We seem to be unhappy if the switch isn't up when ethernet tries to come up.
// Since they share a power supply, we wait for the switch to be up and running.

  #ifdef DELAY_FOR_SWITCH
    delay(10000);
  #endif
                    
// Confirm hardware initialization if debugging 
    
  #ifdef DEBUG
    Serial.begin(115200);
      while (!Serial) {
        ; // wait for serial port to connect.
      }
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

// API setup

  // API rest Variables

    rest.variable("shutterState", &shutterState);
      

        
  // Declare functions to be exposed to the API
    rest.function("roof_command", roof_command);


  // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("OBSCON");
    rest.set_name("Observatory_Control");

  
// First polls
  // AllSky AI

    aiJSON = readAllSkyAI(AIHost, AIPath);
    #ifdef DEBUG
      Serial.println(aiJSON["classification"].as<String>());
      Serial.println(aiJSON["confidence"].as<float>());
    #endif

  // Weather
    wxJSON = readWX(wxHost, wxPath);

// Start watchdog
  // wdt_enable(WDTO_30S);
}


void loop() {
  EthernetClient client = server.available();
  rest.handle(client);
}
