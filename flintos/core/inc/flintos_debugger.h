
#ifndef __FLINTOS_DEBUGGER_H
#define __FLINTOS_DEBUGGER_H

#include "flint.h"
#include "flint_system_api.h"

class FosDbg : public FDbg {
private:
    static FosDbg dbgInstance;
    FosDbg(void);
    FosDbg(const FosDbg &) = delete;
    void operator=(const FosDbg &) = delete;

    bool receivedDataHandler(uint8_t *data, uint32_t length);

    ~FosDbg(void);
public:
    static FosDbg *getInstance(void);
    bool sendData(uint8_t *data, uint32_t length);
    void receiveTask(void);
};

#endif /* __FLINTOS_DEBUGGER_H */
