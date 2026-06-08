
#include <stdio.h>
#include "flintos.h"
#include "esp_board.h"

extern "C" void app_main() {
    Board_Init();
    return FlintOS::main();
}
