#pragma once

#include "../Shared/State.hpp"
#include "System/System.hpp"

class ElectricalSystem final : public System {
public:
    explicit ElectricalSystem(const ElectricalVars &state);

    void Update(float dt) override;
};
