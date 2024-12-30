
#include "VirtualCockpit.hpp"

#include "Backend/Application/Application.hpp"
#include "Backend/Application/Application.hpp"
#include "Backend/Image/Image.hpp"
#include "Components/CircuitBreakerUI/CircuitBreakerUI.hpp"
#include "Components/IlluminatedButton/IlluminatedButton.hpp"
#include "Components/MultiPositionKnob/MultiPositionKnob.hpp"
#include "Components/MultiPositionSwitch/MultiPositionSwitch.hpp"

void VirtualCockpit::RenderImGui() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto image = Ouroboros::ImageStorage::Instance().GetImage("overhead");
    ImGui::SetNextWindowSize({(image->GetWidth() + 50.0f) * 1.3f, ImGui::GetWindowHeight()});
    ImGui::Begin("Virtual Cockpit", NULL, ImGuiWindowFlags_NoMove);
    if (ImGui::Button("FlightControls")) {
        m_Page = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Overhead")) {
        m_Page = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Main Panel")) {
        m_Page = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pedestal")) {
        m_Page = 3;
    }
    ImGui::SameLine();
    if (ImGui::Button("Floor Controls")) {
        m_Page = 4;
    }
    ImGui::SameLine();
    if (ImGui::Button("Circuit Breakers")) {
        m_Page = 5;
    }
    ImGui::SameLine();
    if (ImGui::Button("Oxygen Masks")) {
        m_Page = 6;
    }
    ImGui::SameLine();
    if (ImGui::Button("YokeButtons")) {
        m_Page = 7;
    }

    switch (m_Page) {
        case 0: {
            RenderFlightControlsSliders();
        }
        break;
        case 1: {
            RenderOverheadPanel();
        }
        break;
        case 2: {
            RenderMainPanel();
        }
        break;
        case 3: {
            RenderPedestal();
        }
        break;
        case 4: {
            RenderFloor();
        }
        break;
        case 5: {
            RenderCircuitBreakerPanels();
        }
        break;
        case 6: {
            RenderOxygenMasks();
        }
        break;
        case 7: {
            RenderYokeButtons();
        }
        break;
        default: { ImGui::Text("Page not found"); }
        break;
    }
    ImGui::End();
}

void VirtualCockpit::RenderFlightControlsSliders() {
    if (ImGui::VSliderFloat("Pitch", {20.0f, 200.0f}, &m_SystemState->flight_control_state.demanded_pitch, -1.0f,
                            1.0f)) {
        m_PitchDragging = true;
    } else {
        m_PitchDragging = false;
    }
    if (!m_PitchDragging)
        m_SystemState->flight_control_state.demanded_pitch = 0.0f;

    if (ImGui::SliderFloat("Bank", &m_SystemState->flight_control_state.demanded_bank, -1.0f, 1.0f)) {
        m_BankDragging = true;
    } else {
        m_BankDragging = false;
    }
    if (!m_BankDragging)
        m_SystemState->flight_control_state.demanded_bank = 0.0f;

    if (ImGui::SliderFloat("Yaw", &m_SystemState->flight_control_state.demanded_yaw, -1.0f, 1.0f)) {
        m_YawDragging = true;
    } else {
        m_YawDragging = false;
    }
    if (!m_YawDragging)
        m_SystemState->flight_control_state.demanded_yaw = 0.0f;
}


