# Builds, verifies and smoke-tests the release:
#   release/ArcanaForge_v<ver>/, release/ArcanaForge_v<ver>_win64.zip, release/ArcanaForge_v<ver>_win64.zip.sha256
# Re-running moves an existing output aside (…_old_<timestamp>) instead of deleting it.
param([switch]$SkipSmoke)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
$root = Split-Path -Parent $PSScriptRoot
$game = Join-Path $root 'game'

# version comes from the source so the title screen, exe metadata and file names always agree
$hdr = Get-Content (Join-Path $game 'src\app\App.h') -Raw
if ($hdr -notmatch '#define ARCANA_VERSION "v(\d+\.\d+\.\d+)"') { throw 'ARCANA_VERSION not found in App.h' }
$Version = $Matches[1]
$rc = Get-Content (Join-Path $game 'res\app.rc') -Raw
if ($rc -notmatch [regex]::Escape("`"ProductVersion`", `"$Version`"")) { throw "res\app.rc ProductVersion does not match $Version" }

$name = "ArcanaForge_v$Version"
$relRoot = Join-Path $root 'release'
New-Item -ItemType Directory -Force $relRoot | Out-Null
$out = Join-Path $relRoot $name
$zip = Join-Path $relRoot "${name}_win64.zip"
$stamp = Get-Date -Format 'yyyyMMdd_HHmmss'
foreach ($p in @($out, $zip, "$zip.sha256")) {
    if (Test-Path $p) { Rename-Item $p ((Split-Path $p -Leaf) -replace '(\.zip(\.sha256)?)?$', "_old_$stamp`$1") }
}
New-Item -ItemType Directory -Force $out | Out-Null

& cmd /c "`"$game\build.bat`" release app"
if ($LASTEXITCODE -ne 0) { throw 'build failed' }

Copy-Item (Join-Path $game 'bin\CardForge.exe') (Join-Path $out 'ArcanaForge.exe')
Copy-Item (Join-Path $game 'README.txt') $out
Copy-Item (Join-Path $game 'licenses') $out -Recurse
$data = Join-Path $out 'data'
New-Item -ItemType Directory -Force $data | Out-Null
Get-ChildItem (Join-Path $game 'data') -File | Where-Object { $_.Extension -in '.tsv', '.txt' } | Copy-Item -Destination $data
foreach ($sub in 'bgm', 'font', 'fx', 'gfx', 'sfx') { Copy-Item (Join-Path $game "data\$sub") $data -Recurse }

# verify: required files present, nothing unexpected, every data file of the source tree shipped
$required = 'ArcanaForge.exe', 'README.txt', 'licenses\THIRD_PARTY_NOTICES.txt', 'data\font\Probly12_LICENSE.txt', 'data\sfx\blip8_LICENSE.txt',
            'data\credits.txt', 'data\cards.tsv', 'data\enemies.tsv', 'data\rooms.tsv', 'data\arenas.tsv', 'data\relics.tsv', 'data\events.tsv',
            'data\gfx\dungeon.png', 'data\gfx\dungeon.txt', 'data\font\Probly12-CJK.ttf'
foreach ($r in $required) { if (-not (Test-Path (Join-Path $out $r))) { throw "missing in release: $r" } }
$srcFiles = Get-ChildItem (Join-Path $game 'data') -Recurse -File | ForEach-Object { $_.FullName.Substring((Join-Path $game 'data').Length) }
foreach ($s in $srcFiles) { if (-not (Test-Path (Join-Path $data $s))) { throw "data file not shipped: $s" } }
$bad = Get-ChildItem $out -Recurse -File | Where-Object { $_.Extension -in '.pdb', '.ilk', '.obj', '.psd', '.ps1', '.cpp', '.h' -or $_.Name -like '*_d.exe' }
if ($bad) { throw "development files in release: $($bad.Name -join ', ')" }
$dumpbin = Get-ChildItem "C:\Program Files\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" | Select-Object -First 1 -ExpandProperty FullName
$deps = & $dumpbin /dependents (Join-Path $out 'ArcanaForge.exe') 2>$null | Select-String '\.dll' | ForEach-Object { $_.Line.Trim() }
$nonSystem = $deps | Where-Object { $_ -match '^(MSVCP|VCRUNTIME|ucrtbase|api-ms-win-crt|Effekseer|DxLib)' }
if ($nonSystem) { throw "exe depends on non-system DLLs: $($nonSystem -join ', ')" }

# zip with forward-slash entry names (ZipFile, not Compress-Archive)
[System.IO.Compression.ZipFile]::CreateFromDirectory($out, $zip, [System.IO.Compression.CompressionLevel]::Optimal, $true)
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToLower()
"$hash  $(Split-Path $zip -Leaf)" | Set-Content "$zip.sha256" -Encoding ascii

if (-not $SkipSmoke) {
    # smoke test: unzip into a path with Japanese characters and spaces, then play one autopilot run segment
    $smokeRoot = Join-Path $env:TEMP "AF smoke テスト $stamp"
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $smokeRoot)
    $exe = Join-Path $smokeRoot "$name\ArcanaForge.exe"
    $p = Start-Process $exe -ArgumentList '--autoplay fuseevo --fresh --mute --runs 1 --speed 8 --frames 6000' -WorkingDirectory $env:TEMP -PassThru
    if (-not $p.WaitForExit(300000)) { $p.Kill(); throw 'smoke test timed out' }
    if ($p.ExitCode -ne 0) { throw "smoke test exit code $($p.ExitCode)" }
    $log = Join-Path $smokeRoot "$name\save\log.txt"
    if (-not (Test-Path $log)) { throw 'smoke test wrote no log (data not found or failed to start?)' }
    if (Test-Path (Join-Path $smokeRoot "$name\save\crash.txt")) { throw 'smoke test crashed' }
    if (-not (Select-String -Path $log -Pattern 'exit ok' -Quiet)) { throw 'smoke test did not exit cleanly' }
    "smoke test OK: $smokeRoot"
}
$files = (Get-ChildItem $out -Recurse -File).Count
$mb = [math]::Round((Get-Item $zip).Length / 1MB, 1)
"packaged v$Version : $files files -> $zip ($mb MB) sha256 $hash"
