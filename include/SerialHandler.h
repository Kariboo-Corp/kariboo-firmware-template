#ifndef SERIAL_HANDLER_h
#define SERIAL_HANDLER_h

#include <Arduino.h>
#include <Debugger.h>
#include <STM32FreeRTOS.h>
#include <Handler.h>

class SerialHandler : public RtosTask
{
private:
    Debugger *debug;

    ErrorCodes serialReadError      = { 0x20, "Unable to read command from serial port ..." };
    ErrorCodes invalidBoardSN       = { 0x21, "Unable to read board SN from query .."};
    ErrorCodes invalidBoardType     = { 0x22, "Unable to read board type from query .."};

    uint8_t index;
    char* command;

    char* cmd;
    char* param;
    char* arg;

    void setup() override
    {
        this->debug = new Debugger();
        this->debug->init();

        this->debug->addErrorCode(serialReadError);
        this->debug->addErrorCode(invalidBoardSN);
        this->debug->addErrorCode(invalidBoardType);
    }
    
    void loop() override
    {

        if (this->debug->debug->available() > 0)
        {
            index = this->debug->debug->readBytesUntil('\n', command, 128);
            
            if (index == 0)
            {
                this->debug->throwNew(Warning, serialReadError);
                return;
            }

            // exemple de commande : sn get | sn set 0x123

            cmd = (char*)malloc(20 * sizeof(char));
            param = (char*)malloc(20 * sizeof(char));
            arg = (char*)malloc(20 * sizeof(char));

            sscanf(command, "%s %s %s", cmd, param, arg);

            if (cmd == "sn")
            {
                if (param == "get")
                {
                    this->debug->getBoardSerialNumber();
                    
                    if (this->debug->board.boardSn == 0x0000)
                    {
                        this->debug->throwNew(Critical, invalidBoardSN);
                        return;
                    }
                    this->debug->write("[INFO] - %s %s %s -> %x\n", cmd, param, arg, this->debug->board.boardSn);
                } else if (param == "set")
                {
                    uint16_t id = atoi(arg);

                    if (id == 0)
                    {
                        this->debug->throwNew(Critical, invalidBoardSN);
                        return;
                    }

                    if (this->debug->setBoardSerialNumber(id))
                    {
                        this->debug->throwNew(Critical, invalidBoardSN);
                        return;
                    }

                    this->debug->write("[INFO] - Writed SN successfully !");
                }
            } else if (cmd == "type")
            {
                if (param == "get")
                {
                    this->debug->getBoardType();
                    
                    if (this->debug->board.boardType == 0x00)
                    {
                        this->debug->throwNew(Critical, invalidBoardType);
                        return;
                    }
                    this->debug->write("[INFO] - %s %s %s -> %x\n", cmd, param, arg, this->debug->board.boardType);
                } else if (param == "set")
                {
                    uint16_t id = atoi(arg);

                    if (id == 0)
                    {
                        this->debug->throwNew(Critical, invalidBoardType);
                        return;
                    }

                    if (this->debug->setBoardSerialNumber(id))
                    {
                        this->debug->throwNew(Critical, invalidBoardType);
                        return;
                    }

                    this->debug->write("[INFO] - Writed board type successfully !");

                }
            } else if (cmd == "help")
            {
                this->debug->debug->println(F("[HELP] \n { cmd } { param } { arg } \n\t { cmd } : Command - {sn, type} \n\t { param } : Parameter - { set, get } \n\t { arg } : Argument (optional)"));
            }

            free(cmd);
            free(param);
            free(arg);
        }
        

        /**
        *   N'empèche pas le demarrage de la carte.
        * 
        *   TODO: Commandes : 
        *    - Execution d'un diagnotic
        *    - Assignation et lecture de board_sn
        *    - Assignation et lecture board_type
        *    - autres commande specifiques au prgogramme de la carte
        *       - set tare
        *       - set scale units_g_scale
        * 
        *   Ex : get board_type, set board_sn 0x00
        * 
        */ 
    }
};

#endif