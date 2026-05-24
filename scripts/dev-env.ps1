# Dev build environment: imports MSVC + Windows SDK (vcvars64), adds pip's cmake/ninja.
# Dot-source before building:  . .\scripts\dev-env.ps1
#
# No $ErrorActionPreference='Stop': cmake/cl write benign warnings to stderr that
# 'Stop' would promote to fatal errors (the explicit throws below still work).

$vcvars = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars))
{
  throw "vcvars64.bat not found: $vcvars (install the C++ workload + Windows SDK)"
}
cmd /c "call `"$vcvars`" >NUL 2>&1 && set" | ForEach-Object {
  if ($_ -match '^([^=]+)=(.*)$') { Set-Item -Path "Env:$($matches[1])" -Value $matches[2] }
}

$pyScripts = "$env:APPDATA\Python\Python313\Scripts"
if (Test-Path $pyScripts) { $env:PATH = "$pyScripts;$env:PATH" }

Write-Host "Dev env ready: MSVC + Windows SDK (vcvars64)"
