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
#define WX_CLOUD_CLEAR_DELTA            16      //   If delta between ambient and sky temp >= this value, sky is clear.          SAFE
#define WX_CLOUD_CLOUDY_DELTA           12      //   If delta between ambient and sky temp <= this value, sky is cloudy.       UNSAFE
                                                //   Between these values is "partly cloudy" or "light clouds"                   SAFE
#define WX_RAINSENSOR_WET              910      //   Below this value, the rain sensor is considered wet.                      UNSAFE
                                                //   Frequently changed (lowered), as rain sensor panel degrades
#define WX_MAX_SAFE_WIND                10      //   Above this windspeed (MPH) is UNSAFE                                      UNSAFE
#define WX_MAX_SAFE_GUST                15      //   Wind Gust above this speed (MPH) is UNSAFE                                UNSAFE
#define WX_USE_RSEND                   YES      //   Use rian sensor's digital output?  If yes, then RSEND of 1 is             UNSAFE

// Uncomment these lines to enable features
#define DEBUG                                   // Debug info to serial monitor
//#define DELAY_FOR_SWITCH                      // Delay 10s at start of sketch to wait for network switch to come up
