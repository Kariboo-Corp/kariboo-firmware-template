#ifndef DEBUGGER_h
#define DEBUGGER_h

#include <Arduino.h>
#include <i2c_EEPROM.h>

#define BOARD_TYPE_ADDR 0x00000000
#define BOARD_SN_ADDR   0x00000001
#define EEPROM_WRITE_INDEX_ADDR 0x00000002


#define MIN_MEMORY_INDEX 0x00000005
#define MAX_MEMORY_INDEX 0x00100000

enum BoardTypes {MicroStepper = 0x10, EthernetMotionController = 0x11, LoadCellReader = 0x12};
enum ExceptionTypes {Critical, Warning, Info};

struct ErrorCodes
{
    uint8_t code;
    const char* info;
};

struct Board
{
    uint16_t boardSn;
    BoardTypes boardType;
};

struct Diagnostic 
{
    ;
};

class Debugger
{
private:
    i2c_EEPROM *memory;

    uint32_t eeprom_index = 5; // on ne veut pas overwrite SN et Type si jammais 

    bool isDebugging = false;

    ErrorCodes codes[256];

    bool isTypeValid(uint8_t __board_type);
    uint32_t addrIndexIncrement( void );
public:
    HardwareSerial *debug; 

    Debugger();
    ~Debugger();

    void init( void );
    bool setBoardType(uint8_t __board_type);
    bool setBoardSerialNumber(uint16_t __board_sn);

    void debugEnabled(bool en);
    bool isDebugEnabled( void );

    uint8_t getBoardType( void );
    uint16_t getBoardSerialNumber( void );

    void clearLog( void );
    void dumpLog( void );

    void throwNew(ExceptionTypes __type, ErrorCodes __code);
    void addErrorCode(ErrorCodes __code);

    void write(const char *fmt, ...);

    Board board;

    void executeDiagnotics( void );
};

#endif