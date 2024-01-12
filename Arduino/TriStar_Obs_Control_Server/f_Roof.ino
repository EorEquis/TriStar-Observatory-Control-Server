// Roof functions

  // Check if connected to the SMC
    int checkSMC()
      {
        getRoofInfo();
        if(limitStatus == 65279)
          {
            return 0;
          }
        else
          {
            return 1;
          }
      }
  // Open the roof
    int openRoof()
      {
        if (shutterState == SHUTTERCLOSED)
          {
            setMotorSpeed(roofMotorSpeed);
            shutterState = SHUTTEROPENING;
            lastRoof = millis();
            return 0;               // Success, opening roof
            //return SHUTTEROPENING;
          }
          else
          {
            lastRoof = millis();
            return 1;               // Error, roof is not closed, cannot be opened.
            //return SHUTTERERROR;
          }
      } // END openRoof()
  
  // Close the roof
    int closeRoof()
      {
        if (shutterState == SHUTTEROPEN)
          {
            setMotorSpeed(-1 * roofMotorSpeed);
            shutterState = SHUTTERCLOSING;
            lastRoof = millis();
            return 0;               // Success, closing roof.
            //return SHUTTERCLOSING;
          }
        else
          {
            lastRoof = millis();
            return 1;               // Error, roof is not open, cannot be closed.
            //return SHUTTERERROR;
          }     
      } // END closeRoof()
  
  // Halt the roof immediately
    void abortRoof()
      {
        // setMotorLimit(DECELERATION, 0);
        setMotorSpeed(0);
        // setMotorLimit(DECELERATION, 3);
      } // END abortRoof()
  
  // Handle a button press
    int handleButton()
      {
        #ifdef USEBUTTON
          if (shutterState == SHUTTEROPENING || shutterState == SHUTTERCLOSING)
            {
              abortRoof();
            }
          else if (shutterState == SHUTTERCLOSED)
            {
              buttonPressed = false;
              Serial.print(openRoof());
              Serial.println("#");    
            }
          else if (shutterState == SHUTTEROPEN)
            {
              buttonPressed = false;
              Serial.print(closeRoof());
              Serial.println("#");        
            }
        #endif
      } // END handleButton()
  
  // Get current roof status and info
    int getRoofInfo()  
    {
      limitStatus = getSMCVariable(LIMIT_STATUS);
      errorStatus = getSMCVariable(ERROR_STATUS);
      currentRoofSpeed = getSMCVariable(SPEED);
      targetRoofSpeed = getSMCVariable(TARGET_SPEED);
      roofStatusTime = convertDateTime(rtc.now());
    
      if (currentRoofSpeed == 0 && currentRoofSpeed == targetRoofSpeed)
        {
          roofMotorState=STOPPED;
        }
    
      if (currentRoofSpeed != 0 && currentRoofSpeed == targetRoofSpeed)
        {
          roofMotorState=MOVING;
        }
    
      if (targetRoofSpeed != 0 && abs(currentRoofSpeed) < abs(targetRoofSpeed))
        {
          roofMotorState=ACCELERATING;
        }
    
      if (targetRoofSpeed != 0 && abs(currentRoofSpeed) > abs(targetRoofSpeed))
        {
          roofMotorState=DECELERATING;
        }
        
      // See https://www.pololu.com/docs/0J77/6.4 for Limit and Error Status flag registers
    
      if (errorStatus != 0)
        {
          return SHUTTERERROR;
        }
        
      else if (bitRead(limitStatus, OPENLIMIT) == 1 && bitRead(limitStatus, CLOSEDLIMIT) == 1)
        {
          return SHUTTERERROR;
        }
        
      else if (roofMotorState == STOPPED && bitRead(limitStatus, OPENLIMIT) == 1)
        {
          return SHUTTEROPEN;
        }
    
      else if (roofMotorState == STOPPED && bitRead(limitStatus, CLOSEDLIMIT) == 1)
        {
            return SHUTTERCLOSED;
        }
    
      else if (targetRoofSpeed * DIRECTIONOPEN > 0 || currentRoofSpeed * DIRECTIONOPEN > 0)
        {
          return SHUTTEROPENING;
        }
      
      else if (targetRoofSpeed * DIRECTIONOPEN < 0 || currentRoofSpeed * DIRECTIONOPEN < 0)
        {
          return SHUTTERCLOSING;
        }
    
      #ifdef USEBUTTON
        else if ((roofMotorState == DECELERATING || roofMotorState == STOPPED) && (bitRead(limitStatus, OPENLIMIT) == 0 && bitRead(limitStatus, CLOSEDLIMIT) == 0) && buttonPressed)
          // Roof is not moving or is decelerating, neither limit switch is active, and the button was pressed.  We have intentionally halted the roof "in between"
          {
            return SHUTTEROPEN;
          }
    
        else if ((roofMotorState == DECELERATING || roofMotorState == STOPPED) && (bitRead(limitStatus, OPENLIMIT) == 0 && bitRead(limitStatus, CLOSEDLIMIT) == 0) && buttonPressed == false)
          // Roof is not moving or is decelerating, neither limit switch is active, and the button was not pressed.  Oh shit.
          {
            return SHUTTERERROR;
          }
      #else
        else if ((roofMotorState == DECELERATING || roofMotorState == STOPPED) && (bitRead(limitStatus, OPENLIMIT) == 0 && bitRead(limitStatus, CLOSEDLIMIT) == 0))
          // Roof is not moving or is decelerating, neither limit switch is active.  Oh shit.
          {
            return SHUTTERERROR;
          }
      #endif
    
    } //end getInfo()    
