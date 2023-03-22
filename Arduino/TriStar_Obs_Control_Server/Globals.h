
// Data logger
  const int chipSelect = 4;

// RTC Setup
  RTC_DS3231 rtc;
//  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};  

// Initialize the Ethernet server library
  EthernetServer server(80);
  EthernetClient eth;


// Ethernet UDP Variabls
  unsigned int localPort = 8888;              // local port to listen for UDP packets
  const char timeServer[] = "time.nist.gov";  // time.nist.gov NTP server
  const int NTP_PACKET_SIZE = 48;             // NTP time stamp is in the first 48 bytes of the message
  byte packetBuffer[NTP_PACKET_SIZE];         // buffer to hold incoming and outgoing packets
  unsigned long epoch;  
  EthernetUDP Udp;

// Weather Variables
  unsigned long wxUTC;
  float Temp;
  float Hum;
  float Pres;
  float Wsp;
  float WGust;
  float WDir;
  float RTot;
  float LightSen;
  float SkyTemp;
  float IRAmb;
  float RSen;
  float RSenD;
  const char wxHost[] = "192.168.50.200";
  const char wxPath[] = "/";
  const char AIHost[] = "allskyai.com";
  const char AIPath[] = "/tfapi/v1/live?url=https://allsky.tristarobservatory.com/image.jpg";

  DynamicJsonDocument wxJSON(250);
  DynamicJsonDocument aiJSON(250);    
