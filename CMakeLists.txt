cmake_minimum_required(VERSION 3.4...3.30 FATAL_ERROR)

include(cmake/colors.cmake)

if (WIN32)
    set(VCPKG_TRIPLET "x64-windows")
else ()
    set(VCPKG_TRIPLET "x64-linux")
endif ()
if (NOT VCPKG_ROOT)
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif ()
if (VCPKG_ROOT)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message("${Blue}Using Toolchain file: ${Green}${CMAKE_TOOLCHAIN_FILE}${ColorReset}")
else ()
    message(FATAL_ERROR "VCPKG_ROOT is not set in the environment!")
endif ()


project(EjetSystems VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)


set(IMGUI_SOURCES
        dependencies/ImGui/imgui.cpp
        dependencies/ImGui/imgui_draw.cpp
        dependencies/ImGui/imgui_widgets.cpp
        dependencies/ImGui/imgui_tables.cpp
        dependencies/ImGui/misc/cpp/imgui_stdlib.cpp
        dependencies/ImGui/backends/imgui_impl_glfw.cpp
        dependencies/ImGui/backends/imgui_impl_vulkan.cpp
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Systems
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Util
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Units
)


add_executable(EjetSystems
        src/main.cpp
        src/Systems/E170Systems/E170Systems.cpp
        src/Systems/E170Systems/E170Systems.hpp
        src/Util/DeltaTime/DeltaTime.cpp
        src/Util/DeltaTime/DeltaTime.hpp
        src/Renderer/Fonts.cpp
        src/Renderer/Fonts.h
        src/Renderer/VulkanUtils.h
        src/Systems/SystemsMain/SystemsMain.hpp
        src/Systems/System/System.hpp
        src/Systems/Electrical/Electrical.cpp
        src/Systems/Electrical/Electrical.hpp
        src/Systems/Hydraulic/Hydraulic.cpp
        src/Systems/Hydraulic/Hydraulic.hpp
        src/Units/Units.hpp
        src/Systems/Hydraulic/Components/Accumulator.cpp
        src/Systems/Hydraulic/Components/Accumulator.hpp
        src/Systems/Hydraulic/Components/Actuator.cpp
        src/Systems/Hydraulic/Components/Actuator.hpp
        src/Systems/Hydraulic/Components/FilterManifold.cpp
        src/Systems/Hydraulic/Components/FilterManifold.hpp
        src/Systems/Hydraulic/Components/Fluid.cpp
        src/Systems/Hydraulic/Components/Fluid.hpp
        src/Systems/Hydraulic/Components/HydraulicLine.cpp
        src/Systems/Hydraulic/Components/HydraulicLine.hpp
        src/Systems/Hydraulic/Components/Math.hpp
        src/Systems/Hydraulic/Components/Pump.cpp
        src/Systems/Hydraulic/Components/Pump.hpp
        src/Systems/Hydraulic/Components/Reservoir.cpp
        src/Systems/Hydraulic/Components/Reservoir.hpp
        src/Systems/Hydraulic/Components/HydraulicComponents.hpp
        src/Systems/Shared/SystemStruct.hpp
        src/Systems/Electrical/Components/ElectricalComponent.cpp
        src/Systems/Electrical/Components/ElectricalComponent.hpp
        src/Systems/Electrical/Components/Wire.cpp
        src/Systems/Electrical/Components/Wire.hpp
        src/Systems/Electrical/Busses/Busses.hpp
        src/Systems/Electrical/Components/GenericDcComponent.cpp
        src/Systems/Electrical/Components/GenericDcComponent.hpp
        src/Systems/Electrical/Components/GenericAcComponent.cpp
        src/Systems/Electrical/Components/GenericAcComponent.hpp
        src/Systems/Electrical/Components/CircuitBreaker.cpp
        src/Systems/Electrical/Components/CircuitBreaker.hpp
        src/Systems/Electrical/ElectricalUtil/ElectricalUtil.hpp
        src/Systems/State/State.hpp
)

target_sources(EjetSystems PRIVATE ${IMGUI_SOURCES})


target_link_libraries(EjetSystems PRIVATE Vulkan::Vulkan glfw)
