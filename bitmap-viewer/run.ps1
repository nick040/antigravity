# run.ps1 - Run the compiled Win32 Bitmap Viewer

$exePath = "$PSScriptRoot\build\BitmapViewer.exe"

if (Test-Path $exePath) {
    Write-Host "Running BitmapViewer.exe..." -ForegroundColor Cyan
    # Pass all arguments from the script to the executable
    & $exePath $args
} else {
    Write-Host "Executable not found at $exePath." -ForegroundColor Yellow
    Write-Host "Please build the project first by running: .\build.ps1" -ForegroundColor Yellow
}