void VirtualCockpit::RenderOverheadPanel() {
    ImGui::Text("Overhead");

    auto draw_list = ImGui::GetWindowDrawList();
    auto image = Ouroboros::ImageStorage::Instance().GetImage("overhead");
    Ouroboros::Image::RenderImage(image, {ImGui::GetWindowPos().x + 50.0f, ImGui::GetWindowPos().y + 70.0f}, 1.3f);

    auto window_pos = ImGui::GetWindowPos();
    draw_list->AddRectFilled({window_pos.x + 80.0f, window_pos.y + 190.0f},
                             {window_pos.x + 307.0f, window_pos.y + 624.0f},
                             ImColor(124, 129, 132, 255));
    //ImColor(255, 0, 0, 255));


    ImGui::SetCursorPos({120.0f, 200.0f});
    MultiPositionKnob("IDG 1", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.idg1_state, 3,
                      {"AUTO", "OFF", "DISC"});
    ImGui::SetCursorPos({220.0f, 200.0f});
    MultiPositionKnob("IDG 2", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.idg2_state, 3,
                      {"AUTO", "OFF", "DISC"});
    ImGui::SetCursorPos({170.0f, 320.0f});
    MultiPositionKnob("AC BUS TIES",
                      &m_SystemState->flight_control_state.overhead_panel.electrical_panel.ac_bus_ties_state, 3,
                      {"AUTO", "1 OPEN", "2 OPEN"});


    ImGui::SetCursorPos({260.0f, 366.0f});
    IlluminatedButton("APU GEN", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.apu_gen_state);

    //TODO: GPU button (needs a custom button to mimic real functionality)
    ImGui::SetCursorPos({90.0f, 366.0f});
    IlluminatedButton("GPU", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.gpu_button_state);


    ImGui::SetCursorPos({100.0f, 450.0f});
    MultiPositionSwitch("TRU1", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.tru1_state, 2,
                        20.0f, 50.0f, {{"OFF", ImVec4(0.8f, 0.2f, 0.2f, 1.0f)},
                                       {"AUTO", ImVec4(0.2f, 0.6f, 0.2f, 1.0f)}
                        });
    ImGui::SetCursorPos({180.0f, 450.0f});
    MultiPositionSwitch("TRU ESS", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.tru_ess_state,
                        2, 20.0f, 50.0f, {{"OFF", ImVec4(0.8f, 0.2f, 0.2f, 1.0f)},
                                          {"AUTO", ImVec4(0.2f, 0.6f, 0.2f, 1.0f)}
                        });
    ImGui::SetCursorPos({260.0f, 450.0f});
    MultiPositionSwitch("TRU2", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.tru2_state, 2,
                        20.0f, 50.0f, {{"OFF", ImVec4(0.8f, 0.2f, 0.2f, 1.0f)},
                                       {"AUTO", ImVec4(0.2f, 0.6f, 0.2f, 1.0f)}
                        });

    ImGui::SetCursorPos({160.0f, 560.0f});
    MultiPositionSwitch(
            "DC TIES", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.dc_bus_ties_state, 2,
            20.0f, 50.0f, {{"OFF", ImVec4(0.8f, 0.2f, 0.2f, 1.0f)},
                           {"AUTO", ImVec4(0.2f, 0.6f, 0.2f, 1.0f)}
            });

    ImGui::SetCursorPos({90.0f, 510.0f});
    MultiPositionKnob("BATT 1", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.batt_1_state, 2,
                      {"OFF", "ON"});
    ImGui::SetCursorPos({250.0f, 510.0f});
    MultiPositionKnob("BATT 2", &m_SystemState->flight_control_state.overhead_panel.electrical_panel.batt_2_state, 2,
                      {"OFF", "AUTO"});


    // ____________________________________Cockpit Lights_______________________________
    draw_list->AddRectFilled({window_pos.x + 80.0f, window_pos.y + 630.0f},
                             {window_pos.x + 307.0f, window_pos.y + 804.0f},
                             ImColor(124, 129, 132, 255));
    ImGui::SetCursorPos({140.0f, 650.0f});
    ImGui::Text("COCKPIT LIGHTS");
    ImGui::SetCursorPos({200.0f, 750.0f});
    MultiPositionSwitch(
            "DOME", &m_SystemState->flight_control_state.overhead_panel.cockpit_lights.dome_light, 2,
            20.0f, 50.0f, {{"OFF", ImVec4(0.8f, 0.2f, 0.2f, 1.0f)},
                           {"ON", ImVec4(0.2f, 0.6f, 0.2f, 1.0f)}
            });

}

void VirtualCockpit::RenderMainPanel() {
    ImGui::Text("Main panel");
}

void VirtualCockpit::RenderPedestal() {
    ImGui::Text("Pedestal");
    auto image = Ouroboros::ImageStorage::Instance().GetImage("pedestal");
    Ouroboros::Image::RenderImage(image, {ImGui::GetWindowPos().x + 50.0f, ImGui::GetWindowPos().y + 70.0f}, 0.8f);
}

void VirtualCockpit::RenderFloor() {
    ImGui::Text("Floor");
}

static bool cb1 = true;

void VirtualCockpit::RenderCircuitBreakerPanels() {
    ImGui::Text("CircuitBreakerPanels");
    CircuitBreakerUI("breaker1", &cb1);
}

void VirtualCockpit::RenderOxygenMasks() {
    ImGui::Text("OxygenMasks");
}

void VirtualCockpit::RenderYokeButtons() {
    ImGui::Text("YokeButtons");
}






