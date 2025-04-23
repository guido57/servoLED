#ifndef AUDIO_H
    #define AUDIO_H

    #include "Arduino.h"
    #define _TASK_SLEEP_ON_IDLE_RUN // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass 
    #define _TASK_STATUS_REQUEST    // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
    #define _TASK_WDT_IDS           // Compile with support for wdt control points and task ids
    #define _TASK_PRIORITY          // Support for layered scheduling priority
    #define _TASK_TIMEOUT           // Support for overall task timeout 
    #define _TASK_OO_CALLBACKS

    #include "TaskSchedulerDeclarations.h"
    /* Uncomment to enable printing out nice debug messages. */
    //#define ACS712_DEBUG

    #define DEBUG_PRINTER                                                          \
    Serial /**< Define where debug output will be printed.                       \
            */

    /* Setup debug printing macros. */
    #ifdef AUDIO_DEBUG
        #define DEBUG_PRINT(...)                                                       \
        { DEBUG_PRINTER.print(__VA_ARGS__); }
        #define DEBUG_PRINTLN(...)                                                     \
        { DEBUG_PRINTER.println(__VA_ARGS__); }
    #else
        #define DEBUG_PRINT(...)                                                       \
        {} /**< Debug Print Placeholder if Debug is disabled */
        #define DEBUG_PRINTLN(...)                                                     \
        {} /**< Debug Print Line Placeholder if Debug is disabled */
    #endif

class MyAudio  {
  public:
    ~MyAudio();
    MyAudio();
    float measure(int read_time_msecs);
};



#endif
