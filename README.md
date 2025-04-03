# Stein Game Engine (SGE)

**Stein Game Engine** is an experimental 3D game engine written in C, created as a learning project to explore Vulkan, rendering techniques, and how game engines work under the hood. This project is in early development and is not yet intended for production use.

---

## 🚧 Status

**Early Development** — Core features and architecture are actively evolving. Expect bugs, missing functionality, and breaking changes.

---

## 🎯 Goals

- Learn Vulkan API and 3D rendering fundamentals  
- Understand core game engine architecture (input, rendering, logging, etc.)  
- Build a minimal but extendable foundation for future engine development

---

## 🧠 Features (So Far)

- Custom logging system
- Input handling
- Basic Vulkan rendering setup
- Modular file-based organization
- Configurable CMake build system

---

## 📁 Project Structure

```
SGE-master/
├── Documentation/       # Text-based documentation for engine subsystems
├── include/             # (If exists) Header files
├── src/                 # Core source code for the engine
├── test.c               # Test harness or sample usage
├── CMakeLists.txt       # CMake build config
├── .clang-format        # Formatting rules
├── SGEConfig.cmake.in   # CMake module config
```

---

## 🛠️ Build Instructions

### Requirements

- C compiler (GCC or Clang)
- [CMake](https://cmake.org/) (version 3.10+ recommended)
- Vulkan SDK installed and configured

### Build Steps

```bash
git clone https://github.com/SincPaul/SGE.git
cd SGE
mkdir build && cd build
cmake ..
make
```

---

## 📜 Documentation

See the `Documentation/` folder for subsystem details like:

- `logger.txt` — Logging system info
- `input_control.txt` — Input handling
- `display` — Display/rendering notes

---

## 🤝 Contributing

This project is mainly for personal learning, but contributions and feedback are welcome! Feel free to fork, experiment, or open issues.