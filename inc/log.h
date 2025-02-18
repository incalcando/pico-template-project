// gloc (c) by Instruo & Murdo Graham
// 
// gloc is licensed under a
// Creative Commons Attribution 3.0 Unported License.
// 
// You should have received a copy of the license along with this
// work.  If not, see <https://creativecommons.org/licenses/by/3.0/>.

#include <stdio.h>
#include <stdlib.h>

#define NO_LOG          0x00
#define ERROR_LEVEL     0x01
#define INFO_LEVEL      0x02
#define DEBUG_LEVEL     0x03

#ifndef LOG_LEVEL
#define LOG_LEVEL   DEBUG_LEVEL
#endif

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(...)     { printf("DEBUG: "); printf(__VA_ARGS__); }
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(...)      { printf("INFO: "); printf(__VA_ARGS__); }
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(...)     { printf("ERROR: "); printf(__VA_ARGS__);  while(1); }
#else
#define LOG_ERROR(...)
#endif

/**
 * @brief Open the logging interface via USB Serial
 * 
 */
void log_open();

/**
 * @brief Test the log level functionality
 * 
 */
void log_test_levels();