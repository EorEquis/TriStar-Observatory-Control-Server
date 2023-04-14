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
unsigned long lastAI = millis();
unsigned long lastWX = millis();
unsigned long lastCalcSafe = millis();
unsigned long lastWDT = millis();
float elapsedMillisAI = 0;
float elapsedMillisWX = 0;
float elapsedMillisWDT = 0;

int pollAIEvery = 10000;                        // Number of milliseconds to wait between polls of the AllSkyAI
int pollWXEvery = 10000;                        // Number of milliseconds to wait between polls of the Weather Station
int calcSafeEvery = 10000;                      // Number of milliseconds to wait between calculating the isSafe flag
int resetWatchdogEvery = 5000;                  // Number of miliseconds to reset the watchdog, since 8s is WDT max, and we poll less often

#define WX_CLOUD_CLEAR_DELTA            16      //   If delta between ambient and sky temp >= this value, sky is clear.          SAFE
#define WX_CLOUD_CLOUDY_DELTA           12      //   If delta between ambient and sky temp <= this value, sky is cloudy.       UNSAFE
                                                //   Between these values is "partly cloudy" or "light clouds"                   SAFE
#define WX_RAINSENSOR_WET              910      //   Below this value, the rain sensor is considered wet.                      UNSAFE
                                                //   Frequently changed (lowered), as rain sensor panel degrades
#define WX_MAX_SAFE_WIND                10      //   Above this windspeed (MPH) is UNSAFE                                      UNSAFE
#define WX_MAX_SAFE_GUST                15      //   Wind Gust above this speed (MPH) is UNSAFE                                UNSAFE
#define MAX_JSON_AGE                   300      //   Maximum age (in seconds) of any json source before UNSAFE                 UNSAFE

// Uncomment these lines to enable features
#define DEBUG                                   // Debug info to serial monitor
//#define DELAY_FOR_SWITCH                      // Delay 10s at start of sketch to wait for network switch to come up
#define WX_USE_RSEND                            //   Use rian sensor's digital output?  If defined, then RSEND of 1 is         UNSAFE
