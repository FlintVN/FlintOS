#pragma once

namespace flintos {

class DisplayDriver {
public:
    void clear();
    void drawText(const char* text, int x, int y);
};

} // namespace flintos
