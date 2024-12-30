
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

    struct Overhead {
        // NOTE: not simulating cvr
        struct ElectricalPanel {
            int idg1_state{1}; // idgs are 0=auto, 1=off, 2=disc
            int idg2_state{1};

            int ac_bus_ties_state{0}; // 0=auto, 1=bus1 open, 2=bus2 open

            int apu_gen_state{0};
            int gpu_button_state{0};
            int gpu_available{0};
            int gpu_active{0};

            int tru1_state{0}; // 0 off 1 auto
            int tru2_state{0};
            int tru_ess_state{0};

            int batt_1_state{0};
            int batt_2_state{0};

            int dc_bus_ties_state{0};
        } electrical_panel;

        struct CockpitLights {
            // NOTE: no brightness, so only going to grab dome light
            int dome_light{0};
        } cockpit_lights;

        struct FirePanel {
            int eng_1_fire_extinguisher{0}; // 0 off, 1 pulled, 2 shot
            int eng_2_fire_extinguisher{0};
            int fwd_cargo_fire_extinguisher{0}; // 0 off, 1 pressed
            int aft_cargo_fire_extinguisher{0};
            int apu_fire_extinguisher{0};
        } fire_panel;

        struct FuelPanel {
            int xfeed_selector{0}; // 0 off, 1 eng1 , 2 eng2
            int dc_pump{0}; // 0 off, 1 auto, 2 on
            int ac_pump_1{0};
            int ac_pump_2{0};
        } fuel_panel;

        struct CabinPanel {
            int emergency_lights_switch{0}; // 0 off, 1 armed, 2 on
            int sterile_switch{0}; // TODO: Find the power draw of these shits
            int no_smoking_switch{0};
            int seatbelt_switch{0};
        } cabin_panel;

        struct ApuPanel {
            int apu_master_swith{0}; // 0 off, 1 on, 2 start // TODO: momentary switch (or not)
            int emergency_stop_button{0};
        } apu_panel;

        struct WiperPanel {
            int wiper_switch_1{1}; // 0 timer, 1 off, 2 low, 3 high
            int wiper_switch_2{1};
        } wiper_panel;

        struct ExternalLightsPanel {
            int nav_light_switch{0};
            int strobe_light_switch{0};
            int beacon_light_switch{0};
            int logo_light_switch{0};
            int inspection_light_switch{0};
            int tax_nose_light_switch{0};
            int taxi_side_light_switch{0};
            int landing_left_light_switch{0};
            int landing_right_light_switch{0};
            int landing_nose_light_switch{0};
        } external_lights_panel;

        struct HydraulicPanel {
            int ptu{0}; // 0 off, 1 auto , 2 on
            int sys1_pump_shutoff_button{0};
            int sys2_pump_shutoff_button{0};
            int sys1_elec_pump_switch{0}; // 0 off, 1 auto, 2 on
            int sys2_elec_pump_switch{0};

            int sys3_elec_pump_a_switch{0}; // the 2 way one
            int sys3_elec_pump_b_switch{0}; // the 3 way one
        } hydraulic_panel;

        struct PressurizationPanel {
            int manual_cabin_alt_switch{1}; // 0 down, 1 stop, 2 up
            int lfe_cabin_alt_switch{1}; //
            int cabin_alt_mode_switch{1}; // 0 manual, 1 auto, 2 lfe
            int dump_button_switch{0};
        } pressurization_panel;

        struct IcePanel {
            int windshield_heating_1_switch{0};
            int windshield_heating_2_switch{0};

            int ice_protection_eng1_switch{0};
            int ice_protection_eng2_switch{0};
            int ice_protection_wing_switch{0};
            int ice_protection_mode_switch{0}; // 0 auto, 1 on
            int ice_protection_test_switch{0}; // 0 engine, 1 off, 2 wing
        } ice_panel;

        struct PneumaticPanel {
            float cockpit_temp_knob{0.0f};
            float cabin_temp_knob{0.0f};
            int recirc_switch{0};
            int pack1_switch{0};
            int pack2_switch{0};
            int crossbleed_switch{0};
            int bleed1_switch{0};
            int bleed2_switch{0};
            int apu_bleed_switch{0};
        } pneumatic_panel;

        struct PaxOxygenPanel {
            int mask_deploy_switch{0}; // 0 off, 1 auto, 2 override
            int mask_deployed_switch{0};
        } pax_oxygen_panel;
    } overhead_panel;
};


struct SystemState {
    E170Systems::Variables::Hydraulic::HydraulicVars hydraulic_vars{};
    ElectricalVars electrical_vars{};
    FlightControlState flight_control_state{};
};
