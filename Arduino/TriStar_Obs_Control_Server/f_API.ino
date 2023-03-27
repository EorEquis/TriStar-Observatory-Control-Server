// API Custom functions

  // Calculate Safety
    int calcSafety(DynamicJsonDocument wxJSON, DynamicJsonDocument aiJSON) 
      {
        DateTime now = rtc.now();
        unsigned long reqUTC = now.unixtime();
        unsigned long wxUTC = wxJSON["UTC"].as<unsigned long>();
        unsigned long aiUTC = aiJSON["utc"].as<unsigned long>();
        if(reqUTC - wxUTC > MAX_JSON_AGE || (reqUTC - aiUTC > MAX_JSON_AGE && aiUTC <= reqUTC))
          {
            #ifdef DEBUG
              Serial.println("JSON age too old.  UNSAFE");
              Serial.print("reqUTC is : ");
              Serial.println(reqUTC);
              Serial.print("wxUTC is : ");
              Serial.println(wxUTC);
              Serial.print("reqUTC - wxUTC is ");
              Serial.println(reqUTC - wxUTC);
              Serial.print("aiUTC is : ");
              Serial.println(aiUTC);
              Serial.print("reqUTC - aiUTC is ");
              Serial.println(reqUTC - aiUTC);
            #endif              
            return 0;
          }
        float Temp = wxJSON["Temp"].as<float>();
        float Wsp = wxJSON["Wsp"].as<float>();
        float WGust = wxJSON["WGust"].as<float>();
        float SkyTemp = wxJSON["SkyTemp"].as<float>();
        int RSen = wxJSON["RSen"].as<int>();
        int RSenD = wxJSON["RSenD"].as<int>();
        String classification = aiJSON["classification"].as<String>();
        float confidence = aiJSON["confidence"].as<float>();
        
        if(Temp - SkyTemp <= WX_CLOUD_CLOUDY_DELTA || classification == "heavy_clouds" || classification == "precipitation" || Wsp >= WX_MAX_SAFE_WIND || WGust >= WX_MAX_SAFE_GUST)
          {
            #ifdef DEBUG
              Serial.println("Unsafe criteria met.  UNSAFE");
            #endif
            return 0;
          }

        #ifdef WX_USE_RSEND
          if(RSenD == 1)
            {
            #ifdef DEBUG
              Serial.println("RSenD is 1  UNSAFE");
            #endif
              return 0;
            }
        #endif
        
        return 1;
      }    
  // Move the roof
    int roof_command(String command) 
      {
        return 1;
      }
