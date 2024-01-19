/*
TriStar Obs Control Server

Arduino aREST server that handles weather/safety/roof info

Created : 2023-03-21  Eor
    // Starting with several example programs smashed together.  We'll get there.
Modified : 
          2023-12-19 Eor : Oh look, I'm awake and finally want to do something with this.
              Addition of new mini pc requires networked roof controller.
              Also removing AI safety for now, AllSky is down, and it was never great anyway
          2023-12-19 Eor : Bring in elements of current TriStar Dome firmware
          2023-12-20 Eor : Change serial communication w/ SMC to use Mega's Serial.  Breaking change for Uno
          2023-12-20 Eor : Cleanup unused variables, merge/rename some ino files for more consistent naming, add/clean up some comments
          2024-01-10 Eor : Add timestamp, formatting is balls, will fix it some day (We all know I probably won't)
          2024-01-12 Eor : Stripping out a bunch of stuff for a cleaner "roof control box"
          2024-01-14 Eor : Begin multi-source safety implementation
*/

// Includes
  // Libraries
    #include <Ethernet.h>
    #include <HttpClient.h>
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
  
  // Begin serial to SMC
    Serial1.begin(19200);
  
  // Reset SMC when Arduino starts up
    resetSMC();
    
  // Set up pins
    #ifdef USEBUTTON
      pinMode(buttonPin, INPUT);
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
        Serial.print("Server is at ");
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
  
      rest.variable("requestTimeUTC", &requestTime);
      rest.variable("shutterState", &shutterState);
      rest.variable("safetyScore", &safetyScore);
      rest.variable("roofStatusTimeUTC", &roofStatusTime);    
      rest.variable("wxTimeUTC", &wxTimeUTC);
      rest.variable("aiTimeUTC", &aiTimeUTC);
          
    // Declare functions to be exposed to the API
      rest.function("roof_command", roof_command);
    
  // First polls
    // Set the startup roof values
      shutterState = getRoofInfo();
      lastRoof = millis();
      convertDateTime(rtc.now(), roofStatusTime);
    // Weather
      wxJSON = readJSON(wxHost, wxPath);
      lastWX = millis();
      convertDateTime(rtc.now(), wxTimeUTC);
    // AI
      aiJSON = readJSON(AIHost, AIPath);
      lastAI = millis();
      convertDateTime(rtc.now(), aiTimeUTC);
  
    // First safety score
      wxUTC = wxJSON["LastWrite_timestamp"].as<unsigned long>();
      safetyScore = checkJSONage(wxUTC) + checkJSONage(aiUTC) + (wxJSON["CloudCondition"].as<unsigned long>() - 1) + (wxJSON["WindCondition"].as<unsigned long>() - 1) + getClassificationScore(aiJSON["classification"]);  
      #ifdef DEBUG
        Serial.print("safetyScore is ");
        Serial.println(safetyScore);
      #endif    
  
  // Start watchdog
    wdt_disable();        // Disable the watchdog and wait for more than 2 seconds
    delay(3000);          // Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration
    wdt_enable(WDTO_8S);  // Enable the watchdog with a timeout of 8 seconds
} // end setup()


void loop() {
  safetyScore = 0;
  // Get roof info
    if (millis() - lastRoof > roofInfoDelay)
      {
        shutterState=getRoofInfo();
        #ifdef USEBUTTON
          buttonState = digitalRead(buttonPin);
          if (buttonState==HIGH)
            {
              if (millis() - lastButton > buttonDelay)   // debounce button for buttonDelay
                {
                  lastButton=millis();
                  buttonPressed = true;
                  handleButton();                
                }
            }
        #endif
        #ifdef DEBUG
            Serial.print("getRoofInfo() called, shutterState is ");
            Serial.println(shutterState);
            Serial.print("Roof limitStatus : ");
            Serial.println(limitStatus);
            Serial.print("Roof errorStatus : ");
            Serial.println(errorStatus);
            Serial.print("Roof currentRoofSpeed : ");
            Serial.println(currentRoofSpeed);
            Serial.print("Roof targetRoofSpeed : ");
            Serial.println(targetRoofSpeed);            
            Serial.print("Roof Status Time : ");
            Serial.println(roofStatusTime); 
        #endif 
        lastRoof = millis();
        convertDateTime(rtc.now(), roofStatusTime);        
      }   // end if millis

  if (millis() - lastWX >= pollWXEvery)
    {
      wxJSON = readJSON(wxHost, wxPath);
      lastWX = millis();
      convertDateTime(rtc.now(), wxTimeUTC);
      //wxTimeUTC = convertDateTime(rtc.now());
    }

  if (millis() - lastAI >= pollAIEvery)
    {
      aiJSON = readJSON(AIHost, AIPath);
      lastAI = millis();
      convertDateTime(rtc.now(), aiTimeUTC);
      //aiTimeUTC = convertDateTime(rtc.now());
    }

  // Calculate the safety score

    wxUTC = wxJSON["LastWrite_timestamp"].as<unsigned long>();
    aiUTC = aiJSON["utc"].as<unsigned long>();
    safetyScore = checkJSONage(wxUTC) + checkJSONage(aiUTC) + (wxJSON["CloudCondition"].as<unsigned long>() - 1) + (wxJSON["WindCondition"].as<unsigned long>() - 1) + getClassificationScore(aiJSON["classification"]);  
      
  // Pet the dog
    if (millis() - lastWDT >= resetWatchdogEvery)
      {
        wdt_reset();
        lastWDT = millis();      
      }

      
  EthernetClient client = server.available();
  if (client)
    {
      convertDateTime(rtc.now(), requestTime);
      //requestTime = convertDateTime(rtc.now());
      rest.handle(client);  
    }
  
}
