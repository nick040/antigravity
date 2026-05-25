# C++ MinGW (w64devkit) Project Template

A template for a Windows 11-compatible C++ project conforming to the **C++98** standard. It compiles via the command line interface (CLI) using CMake and compilers provided by the `w64devkit` distribution.

## Project Structure
- `CMakeLists.txt` - CMake configuration file specifying C++98, compilation targets, and warnings.
- `src/main.cpp` - Sample source file implementing C++98 compliant logic, showing compiler version, and using STL containers.
- `build.ps1` - PowerShell utility script to automatically configure CMake and compile the executable.
- `run.ps1` - PowerShell helper to execute the compiled program.

## Prerequisites
- **OS**: Windows 11
- **Toolchain**: `w64devkit` located at `D:\Tools\w64devkit` (specifically `D:\Tools\w64devkit\bin` containing `g++`, `gcc`, `make`, and `cmake`).

---

## Getting Started

### Option A: Using Helper Scripts (Recommended)

1. **Build the project:**
   ```powershell
   .\build.ps1
   ```
   This will generate a `build/` directory, configure CMake with MinGW Makefiles, and compile `src/main.cpp`.

2. **Run the executable:**
   ```powershell
   .\run.ps1
   ```

### Option B: Building Manually from PowerShell CLI

If you want to configure and compile manually without the helper scripts, execute the following commands in order:

1. **Add the compiler tools to your CLI path temporarily:**
   ```powershell
   $env:PATH = "D:\Tools\w64devkit\bin;" + $env:PATH
   ```

2. **Configure CMake targeting MinGW Makefiles:**
   ```powershell
   cmake -G "MinGW Makefiles" -B build -S .
   ```

3. **Build the project executable:**
   ```powershell
   cmake --build build
   ```

4. **Run the program:**
   ```powershell
   .\build\Cpp98Project.exe
   ```

---

## Compiler Enforcement Check
The sample program outputs the C++ standard value (`__cplusplus`). For C++98, it should print:
```text
C++ Standard version (__cplusplus): 199711
-> Enforcing C++98 standard successfully!
```
