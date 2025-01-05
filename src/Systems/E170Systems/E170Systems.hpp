#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "Electrical/Electrical.hpp"
#include "Hydraulic/Hydraulic.hpp"
#include "Shared/SystemStruct.hpp"
#include "System/System.hpp"

#define EXTERNAL_SYSTEMS

namespace E170Systems {
    class E170SystemsRoot {
    public:
        explicit E170SystemsRoot(SystemState &state);

        ~E170SystemsRoot();


        void Run();
#ifdef EXTERNAL_SYSTEMS

    private:
        void Update(float deltaTime) const;
#else
        void Update(float deltaTime) const;

    private:
#endif

        float m_DeltaTime;
        bool m_Running;

        std::unique_ptr<HydraulicSystem> m_HydraulicSystem;
        std::unique_ptr<ElectricalSystem> m_ElectricalSystem;

        std::vector<std::function<void(float)>> m_UpdateFunctions;
    };
} // namespace E170Systems
