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
      rest.variable("upsTimeUTC", &aiTimeUTC);
          
    // Declare functions to be exposed to the API
      rest.function("roof_command", roof_command);
    
  // First polls
    // Shutter
      #ifdef DEBUG
        Serial.println("First shutter poll");
      #endif
      shutterState = getRoofInfo();
      lastRoof = millis();
      convertDateTime(rtc.now(), roofStatusTime);
    
    // Weather
      #ifdef DEBUG
        Serial.println("First weather poll");
      #endif
      startWxJSONRequest(wxHost, wxPath);
      lastWX = millis();
      while (wxJSON.isNull()) {
        delay(500);
        wxJSON = processWxJSONResponse();
      }
      convertDateTime(rtc.now(), wxTimeUTC);
      
    // AI
      #ifdef DEBUG
        Serial.println("First AI poll");
      #endif
      startAIJSONRequest(AIHost, AIPath);
      lastAI = millis();
      while (aiJSON.isNull()) {
        delay(500);
        aiJSON = processAIJSONResponse();
      }
      convertDateTime(rtc.now(), aiTimeUTC);

    // UPS
      #ifdef DEBUG
        Serial.println("First UPS poll");
      #endif
      startUpsJSONRequest(upsHost, upsPath, upsPort);
      lastUPS = millis();
      while (upsJSON.isNull()) {
        delay(500);
        upsJSON = processUpsJSONResponse();
      }
      convertDateTime(rtc.now(), upsTimeUTC);
        
    // First safety score
      #ifdef DEBUG
        Serial.println("First safety score calc");
      #endif      
      wxUTC = wxJSON["LastWrite_timestamp"].as<unsigned long>();
      aiUTC = aiJSON["utc"].as<unsigned long>();
      upsUTC = upsJSON["currentTimeUTC"].as<unsigned long>();
      
      safetyScore = checkJSONage(wxUTC) + checkJSONage(aiUTC) + checkJSONage(upsUTC)
                  + (wxJSON["CloudCondition"].as<unsigned long>() - 1)
                  + (wxJSON["WindCondition"].as<unsigned long>() - 1)
                  + (upsJSON["powerFlag"].as<unsigned long>() - 1)
                  + (upsJSON["commFlag"].as<unsigned long>() - 1)
                  + getClassificationScore(aiJSON["classification"]);
 
//   Start watchdog
    wdt_disable();        // Disable the watchdog and wait for more than 2 seconds
    delay(3000);          // Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration
    wdt_enable(WDTO_8S);  // Enable the watchdog with a timeout of 8 seconds
} // end setup()


void loop() {
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
 
        lastRoof = millis();
        convertDateTime(rtc.now(), roofStatusTime);        
      }   // end if millis


  // Start JSON requests if needed
    if (!wxRequest.requestInProgress && millis() - lastWX >= pollWXEvery) {
      #ifdef DEBUG
        Serial.println("Starting wx request");
      #endif
      startWxJSONRequest(wxHost, wxPath);
      lastWX = millis();
    }

    if (!aiRequest.requestInProgress && millis() - lastAI >= pollAIEvery) {
        #ifdef DEBUG
          Serial.println("Starting ai request");
        #endif        
        startAIJSONRequest(AIHost, AIPath);
        lastAI = millis();
    }

    if (!upsRequest.requestInProgress && millis() - lastUPS >= pollUPSEvery) {
        #ifdef DEBUG
          Serial.println("Starting UPS request");
        #endif        
        startUpsJSONRequest(upsHost, upsPath, upsPort);
        lastUPS = millis();
    }
    
  // Process JSON responses
    if (wxRequest.requestInProgress) {
      wxJSON = processWxJSONResponse();
      wxUTC = wxJSON["LastWrite_timestamp"].as<unsigned long>();
      convertDateTime(rtc.now(), wxTimeUTC);
    }

    if (aiRequest.requestInProgress) {
      aiJSON = processAIJSONResponse();
      aiUTC = aiJSON["utc"].as<unsigned long>();
      convertDateTime(rtc.now(), aiTimeUTC);
    }

    if (upsRequest.requestInProgress) {
      upsJSON = processUpsJSONResponse();
      upsUTC = upsJSON["currentTimeUTC"].as<unsigned long>();
      convertDateTime(rtc.now(), upsTimeUTC);
    }
    
  // Calculate the safety score if needed

    if (calcScore) {
      wxUTC = wxJSON["LastWrite_timestamp"].as<unsigned long>();
      aiUTC = aiJSON["utc"].as<unsigned long>();
      upsUTC = upsJSON["currentTimeUTC"].as<unsigned long>();
      safetyScore = checkJSONage(wxUTC) + checkJSONage(aiUTC) + checkJSONage(upsUTC)
            + (wxJSON["CloudCondition"].as<unsigned long>() - 1)
            + (wxJSON["WindCondition"].as<unsigned long>() - 1)
            + (upsJSON["powerFlag"].as<unsigned long>() - 1)
            + (upsJSON["commFlag"].as<unsigned long>() - 1)
            + getClassificationScore(aiJSON["classification"]);
      #ifdef DEBUG
        Serial.print("safetyScore: ");
        Serial.println(safetyScore);
        Serial.print("checkJSONage(wxUTC): ");
        Serial.println(checkJSONage(wxUTC));
        Serial.print("checkJSONage(aiUTC): ");
        Serial.println(checkJSONage(aiUTC));
        Serial.print("checkJSONage(upsUTC): ");
        Serial.println(checkJSONage(upsUTC));
        Serial.print("CloudCondition: ");
        Serial.println(wxJSON["CloudCondition"].as<unsigned long>() - 1);
        Serial.print("WindCondition: ");
        Serial.println(upsJSON["WindCondition"].as<unsigned long>() - 1);                
        Serial.print("powerFlag: ");
        Serial.println(wxJSON["powerFlag"].as<unsigned long>() - 1); 
        Serial.print("commFlag: ");
        Serial.println(upsJSON["commFlag"].as<unsigned long>() - 1);         
        Serial.print("classification: ");
        Serial.println(getClassificationScore(aiJSON["classification"]));
      #endif
      calcScore = false;
    }
    
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
      rest.handle(client);  
    }
  
}
