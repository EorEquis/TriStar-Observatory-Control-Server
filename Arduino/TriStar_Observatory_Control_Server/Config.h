/* ---------------------------------------------------------------------------------------------------------------------------------
 * User Modifiable Configuration for Obs Control
 *
 *    * Contains #defines for frequently adjusted variables and settings 
 *
 * ---------------------------------------------------------------------------------------------------------------------------------
*/

/*
VARIABLE                             VALUE           COMMENT                                                              SAFE/UNSAFE
 */


int resetWatchdogEvery = 5000;                  // Number of miliseconds to reset the watchdog, since 8s is WDT max, and we poll less often

// Uncomment these lines to enable features
//#define DEBUG                                   // Debug info to serial monitor
//#define DELAY_FOR_SWITCH                      // Delay 10s at start of sketch to wait for network switch to come up
//#define USEBUTTON                             // Uncomment to allow use of manual roof open/close button
