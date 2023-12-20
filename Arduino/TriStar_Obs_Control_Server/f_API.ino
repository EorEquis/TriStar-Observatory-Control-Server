// API Custom functions

  // Calculate Safety
    int calcSafety(DynamicJsonDocument wxJSON) 
      {
        DateTime now = rtc.now();
        unsigned long reqUTC = now.unixtime();
        unsigned long wxUTC = wxJSON["UTC"].as<unsigned long>();
        if(reqUTC - wxUTC > MAX_JSON_AGE)
          {
            #ifdef DEBUG
              Serial.println("JSON age too old.  UNSAFE");
              Serial.print("reqUTC is : ");
              Serial.println(reqUTC);
              Serial.print("wxUTC is : ");
              Serial.println(wxUTC);
              Serial.print("reqUTC - wxUTC is ");
              Serial.println(reqUTC - wxUTC);
            #endif              
            return 0;
          }
        float Temp = wxJSON["Temp"].as<float>();
        float Wsp = wxJSON["Wsp"].as<float>();
        float WGust = wxJSON["WGust"].as<float>();
        float SkyTemp = wxJSON["SkyTemp"].as<float>();
        int RSen = wxJSON["RSen"].as<int>();
        int RSenD = wxJSON["RSenD"].as<int>();
        
        
        if  (
              Temp - SkyTemp <= WX_CLOUD_CLOUDY_DELTA                                                                             // WX Station Cloudy
              || RSen <= WX_RAINSENSOR_WET                                                                                        // WX rain
              || Wsp >= WX_MAX_SAFE_WIND                                                                                          // Windy
              || WGust >= WX_MAX_SAFE_GUST                                                                                        // Gusty
            )                                                                             
                // Unsafe conditions
                {
                  #ifdef DEBUG
                    Serial.println("Unsafe criteria met.  UNSAFE");
                  #endif
                  return 0;
                }

        // Safe above, check the digital rain sensor if defined to do so
        #ifdef WX_USE_RSEND
          if(RSenD == 1)
            {
            #ifdef DEBUG
              Serial.println("RSenD is 1  UNSAFE");
            #endif
              return 0;
            }
        #endif
        
        // Conditions must be safe
        return 1;
      }    
  // Move the roof
    int roof_command(String command) 
      {
        if(command == "open" || command == "close" || command == "halt" || command == "status")
          {
            return 0;
          }
        else
          {
            return 1;
          }
      }
