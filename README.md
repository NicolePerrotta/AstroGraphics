# AstroGraphics Project

AstroGraphics is a Vulkan-based graphics project that simulates the liftoff of a spacecraft and allows the user to control it in a space scene with planets, asteroids, satellites and the sun.
The project features a real-time rendering engine using shaders and Vulkan pipelines.
This project was developed for the Computer Graphics exam at the Polytechnic of Milan in the academic year 2023/2024.

## Features

- **Spacecraft Liftoff Simulation**: Initiate a countdown and experience a realistic launch sequence.
- **Vulkan-based Rendering**: Uses Vulkan API for high-performance graphics.
- **Space Exploration**: Navigate the spacecraft in a dynamic space environment.
- **Shader-Based Lighting**: Implements Blinn, Emission, Skybox, and Normal mapping shaders.
- **Collision Detection**: Detects collisions between the spacecraft and scene objects.
- **Dynamic Camera Views**: Switch between different camera perspectives.

## Requirements

- Vulkan SDK
- GLFW
- GLM (OpenGL Mathematics)
- C++ Compiler (GCC, Clang, or MSVC)
- CMake
- CLion (for development setup)

## Installation

1. Copy the repository directly from **CLion**:
   - Open CLion and select `New Project from Version Control`.
   - Enter the repository URL and clone it.
   - The project will be automatically set up in CLion.

2. Install dependencies:
   - Ensure Vulkan SDK is installed and properly configured.
   - Install GLFW and GLM:
     ```bash
     sudo apt install libglfw3-dev
     sudo apt install libglm-dev
     ```

3. Configure the libraries in `CMakeLists.txt`:
   - Open `CMakeLists.txt` and ensure the paths to Vulkan, GLFW, and GLM are correctly set.
   - Modify any necessary library paths based on your system configuration.

4. Build the project in CLion:
   - Click on `Build` in the CLion toolbar.
   - Ensure CMake is configured correctly.

5. Run the application:
   ```bash
   ./AstroGraphics
   ```

## Shader Compilation

Before running the project, check that the shaders are already compiled:

```bash
./compileShaders.bat   # Windows
sh compileShaders.sh   # Linux/macOS
```

## Controls

- `SPACE`: Start the launch sequence.
- `ESC`: Quit the application.
- `V`: Print debug information.
- `1`, `2`: Switch camera views.
- `3`, `4`: Toggle between look-in and look-at modes.
- `W`, `A`, `S`, `D`: Move the spacecraft.
- `SHIFT`: Increase movement speed.
```

## License

This project is licensed under the MIT License. See `LICENSE` for details.

