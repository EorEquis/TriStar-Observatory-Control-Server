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
