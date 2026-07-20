
#include "flintos.h"
#include "flintos_debugger.h"

FosDbg FosDbg::dbgInstance;

static bool CheckDbgMsgFormat(uint8_t *data, uint32_t length) {
    uint32_t rxLen = (data[1] >> 6) | (data[2] << 2) | (data[3] << 10);
    if(length < 6 || length != rxLen) return false;
    uint16_t crc = data[length - 2] | (data[length - 1] << 8);
    if(crc != Crc16(data, length - 2)) return false;
    return true;
}

FosDbg *FosDbg::getInstance(void) {
    return &dbgInstance;
}

bool FosDbg::receivedDataHandler(uint8_t *data, uint32_t length) {
    DbgCmd cmd = (DbgCmd)(data[1] & 0x3F);
    if(cmd == DBG_CMD_START_DEBUG_SESSION) {
        if(CheckDbgMsgFormat(data, length)) {
            Flint *flint = FlintOS::newFlint();
            if(flint != NULL) setTarget(flint);
        }
    }
    return FDbg::receivedDataHandler(data, length);
}
