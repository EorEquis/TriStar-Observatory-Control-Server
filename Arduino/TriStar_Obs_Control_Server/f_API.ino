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

  // Convert DateTime to String
    String convertDateTime(DateTime datetime)
      {
        String str = String(datetime.year(), DEC) + "-" + padLeadingZero(String(datetime.month(), DEC)) + "-" + padLeadingZero(String(datetime.day(), DEC)) + "T" + padLeadingZero(String(datetime.hour(), DEC)) + ":" + padLeadingZero(String(datetime.minute(), DEC)) + ":" + padLeadingZero(String(datetime.second(), DEC));
        return str;
      }

  // Pad dates and times with leading 0s
    String padLeadingZero(String toPad)
      {
        toPad = "0" + toPad;
        toPad = toPad.substring(toPad.length() - 2);
        return toPad;
      }
