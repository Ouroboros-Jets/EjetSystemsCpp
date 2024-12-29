cmake_minimum_required(VERSION 3.4...3.30 FATAL_ERROR)

include(cmake/colors.cmake)
include(FetchContent)

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


find_package(Vulkan REQUIRED)

FetchContent_Declare(GLFW GIT_REPOSITORY https://github.com/TheCherno/glfw.git)
FetchContent_Declare(Infinity GIT_REPOSITORY https://github.com/Tacotakedown/InfinityPackageBuilderDeps.git)
FetchContent_MakeAvailable(GLFW Infinity)


file(GLOB IMGUI_SOURCES
        ${infinity_SOURCE_DIR}/src/imgui/
        ${infinity_SOURCE_DIR}/src/imgui/imgui.cpp
        ${infinity_SOURCE_DIR}/src/imgui/imgui_draw.cpp
        ${infinity_SOURCE_DIR}/src/imgui/imgui_widgets.cpp
        ${infinity_SOURCE_DIR}/src/imgui/imgui_tables.cpp
        ${infinity_SOURCE_DIR}/src/imgui/misc/cpp/imgui_stdlib.cpp
        ${infinity_SOURCE_DIR}/src/imgui/backends/imgui_impl_glfw.cpp
        ${infinity_SOURCE_DIR}/src/imgui/backends/imgui_impl_vulkan.cpp
        ${infinity_SOURCE_DIR}/src/imguiTextEditor/TextEditor.cpp
        ${infinity_SOURCE_DIR}/src/imguiTextEditor/TextEditor.h
)


file(GLOB STB_IMAGE ${infinity_SOURCE_DIR}/include/stb_image/stb_image.h)
file(GLOB JSON_SOURCES ${infinity_SOURCE_DIR}/include/Json/*.hpp)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Systems
)


add_executable(EjetSystems
        src/main.cpp
        src/Systems/E170Systems/E170Systems.cpp
        src/Systems/E170Systems/E170Systems.hpp
        src/Util/DeltaTime/DeltaTime.cpp
        src/Util/DeltaTime/DeltaTime.hpp
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
        src/Systems/Shared/State.hpp
        src/Util/Weights/Weights.cpp
        src/Util/Weights/Weights.hpp
        src/Util/VirtualCockpit/VirtualCockpit.cpp
        src/Util/VirtualCockpit/VirtualCockpit.hpp
        src/Backend/Application/Application.cpp
        src/Backend/Application/Application.hpp
        src/Backend/Image/Image.cpp
        src/Backend/Image/Image.hpp
        src/Backend/VulkanManager/VulkanManager.hpp
        src/Backend/Layer/Layer.hpp
        src/Backend/UiHelpers/UiHelpers.cpp
        src/Backend/UiHelpers/UiHelpers.hpp
        src/Backend/ImGuiTheme/ImGuiTheme.cpp
        src/Backend/ImGuiTheme/ImGuiTheme.hpp
        src/Util/VirtualCockpit/Components/MultiPositionKnob/MultiPositionKnob.hpp
)

target_sources(EjetSystems PRIVATE ${STB_IMAGE} ${IMGUI_SOURCES} ${JSON_SOURCES})

target_include_directories(EjetSystems PRIVATE
        ${infinity_SOURCE_DIR}/src/imgui
        ${infinity_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(EjetSystems PRIVATE Vulkan::Vulkan glfw)
