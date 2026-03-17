$ErrorActionPreference = "Stop"

$appName = "BrightnessEdge"
$exePath = Join-Path $PSScriptRoot "brightness_edge.exe"

if (-not (Test-Path $exePath)) {
    Write-Error "brightness_edge.exe not found in: $PSScriptRoot"
}

$regPath = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"
New-Item -Path $regPath -Force | Out-Null

$value = '"' + $exePath + '"'
New-ItemProperty -Path $regPath -Name $appName -Value $value -PropertyType String -Force | Out-Null

Write-Host "Startup enabled for $appName."
