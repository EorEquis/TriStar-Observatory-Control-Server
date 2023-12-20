// NTP functions for managing real time clock

  // Send an NTP request to the time server at the given address
    void sendNTPpacket(const char * address) {
      // set all bytes in the buffer to 0
      memset(packetBuffer, 0, NTP_PACKET_SIZE);
      // Initialize values needed to form NTP request
      // (see URL above for details on the packets)
      packetBuffer[0] = 0b11100011;   // LI, Version, Mode
      packetBuffer[1] = 0;     // Stratum, or type of clock
      packetBuffer[2] = 6;     // Polling Interval
      packetBuffer[3] = 0xEC;  // Peer Clock Precision
      // 8 bytes of zero for Root Delay & Root Dispersion
      packetBuffer[12]  = 49;
      packetBuffer[13]  = 0x4E;
      packetBuffer[14]  = 49;
      packetBuffer[15]  = 52;
    
      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      Udp.beginPacket(address, 123); // NTP requests are to port 123
      Udp.write(packetBuffer, NTP_PACKET_SIZE);
      Udp.endPacket();
    } // END sendNTPpacket()
  
  // Received an NTP packet, read the data from it
    void readNTPpacket(){
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    
        // the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:
    
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        // now convert NTP time into everyday time:
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        epoch = secsSince1900 - seventyYears;
    } // END readNTPpacket()
  
    // set RTC time
  
  // Use NTP to set the RTC time
    void setRTCTime() {
    
        sendNTPpacket(timeServer);
    
        while (!Udp.parsePacket()) {
          ;   // Do nothing until packet is available      
        }
    
        readNTPpacket();
        rtc.adjust(DateTime(epoch));    // Set the time to the epoch we just received from NTP
    } // END setRTCTime()
