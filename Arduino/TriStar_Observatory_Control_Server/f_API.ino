// API Custom functions

  // Move the roof
    int roof_command(String command) 
      {
        if (command == "abort")
          {
            abortRoof();    // No return value, stops motor immediately
          }
        else if(command == "open")
          {
            return openRoof();
          }
        else if(command == "close")
          {
            return closeRoof();
          }
        else if(command == "reset")
          {
            resetSMC();
            shutterState=getRoofInfo();
            lastRoof = millis();
          }          
        else  // Invalid command, return error.
          {
            
            return 1;
          }
      }

    // Convert DateTime to a char array (buffer)
      void convertDateTime(DateTime datetime, char* buffer) {
        snprintf(buffer, 20, "%04d-%02d-%02dT%02d:%02d:%02d",
               datetime.year(), datetime.month(), datetime.day(),
               datetime.hour(), datetime.minute(), datetime.second());
        }

    
    // Pad dates and times with leading 0s
      void padLeadingZero(int number, char* buffer) {
        snprintf(buffer, 3, "%02d", number);
    }

//
//  // Convert DateTime to String
//    String convertDateTime(DateTime datetime)
//      {
//        String str = String(datetime.year(), DEC) + "-" + padLeadingZero(String(datetime.month(), DEC)) + "-" + padLeadingZero(String(datetime.day(), DEC)) + "T" + padLeadingZero(String(datetime.hour(), DEC)) + ":" + padLeadingZero(String(datetime.minute(), DEC)) + ":" + padLeadingZero(String(datetime.second(), DEC));
//        return str;
//      }
//
//  // Pad dates and times with leading 0s
//    String padLeadingZero(String toPad)
//      {
//        toPad = "0" + toPad;
//        toPad = toPad.substring(toPad.length() - 2);
//        return toPad;
//      }

  // Check json age, return 0 if acceptable, 99 if too old, for safety score
    int checkJSONage(unsigned long unixTimeToCheck)
      {
        DateTime now = rtc.now();
        unsigned long reqTime = now.unixtime();
//        #ifdef DEBUG
//          Serial.print("unixTimeToCheck: ");
//          Serial.println(unixTimeToCheck);
//          Serial.print("reqTime: ");
//          Serial.println(reqTime);
//          Serial.print("MAX_JSON_AGE: ");
//          Serial.println(MAX_JSON_AGE);
//          Serial.print("reqTime - unixTimeToCheck: ");
//          Serial.println(reqTime - unixTimeToCheck);
//        #endif
        if(reqTime - unixTimeToCheck > MAX_JSON_AGE)
          {
            return 99;
          }
        else
          {
            return 0;
          }
      }

  // Get the AI clssification score
    int getClassificationScore(const char* classification) {
      for (int i = 0; i < sizeof(classifications) / sizeof(classifications[0]); i++) {
        if (strcmp(classifications[i], classification) == 0) {
          return i;
        }
      }
    }
