/*
 * This File serves as the entry point to the systems running in a single threaded environment. The use of the debug GUI simply causes too much clutter to put in with the main thread
 */

#pragma once


#include "E170Systems/E170Systems.hpp"

#ifdef EXTERNAL_SYSTEMS
inline void SystemMain() {
    auto HydraulicState = Variables::Hydraulic::HydraulicVars{
            .System1 = Variables::Hydraulic::System1Vars{
                    .ReservoirLevel = 0.0f,
                    .EngineDrivenPumpRPM = 0.0f,
                    .AcMotorPumpState = false,
                    .PreManifoldPressure = 0.0f,
                    .PostManifoldPressure = 0.0f,
                    .LhThrustReverserPosition = 0.0f
            }
    };
    auto system_state = SystemState{.hydraulic_vars = HydraulicState, .electrical_vars = {.voltage = 0.0f}};
    auto *systems = new E170SystemsRoot(system_state);

    systems->Run();

    delete systems;
}
#else
auto HydraulicState = Variables::Hydraulic::HydraulicVars{
    .System1 = Variables::Hydraulic::System1Vars{
        .ReservoirLevel = 0.0f,
        .EngineDrivenPumpRPM = 0.0f,
        .AcMotorPumpState = false,
        .PreManifoldPressure = 0.0f,
        .PostManifoldPressure = 0.0f,
        .LhThrustReverserPosition = 0.0f
}
};
auto system_state = SystemState{.electrical_vars = {.voltage = 0.0f}, .hydraulic_vars = HydraulicState};
inline E170Systems::E170SystemsRoot systems_root(system_state);

inline void SystemsMain() {
    // example global update function call
    float dt = 0.0f;
    systems_root.Update(dt);
}

#endif
