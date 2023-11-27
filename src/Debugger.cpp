#include <Debugger.h>

Debugger::Debugger()
{
    HardwareSerial serial(PA10, PA9);
    this->debug = &serial;
    this->memory = new i2c_EEPROM(kbits_1024, 1, 256, 0x56);
}

Debugger::~Debugger()
{
}

void Debugger::init( void )
{
    this->debug->begin(115200);
    this->memory->begin();

    eeprom_index = this->memory->read(EEPROM_WRITE_INDEX_ADDR); // à tester que ça renvoie bien le bon index

    if (isDebugging)
    {
        this->debug->print("[BOARD_EEPROM_INDEX_READ] Log started at addr : ");
        this->debug->println(eeprom_index, HEX);
    }
}

void Debugger::write(const char *fmt, ...)
{
    if (isDebugging) {
        va_list ap;
        char buffer[128];

        va_start(ap, fmt);
        vsprintf(buffer, fmt, ap);
        va_end(ap);

        this->debug->print(buffer);
    }
}

bool Debugger::setBoardType(uint8_t __board_type)
{
    if (isTypeValid(__board_type))
    {
        byte err = this->memory->write(BOARD_TYPE_ADDR, __board_type);
        if (err > 0)
        {
            if (isDebugging)
            {
                this->debug->print("[BOARD_TYPE] Board type can not be assigned with error : ");
                this->debug->println(err, HEX);
            }
            return false;
        }
    }

    this->debug->print("[BOARD_TYPE] Board type assigned : ");
    this->debug->println(__board_type, HEX);

    this->board.boardType = (BoardTypes)__board_type;

    return true;
}

uint8_t Debugger::getBoardType( void )
{
    uint8_t board_type = this->memory->read(BOARD_TYPE_ADDR);

    if (isTypeValid(board_type))
    {
        this->board.boardType = (BoardTypes)board_type;
        this->debug->print("[BOARD_TYPE] Board type read from EEPROM : ");
        this->debug->println(board_type, HEX);

        return board_type;
    }

    if (isDebugging)
    {
        this->debug->print("[BOARD_TYPE] Board type unreadable or unassigned : ");
        this->debug->println(0x00, HEX);
    }

    return 0;
}

bool Debugger::isTypeValid(uint8_t __board_type)
{
    uint8_t error = 0;

    switch (__board_type)
    {
    case MicroStepper:
        return true;
        break;
    case EthernetMotionController:
        return true;
        break;
    case LoadCellReader:
        return true;

    default:
        if (isDebugging)
        return false;
        break;
    }

    if (error > 0)
        return false;

    return true;
}

void Debugger::debugEnabled(bool en)
{
    this->isDebugging = en;
}

bool Debugger::isDebugEnabled( void )
{
    return this->isDebugging;
}

bool Debugger::setBoardSerialNumber(uint16_t __board_sn)
{
    byte err = this->memory->write(BOARD_SN_ADDR, __board_sn);

    this->board.boardSn = __board_sn;

    if (err == 0) {
        this->debug->print("[BOARD_SN] Board SN unreadable or unassigned : ");
        this->debug->println(__board_sn, HEX);
        return true;
    }

    return false;
}

uint16_t Debugger::getBoardSerialNumber( void )
{
    uint8_t snHHalf = this->memory->read(BOARD_SN_ADDR);
    uint8_t snLHalf = this->memory->read(BOARD_SN_ADDR + 1);

    this->board.boardSn = snHHalf * 0xFF + snLHalf;

    return snHHalf * 0xFF + snLHalf;
}

void Debugger::throwNew(ExceptionTypes __type, ErrorCodes __code)
{
    switch (__type)
    {
    case Critical:
        this->debug->print("[ERROR] A critical error happened ... Error code : ");
        this->debug->print(__code.code, HEX);
        this->debug->print(" : ");
        this->debug->println(__code.info);
        break;
    case Warning:
        this->debug->print("[WARNING] Something bad happened ... Error code : ");
        this->debug->print(__code.code, HEX);
        this->debug->print(" : ");
        this->debug->println(__code.info);
        break;
    case Info:
        this->debug->print("[INFO] OK code : ");
        this->debug->print(__code.code, HEX);
        this->debug->print(" : ");
        this->debug->println(__code.info);
        break;
    default:
        break;
    }

    this->memory->write(this->addrIndexIncrement(), __code.code);
    eeprom_index++;
}

uint32_t Debugger::addrIndexIncrement( void )
{
    if (eeprom_index < MIN_MEMORY_INDEX)
    {
        eeprom_index = MIN_MEMORY_INDEX;
    }

    if (eeprom_index + 1 < MAX_MEMORY_INDEX)
    {
        eeprom_index++;
    }

    return eeprom_index;
}

void Debugger::clearLog( void )
{
    for (uint32_t i = MIN_MEMORY_INDEX; i < MAX_MEMORY_INDEX; i++)
    {
        this->memory->write(i, 0x00000000);
    }
    
}

void Debugger::dumpLog( void )
{
    uint32_t addr;
    uint8_t code;

    this->debug->println("[INFO] Start Dumping Logs ...");

    do
    {
        code = this->memory->read(addr);

        this->debug->print("[LOG] - ");
        this->debug->print(addr);
        this->debug->print(" : ");
        this->debug->println(this->codes[code].info);

    } while (code != 0x0);
    
    this->debug->println("[INFO] Logs dumping done.");

}

/**
 * addErrorCode
 * @brief Permet d'ajouter un code d'erreur au Debugger.
 * @param __code contient le code et l'information associé au nouveau code d'erreur
 * @return void
*/

void Debugger::addErrorCode(ErrorCodes __code)
{
    this->codes[__code.code] = { __code.code, __code.info };
}