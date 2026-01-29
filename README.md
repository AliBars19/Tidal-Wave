# Stereo Madness

A C++ recreation of the iconic "Stereo Madness" level from **Geometry Dash**, built using OpenGL, GLFW, and GLM. This project demonstrates real-time 2D game rendering, physics simulation, and collision detection.

## Project Overview

Stereo Madness is a fast-paced rhythm platformer level recreated from scratch using modern OpenGL. The game features:

- **Player Mechanics**: Cube and ship game modes with distinct physics
- **Dynamic Camera**: Follows the player smoothly through the level
- **Level Objects**: Platforms, spikes, and obstacles with collision detection
- **Shader-based Rendering**: Vertex and fragment shaders for efficient graphics
- **Game Physics**: Gravity, velocity, rotation, and platform interaction

## Tech Stack

- **Language**: C++17
- **Graphics API**: OpenGL 3.3 Core Profile
- **Window Management**: GLFW 3
- **Mathematics**: GLM (OpenGL Mathematics)
- **Image Loading**: STB Image
- **Shader Language**: GLSL

## Project Structure

```
Stereo Madness/
├── README.md                 # Project documentation
├── assets/
│   ├── vertex.vs            # Vertex shader
│   ├── fragment.fs          # Fragment shader
│   ├── shader_s.h           # Shader class header
│   └── level.txt            # Level object definitions
├── include/
│   ├── stb_image.h          # Image loading library
│   ├── glad/                # GLAD loader headers
│   ├── GLFW/                # GLFW headers
│   └── glm/                 # GLM math library
├── lib/                     # Compiled libraries (GLFW3dll)
└── src/
    ├── stereomadness.cpp    # Main application
    ├── glad.c               # GLAD loader implementation
    └── level.txt            # Level data
```

## Building & Running

### Prerequisites
- MinGW64 with g++ compiler
- GLFW3 library
- OpenGL drivers

### Build Instructions

Using the included build task in VS Code:

```bash
# Build via VSCode task (Ctrl+Shift+B)
C/C++: g++.exe build active file
```

Or manually compile:

```bash
g++ -g -std=c++17 \
    -I./include \
    -L./lib \
    src/stereomadness.cpp src/glad.c \
    -lglfw3dll \
    -o cutable.exe
```

Then run:
```bash
./cutable.exe
```

## Game Features

### Player Modes

1. **Cube Mode** (Default)
   - Jump mechanic on ground contact
   - 90-degree rotations during flight
   - Controlled velocity and gravity
   - Landing alignment to snap to 90-degree angles

2. **Ship Mode** (Planned)
   - Alternative movement mechanics
   - Different collision bounds (2.25 x 0.75)
   - Planned implementation

### Level Objects

- **Squares**: Static platforms for movement
- **Spikes**: Obstacles that trigger level reset
- **Series Creation**: Support for repeating object patterns

### Physics System

- **Gravity**: -75.0f units per second squared
- **Jump Velocity**: 17.0f units per second
- **Horizontal Velocity**: Constant 10.385 units per second
- **Rotation**: 360 degrees per jump cycle
- **Ground Level**: Y = 2.0 units

### Camera System

- Follows player position with smooth transitions
- Maintains 16x9 viewport ratio (1280x720)
- Dynamic vertical tracking with boundaries

## Controls

- **Spacebar** (planned): Jump/Interact
- **ESC**: Close application
- More controls to be implemented

## Code Architecture

### Main Components

1. **Player Struct**
   - Position, velocity, rotation tracking
   - Mode-specific behavior (cube vs ship)
   - Physics calculations
   - Input handling
   - Rendering method

2. **LevelObject Struct**
   - Position, dimensions, rotation
   - Block type (square/spike)
   - Color and texture properties
   - Collision metadata

3. **Rendering Functions**
   - `drawQuad()`: Renders square objects
   - `drawTri()`: Renders triangular objects
   - `drawLevelObject()`: Renders level obstacles
   - Shader-based transformations

4. **Game Loop**
   - Delta time calculations
   - Input processing
   - Physics updates
   - Collision detection
   - Rendering pipeline

## Future Implementations

### Short-term Features

- [ ] **Complete Ship Mode Physics**: Implement flying/hovering mechanics
- [ ] **Collision Detection System**: Precise spike collision and reset mechanics
- [ ] **Level Loading System**: Parse level.txt and dynamically create objects
- [ ] **Input System**: Proper spacebar and ESC key handling
- [ ] **Sound System**: Music and sound effects integration
- [ ] **Score/Progress Tracking**: Percentage completion and attempt counter

### Medium-term Features

- [ ] **Texture Support**: Load and render textures for game objects
- [ ] **Particle Effects**: Jump dust, spike contact effects, level-up visuals
- [ ] **Multiple Levels**: Extend beyond Stereo Madness
- [ ] **Editor Mode**: In-game level editor for custom levels
- [ ] **UI System**: Menu, pause screen, level selection
- [ ] **Animation System**: Smoother rotations, sprite animations

### Long-term Features

- [ ] **Advanced Physics**: Slopes, moving platforms, portals
- [ ] **Gameplay Variants**: Dual mode, wave, ball, UFO modes
- [ ] **3D Rendering** (optional): Extend to full 3D with perspective
- [ ] **Network Play**: Multiplayer/competitive features
- [ ] **Mobile Port**: iOS/Android adaptation
- [ ] **Performance Optimization**: Batch rendering, frustum culling

### Code Quality Improvements

- [ ] **Object-Oriented Refactor**: Game state manager, entity system
- [ ] **Memory Management**: Smart pointers, resource pools
- [ ] **Error Handling**: Graceful failure modes, debug logging
- [ ] **Unit Tests**: Physics, collision, and rendering tests
- [ ] **Documentation**: Detailed code comments and API docs

## References & Resources

- [GLFW Tutorial](https://youtu.be/Y4F0tI7WlDs?si=fl_SR-Lq4Uc3dIxB) - Initial setup guide
- [Learn OpenGL](https://learnopengl.com/) - OpenGL concepts and best practices
- [GLM Documentation](https://glm.g-truc.net/) - Math library reference
- [Geometry Dash](https://www.geometrydash.com/) - Original game inspiration

## Known Issues & Notes

- Level objects currently hard-coded; level.txt parsing not yet implemented
- Ship mode physics not yet functional
- Collision detection system needs refinement
- Camera can occasionally skip if movement is too rapid

## Contributing

When adding new features:
1. Follow the existing code style and architecture
2. Use the physics constants at the top of the file
3. Update the level vector with test objects
4. Test collision and physics interactions
5. Update this README with new features

## License

This is a fan-made recreation project for educational purposes.

---

**Last Updated**: January 2026
**Status**: Active Development
