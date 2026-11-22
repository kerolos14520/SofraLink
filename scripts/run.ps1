$ErrorActionPreference = "Stop"
cmake -S . -B build
cmake --build build
if (Test-Path "build/Debug/SofraLink.exe") { & "build/Debug/SofraLink.exe" }
elseif (Test-Path "build/Release/SofraLink.exe") { & "build/Release/SofraLink.exe" }
elseif (Test-Path "build/SofraLink.exe") { & "build/SofraLink.exe" }
