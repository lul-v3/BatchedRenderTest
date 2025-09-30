# BatchedRenderTest

**BatchedRenderTest** is a small learning/demo project to explore **batched rendering** in OpenGL using SDL3. It allows you to render thousands of rectangles efficiently, experiment with instancing, and visualize performance metrics.

---

## Features

- **Batched Rendering** of rectangles and sprites using OpenGL instancing.
- **Player-controlled rectangle** using `W`, `A`, `S`, `D`.
- **Stress Test Mode** to spawn thousands of rectangles dynamically.
- **Debug Overlay** with:
    - FPS and Delta Time
    - Keybinds
    - Settings panel to control max rectangles
- Switch **polygon mode** between fill and line (`F1`).
- Toggle **settings overlay** (`F2`).

---

## Keybinds

| Key  | Action |
|------|--------|
| W,A,S,D | Move player rectangle |
| F1     | Toggle GL polygon mode (fill/line) |
| F2     | Show/Hide settings overlay |

---

## Getting Started

### Prerequisites

- C++20 compatible compiler
- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [GLEW](http://glew.sourceforge.net/)
- [GLM](https://github.com/g-truc/glm)
- OpenGL 4.3 or higher

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/lul-v3/BatchedRenderTest.git
    ```

2. Build with your preferred C++ build system (e.g., CMake, Makefile, Visual Studio).
3. Run the executable.

### Configuration

- ``MAX_RECTS``: Set the maximum number of rectangles that can be rendered.
- ``STRESS_TEST_AMOUNT``: Control how many rectangles are spawned during the stress test via the debug overlay slider (``F2``).
> Note: To increase the maximum stress test amount, you must also increase ``MAX_RECTS`` in the code.

### Code Structure
- ``main.cpp`` - Entry point, rendering loop, player input, batch rendering logic.
- ``debug.h/cpp`` - Debug overlay, FPS display, settings panel.
- ``shaders.h`` - Vertex and fragment shader sources for rectangles.
- ``assets/`` - Placeholder for images/textures (if sprites are added later).

## Future Improvements

- [ ] Add sprite rendering using the same instancing approach.
- [ ] Display VRAM usage in the debug overlay.
- [ ] Support dynamic window resizing with projection updates.
- [ ] Add more stress test shapes and color variations.

## Author

lulv3 – A personal project to explore OpenGL batched rendering with SDL3.