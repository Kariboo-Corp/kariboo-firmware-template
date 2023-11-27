#ifndef WATCHDOG_HANDLER_h
#define WATCHDOG_HANDLER_h

#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <Handler.h>
#include <IWatchdog.h>

/** WatchdogHandler
 * 
 * Cette classe permet de créer une tache qui à 
 * pour seul role de reset la carte en cas de freeze
 * complet du programme.
 * 
*/

class WatchdogHandler : public RtosTask
{
public:
    void reset( void )
    {
        delay(this->timeout); // on sleep pendant timout ms pour trigger le watchdog qui va reset la carte
    }

    bool hasRecoverFromReset( void )
    {
        return IWatchdog.isReset(true);
    }
private:
    uint32_t timeout;

    void setup() override
    {
        IWatchdog.begin(this->timeout); // every 3ms
    }
    
    void loop() override
    {
        IWatchdog.reload();
    }
};

#endif