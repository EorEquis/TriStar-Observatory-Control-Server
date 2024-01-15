// SMC (Simple Motor Controller) Functions

  // Read an SMC serial byte
    int readSMCByte()
      {
        char c;
        if(Serial1.readBytes(&c, 1) == 0){ return -1; }
        return (byte)c;
      } // END readSMCByte()
  
  // Set SMC Motor Limit
    unsigned char setMotorLimit(unsigned char  limitID, unsigned int limitValue)
      {
        Serial1.write(0xA2);
        Serial1.write(limitID);
        Serial1.write(limitValue & 0x7F);
        Serial1.write(limitValue >> 7);
        return readSMCByte();
      } // END setMotorLimit()
  
  // Set SMC Motor Speed
  // Speed should be a number from -3200 to 3200
  
    void setMotorSpeed(int speed)
      {
        if (speed < 0)
        {
          Serial1.write(0x86);  // motor reverse command
          speed = -speed;  // make speed positive
        }
        else
        {
          Serial1.write(0x85);  // motor forward command
        }
        Serial1.write(speed & 0x1F);
        Serial1.write(speed >> 5);
      } // END setMotorSpeed()
  
  // Retrieve value of SMC Variable
    int getSMCVariable(unsigned char variableID)
      {
        Serial1.write(0xA1);
        Serial1.write(variableID);
        return readSMCByte() + 256 * readSMCByte();
      } // END getSMCVariable()
  
  // Resets the SMC
    void resetSMC()
      {
        // must wait at least 1 ms after reset before transmitting
        delay(10);
      
        // Set up motor controller.
        Serial1.write(0xAA);  // send baud-indicator byte
      } // END resetSMC()
