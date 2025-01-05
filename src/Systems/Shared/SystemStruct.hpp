#pragma once

#include "Units/Units.hpp"

using namespace E170Systems::Units::Pressure;
using namespace E170Systems::Units::Volume;
using namespace E170Systems::Units::ElectricCurrent;
using namespace E170Systems::Units::ElectricStorage;
using namespace E170Systems::Units::Power;
using namespace E170Systems::Units::Energy;
using namespace E170Systems::Units::Voltage;
using namespace E170Systems::Units::Resistance;
using namespace E170Systems::Units::Capacitance;


namespace E170Systems::Variables {
    namespace Hydraulic {

        struct System1Vars {
            Liter ReservoirLevel;
            float EngineDrivenPumpRPM;
            bool AcMotorPumpState;
            PSI PreManifoldPressure;
            PSI PostManifoldPressure;
            float LhThrustReverserPosition;
        };

        struct HydraulicVars {
            System1Vars System1;
        };

    } // namespace Hydraulic

    namespace Electrical {}

    namespace Electrical {
        struct ElectricalSystemVars {};
    } // namespace Electrical
} // namespace E170Systems::Variables
