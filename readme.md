# Ouroboros EJet Systems

A high-performance, modern C++ systems implementation for the Embraer EJet family MSFS addon, designed to run both
natively and
in WebAssembly.

## Features

- **Ultra-High Performance**: Built with modern C++23, utilizing advanced language features for maximum efficiency
- **Flexible Runtime Environment**:
    - Runs natively as a Win32 executable
    - Runs inside MSFS via WebAssembly
- **Interactive Debug Environment**:
    - Virtual cockpit with fully functional buttons and switches
    - Real-time system parameter adjustment
    - Built with platform-agnostic technologies (GLFW & Vulkan)
    - Weights system for fine-tuning system calculations and outputs

## Technical Overview

### Architecture

- Modern C++23 implementation with extensive use of OOP principles
- Class hierarchy based design for maximum code reuse and maintainability
- System state management through efficient reference passing (making the systems run without knowing of the simulator's
  existence)
- Systems are modular in nature making it easy to adapt to different aircraft

### Performance

- Optimized for both single-threaded (WASM) and multi-threaded (WIN32) operations
- Efficient state management and communication between systems

### Debug GUI

- Real-time system state visualization
- Interactive controls for system testing via the virtual cockpit
- Performance monitoring tools similar to the actual EICAS and MFD
- Weight adjustment interface for system tuning

## Project Structure

```
src/
├── Systems/ # Core aircraft systems implementation
├── Systems/SystemsMain/SystemsMain.hpp # Entry point for systems outside of the debug environment
├── Backend/ # Platform-agnostic debug GUI
├── Units/ # Unit typedefs
├── Util/ # Utility wrapper for virtual cockpit, weights etc.
└── Assets/ # Header files containing icons, images etc.

```

Key Components:

- `SystemMain.hpp`: Primary entry point for both WASM and native builds
- Individual system classes in `systems/` directory
- Debug GUI implementation in `Renderer/` directory

## Building

1. Clone the repository with submodules:
   ```shell
   git clone --recursive https://github.com/yourusername/EjetSystemsCpp.git
   cd EjetSystemsCpp
   ```

2. Generate build files:
   ```shell
    mkdir build && cd build
    cmake .. #Note: If vcpkg does not auto install deps with cmake you must run `vcpkg_install` in the build dir prior to generating build files 
   ```

3. Build the project:
   ```shell
    cmake --build .
   ```

## Development

### Adding New Systems

1. Create a new class inheriting from `System` base class
2. Implement required virtual methods
3. Create a modular system that operates via sequential calculations and is reconstructed each loop
4. Register the system in `E170Systems.cpp` inside the `E170SystemsRoot`class.  **Make sure
   your `system->Update(float dt)` method is implemented**

### Debug GUI Usage

1. Launch the debug executable
2. Use the virtual cockpit controls to test system responses
3. Adjust weights through the GUI to fine-tune system behavior
4. Monitor real-time performance metrics
5. Press `Save` on the weights tab and close out of the gui
6. run `python3 scripts/convert_weights` then rebuild the systems, this will compile your weights into the binary

## Requirements

- C++ compiler with C++23 support (`GCC 14.2` and `MSVC 19.40 (VS2022)` tested)
- CMake 3.20 or higher
- Vcpkg (`VCPKG_ROOT` env var set)

## License

See [LICENSE](LICENSE)