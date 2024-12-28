
#pragma once
#include "Shared/SystemStruct.hpp"


// TODO: Move to electrical system like the hydraulic system
struct ElectricalVars {
    E170Systems::Units::Voltage::Volt voltage;
};


struct SystemState {
    E170Systems::Variables::Hydraulic::HydraulicVars hydraulic_vars;
    ElectricalVars electrical_vars;
};
