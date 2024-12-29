
#pragma once
#include "SystemStruct.hpp"


// TODO: Move to electrical system like the hydraulic system
struct ElectricalVars {
    E170Systems::Units::Voltage::Volt voltage;
};

struct FlightControlState {
    float demanded_pitch{0.0f};
    float demanded_bank{0.0f};
    float demanded_yaw{0.0f};
    float thrust_lever_1{0.0f};
    float thrust_lever_2{0.0f};
    bool gear_lever{false};
    bool park_brake{false};
    bool spoiler_lever{false};
    // TODO: Impl all flight controls, buttons, switches
};


struct SystemState {
    E170Systems::Variables::Hydraulic::HydraulicVars hydraulic_vars{};
    ElectricalVars electrical_vars{};
    FlightControlState flight_control_state;
};
