// Initialize RTC
  RTC_DS3231 rtc;

// Initialize the Ethernet server and client
  EthernetServer server(80);
  EthernetClient eth;


// Ethernet UDP Variabls
  unsigned int localPort = 8888;              // local port to listen for UDP packets
  const char timeServer[] = "time.nist.gov";  // time.nist.gov NTP server
  const int NTP_PACKET_SIZE = 48;             // NTP time stamp is in the first 48 bytes of the message
  byte packetBuffer[NTP_PACKET_SIZE];         // buffer to hold incoming and outgoing packets
  unsigned long epoch;  
  EthernetUDP Udp;

// API Variables
  const char wxHost[] = "192.168.50.200";
  const char wxPath[] = "/";
  const char AIHost[] = "allskyai.com";
  const char AIPath[] = "/tfapi/v1/live?url=https://allsky.tristarobservatory.com/image.jpg";

  int shutterState = 0;                       // ASCOM shutterstate.  0=shutterOpen, 1=shutterClosed, 2=shutterOpening, 3=shutterClosing, 4=shutterError
  int isSafe = 0;                             // simple isSafe 1/0 flag.

// JSON Documents to hold responses from sources
  DynamicJsonDocument wxJSON(175);
  DynamicJsonDocument aiJSON(175);    

  

 
