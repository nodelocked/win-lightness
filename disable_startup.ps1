$ErrorActionPreference = "Stop"

$appName = "BrightnessEdge"
$regPath = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

if (Test-Path $regPath) {
    Remove-ItemProperty -Path $regPath -Name $appName -ErrorAction SilentlyContinue
}

Write-Host "Startup disabled for $appName."
