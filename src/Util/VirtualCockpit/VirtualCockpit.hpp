// Purpose: Provide a imgui interface for all flight deck controls (i want to fly airplane in imgui)
#pragma once

#include "Shared/State.hpp"
#include "imgui.h"

class VirtualCockpit {
public:
    static VirtualCockpit &GetInstance() {
        static VirtualCockpit instance;
        return instance;
    };

    void Initialize(SystemState &system_state) {
        m_SystemState = &system_state;
        m_Initialized = true;
    };

    [[nodiscard]] bool IsInitialized() const { return m_Initialized; }

    void RenderImGui();

private:
    VirtualCockpit() : m_SystemState(nullptr), m_Page(0) {}

    void RenderFlightControlsSliders();
    void RenderOverheadPanel();
    void RenderMainPanel();
    void RenderPedestal();
    void RenderFloor(); // emergency gear release
    void RenderCircuitBreakerPanels(); // all of them
    void RenderOxygenMasks();
    void RenderYokeButtons();


    SystemState *m_SystemState;
    bool m_Initialized{false};
    bool m_PitchDragging{false};
    bool m_BankDragging{false};
    bool m_YawDragging{false};

    int m_Page;
};
