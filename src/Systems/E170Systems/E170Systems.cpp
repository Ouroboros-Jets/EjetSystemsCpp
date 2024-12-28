#include "E170Systems.hpp"

#include <iostream>
#include <thread>

#include "Electrical/Electrical.hpp"
#include "Hydraulic/Hydraulic.hpp"
#include "DeltaTime/DeltaTime.hpp"
#include "Shared/SystemStruct.hpp"
#include "State/State.hpp"

using namespace std::chrono_literals;

namespace E170Systems {
    E170SystemsRoot::E170SystemsRoot(SystemState &state) :
        m_DeltaTime(0.0f), m_Running(true),
        m_ElectricalSystem(std::make_unique<ElectricalSystem>(state.electrical_vars)),
        m_HydraulicSystem(std::make_unique<HydraulicSystem>(state.hydraulic_vars)) {
        m_UpdateFunctions.emplace_back([this](const float dt) { m_ElectricalSystem->Update(dt); });
        m_UpdateFunctions.emplace_back([this](const float dt) { m_HydraulicSystem->Update(dt); });
    }

    E170SystemsRoot::~E170SystemsRoot() = default;

    void E170SystemsRoot::Update(const float deltaTime) const {
        for (const auto &system: m_UpdateFunctions) {
            system(deltaTime);
        }
    }

    void E170SystemsRoot::Run() {


        Util::DeltaTime deltaTime;
        while (m_Running) {
            m_DeltaTime = deltaTime.getDeltaTime();
            Update(m_DeltaTime);
            std::this_thread::sleep_for(17ms);
        }

    }
} // namespace E170Systems
