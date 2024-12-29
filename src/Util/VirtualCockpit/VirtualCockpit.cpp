
#include "VirtualCockpit.hpp"

#include "Backend/Image/Image.hpp"
#include "Components/MultiPositionKnob/MultiPositionKnob.hpp"

void VirtualCockpit::RenderImGui() {
    ImGui::Begin("Virtual Cockpit");
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

static int knob_position = 0;
static int previous_knob_position = 0;
static int knob_position2 = 0;
static int previous_knob_position2 = 0;

void VirtualCockpit::RenderOverheadPanel() {
    ImGui::Text("Overhead");
    auto image = Ouroboros::ImageStorage::Instance().GetImage("overhead");
    Ouroboros::Image::RenderImage(image, {ImGui::GetWindowPos().x + 50.0f, ImGui::GetWindowPos().y + 70.0f}, 1.3f);
    ImGui::SetWindowSize({(image->GetWidth() + 50.0f) * 1.3f, (image->GetHeight() + 70.0f) * 1.3f});
    ImGui::SetCursorPos({120.0f, 220.0f});
    MultiPositionKnob("IDG 1", &knob_position, 3, {"AUTO", "OFF", "DISC"});
    if (knob_position != previous_knob_position) {
        printf("knob_position = %d\n", knob_position);
    }
    previous_knob_position = knob_position;
    ImGui::SetCursorPos({220.0f, 220.0f});
    MultiPositionKnob("IDG 2", &knob_position2, 3, {"AUTO", "OFF", "DISC"});
    if (knob_position2 != previous_knob_position2) {
        printf("knob_position = %d\n", knob_position);
    }
    previous_knob_position2 = knob_position2;
}

void VirtualCockpit::RenderMainPanel() {
    ImGui::Text("Main panel");
}

void VirtualCockpit::RenderPedestal() {
    ImGui::Text("Pedestal");
    auto image = Ouroboros::ImageStorage::Instance().GetImage("pedestal");
    Ouroboros::Image::RenderImage(image, {ImGui::GetWindowPos().x + 50.0f, ImGui::GetWindowPos().y + 70.0f}, 1.0f);
}

void VirtualCockpit::RenderFloor() {
    ImGui::Text("Floor");
}

void VirtualCockpit::RenderCircuitBreakerPanels() {
    ImGui::Text("CircuitBreakerPanels");
}

void VirtualCockpit::RenderOxygenMasks() {
    ImGui::Text("OxygenMasks");
}

void VirtualCockpit::RenderYokeButtons() {
    ImGui::Text("YokeButtons");
}






