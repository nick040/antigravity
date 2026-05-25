# run.ps1 - Run the compiled C++ executable

$exePath = ".\build\Cpp98Project.exe"

if (Test-Path $exePath) {
    Write-Host "Running Cpp98Project.exe..." -ForegroundColor Cyan
    & $exePath
} else {
    Write-Host "Executable not found at $exePath." -ForegroundColor Yellow
    Write-Host "Please build the project first by running: .\build.ps1" -ForegroundColor Yellow
}
