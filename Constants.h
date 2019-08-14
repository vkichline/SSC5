//  Hardware input pin assignments, baud rates, other hardware settings.

#include <limits.h>             // Supplies LONG_MAX, etc.

#define PIR_INPUT_PIN       D8
#define RADAR_INPUT_PIN     D0

#define BAUD_RATE           115200
#define LOG_LEVEL           LOG_INFO
#define AP_NAME_PREFIX      "SSC5_"
#define INIT_FAILURE_DELAY  60000
