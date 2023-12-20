#include <SoftwareSerial.h>

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

  int shutterState = 1;                       // ASCOM shutterstate.  0=shutterOpen, 1=shutterClosed, 2=shutterOpening, 3=shutterClosing, 4=shutterError
  int isSafe = 0;                             // simple isSafe 1/0 flag.
  int roof_command(String command);    // This will eventually be O, C, H for open, close, halt

// JSON Documents to hold responses from sources
  DynamicJsonDocument wxJSON(195);

//  Setup for roof button
  #ifdef USEBUTTON
    const int buttonPin = 7;      // pin 7 connects to button   : Yellow
    unsigned long lastButton = 0; // Counter for last button press
    int buttonState = 0;          // Current sate of button
    int buttonDelay = 1000;       // Debounce delay
    bool buttonPressed = false; 
  #endif

// Pololu SMC config
  const int rxPin = 3;          // pin 3 connects to SMC TX   : Green
  const int txPin = 4;          // pin 4 connects to SMC RX   : Orange
  const int resetPin = 5;       // pin 5 connects to SMC nRST : Grey
  const int errPin = 6;         // pin 6 connects to SMC ERR  : Blue  
  
// SMC Variable IDs
  #define ERROR_STATUS 0
  #define LIMIT_STATUS 3
  #define TARGET_SPEED 20
  #define SPEED 21

// SMC motor limit IDs
  #define DECELERATION 2 

// Motor State Values
  #define STOPPED 1         // targetSpeed = 0, currentSpeed = 0
  #define MOVING 2          // targetSpeed = currentSpeed
  #define ACCELERATING 3    // targetSpeed > currentSpeed
  #define DECELERATING 4    // targetSpeed < currentSpeed
  #define DIRECTIONOPEN 1   // 1 : Positive motor speed opens roof.  -1 : Negative motor speed opens roof

/** Limit Variable Bits
    Bit 0: Motor is not allowed to run due to an error or safe-start violation.
    Bit 4: Motor speed is not equal to target speed because of acceleration, deceleration, or brake duration limits.
    Bit 7: AN1 limit/kill switch is active (scaled value ≥ 1600).
    Bit 8: AN2 limit/kill switch is active (scaled value ≥ 1600).
**/

  #define ERRORBIT 0
  #define SPEEDMISMATCH 4
  #define OPENLIMIT 8
  #define CLOSEDLIMIT 7    

// ASCOM ShutterState Enumeration : http://www.ascom-standards.org/Help/Developer/html/T_ASCOM_DeviceInterface_ShutterState.htm
  #define SHUTTEROPEN 0
  #define SHUTTERCLOSED 1
  #define SHUTTEROPENING 2
  #define SHUTTERCLOSING 3
  #define SHUTTERERROR 4

// Other variables
  String strRoofCmd;
  unsigned int limitStatus = 0;
  unsigned int errorStatus = 0;
  int currentSpeed = 0;
  int targetSpeed = 0;
  const int motorSpeed = 2800;
  int roofInfoDelay = 1000;
  int motorState = 0;
  bool closedLimitSwitch = false;
  bool openLimitSwitch = false;


// SoftwareSerial for communication w/ SMC
  SoftwareSerial smcSerial = SoftwareSerial(rxPin, txPin);      
