/* ---------------------------------------------------------------------------------------------------------------------------------
 * User Modifiable Configuration for Obs Control
 *
 *    * Contains #defines for frequently adjusted variables and settings 
 *
 * ---------------------------------------------------------------------------------------------------------------------------------
*/

int resetWatchdogEvery = 5000;                  // Number of miliseconds to reset the watchdog, since 8s is WDT max, and we poll less often
int pollWXEvery = 10000;                        // Number of milliseconds to wait between polls of the Weather Station 

/*
VARIABLE                             VALUE           COMMENT                                                              SAFE/UNSAFE
*/
#define MAX_JSON_AGE                   300      //   Maximum age (in seconds) of any json source before UNSAFE                 UNSAFE




// Uncomment these lines to enable features
#define DEBUG                                   // Debug info to serial monitor
//#define DELAY_FOR_SWITCH                      // Delay 10s at start of sketch to wait for network switch to come up
//#define USEBUTTON                             // Uncomment to allow use of manual roof open/close button
