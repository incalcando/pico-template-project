/**
 * gloc (c) by Instruo & Murdo Graham
 * 
 * gloc is licensed under a
 * Creative Commons Attribution 3.0 Unported License.
 * 
 * You should have received a copy of the license along with this
 * work.  If not, see <https://creativecommons.org/licenses/by/3.0/>.
 */

#ifndef LOG_H_
#define LOG_H_

#include "pico/stdlib.h"
#include "log.h"

void log_open() 
{
    stdio_usb_init();

    sleep_ms(2000);

    printf("\n\n===============================\n");
}

void log_test_levels() 
{    
    printf("\n\n===============================\n");
    
    printf("Current log level is set to %d\n", LOG_LEVEL);
    
    LOG_INFO("This is info level.");

    LOG_DEBUG("This is debug level.");

    LOG_ERROR("This is an error.");
}

#endif // LOG_H_