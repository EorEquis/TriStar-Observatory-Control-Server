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

  // Create aREST instance : Dunno why this doesn't work in Globals.h but it doesn't
    aREST rest = aREST();
    int roof_command(String command);    // This will eventually be O, C, H for open, close, halt

void setup() {

// We seem to be unhappy if the switch isn't up when ethernet tries to come up.
// Since they may share a power supply, we wait for the switch to be up and running.

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
  #ifdef DEBUG
    Serial.println("RTC Initialized");
  #endif
  
  setRTCTime();

// aREST setup
  // Name & ID
    rest.set_id("OBSCON");
    rest.set_name("Observatory_Control");
    
  // API rest Variables

    rest.variable("shutterState", &shutterState);
    rest.variable("isSafe", &isSafe);
        
  // Declare functions to be exposed to the API
    rest.function("roof_command", roof_command);
  
// First polls
  // AllSky AI
    aiJSON = readJSON(AIHost, AIPath);
    lastAI = millis();
  // Weather
    wxJSON = readJSON(wxHost, wxPath);
    lastWX = millis();
  // Calculate safety
    isSafe = calcSafety(wxJSON, aiJSON);
    
// Start watchdog
  wdt_disable();        // Disable the watchdog and wait for more than 2 seconds
  delay(3000);          // Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration
  wdt_enable(WDTO_8S);  // Enable the watchdog with a timeout of 8 seconds
}


void loop() {

  if (millis() - lastAI >= pollAIEvery)
    {
      aiJSON = readJSON(AIHost, AIPath);
      lastAI = millis();      
    }

  if (millis() - lastWX >= pollWXEvery)
    {
      wxJSON = readJSON(wxHost, wxPath);
      lastWX = millis();      
    }
  // Calculate safety

  if (millis() - lastCalcSafe >= calcSafeEvery)
    {
      isSafe = calcSafety(wxJSON, aiJSON);
      #ifdef DEBUG
        Serial.print("isSafe ");
        Serial.println(isSafe);
      #endif
      lastCalcSafe = millis();      
    }  

  if (millis() - lastWDT >= resetWatchdogEvery)
    {
      wdt_reset();
      lastWDT = millis();      
    }
  // Calculate safety
      
  EthernetClient client = server.available();
  rest.handle(client);
}
