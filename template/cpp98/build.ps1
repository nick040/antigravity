# build.ps1 - C++ MinGW/w64devkit build script

# Add w64devkit to PATH for this process
$env:PATH = "D:\Tools\w64devkit\bin;" + $env:PATH

Write-Host "Configuring CMake with MinGW Makefiles..." -ForegroundColor Cyan

# Run CMake Configuration (Single-line to avoid PowerShell line continuation issues)
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER="D:/Tools/w64devkit/bin/gcc.exe" -DCMAKE_CXX_COMPILER="D:/Tools/w64devkit/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="D:/Tools/w64devkit/bin/make.exe" -B build -S .

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed."
    exit $LASTEXITCODE
}

Write-Host "Building project..." -ForegroundColor Cyan

# Run CMake Build
cmake --build build

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed."
    exit $LASTEXITCODE
}

Write-Host "Build completed successfully!" -ForegroundColor Green
