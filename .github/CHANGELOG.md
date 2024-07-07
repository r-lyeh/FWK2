## Changelog

New in v2:

- Smaller demos.
- Smaller codebase.
- Smaller repository size.
- Faster warm-up/boot times.
- Smoother experience. Higher framerates, less CPU usage.
- New ext/ plugin system. Self-discovery. Community-driven (Github).
- New backends: SDL3, OpenAL, Luajit, DearImgui, ImPlot, etc.
- New redesigned UI module: Docking and multi-viewports ready.
- New redesigned Game modules: Loop, App, 3D Audio, Script, Shader...
- New bindings generator. All modules and APIs are 1:1 exposed to Lua scripts.
- Simplified build process. MAKE will link automatically ext/ dependencies.
- Simplified code structure. Old split/joint concepts are gone now.
- Simplified cook process. Python + tools/ folder no longer required.
- Simplified APIs in many cases. Smaller is better.
- Simplified implementations for many modules: cook, file, memory, panic, logger...
- Editor, Rendering and Scene modules decoupled from other APIs.
- Engine, Editor and Demos can interop with C/C++ external code now.
- Engine, Editor and Demos can optionally embed assets into output binary now.
- Python3 no longer required.
- And more.

Compared to v1, many things dropped:

- Dropped pure C engine concept: the engine is still C, but there are C++ dependencies now.
- Dropped support for Linux, OSX and Emscripten targets.
- Dropped support for gcc, clang, clang-cl and tcc compilers.
- Dropped support for Python bindings.
- Dropped support for single-header distributions.
- Dropped support for cooked zipfiles.
- Dropped support for fused binaries.
- Dropped support for self-generated documentation.
- And more.
