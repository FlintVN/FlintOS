#pragma once

namespace flintos {

class BoardProfile {
public:
    const char* name() const;
    const char* target() const;
    bool hasPsram() const;
    unsigned flashSizeMb() const;
};

} // namespace flintos
