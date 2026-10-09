# Lists every character used in game text (string literals in src/app, src/game and the data files) that the
# bundled font cannot draw. DxLib silently draws nothing / a box for those, so they must be fixed in the text.
Add-Type -AssemblyName PresentationCore
$root = Split-Path -Parent $PSScriptRoot
$game = Join-Path $root 'game'
$font = New-Object System.Windows.Media.GlyphTypeface (New-Object System.Uri (Join-Path $game 'data\font\Probly12-CJK.ttf'))
$map = $font.CharacterToGlyphMap

$sources = @()
$sources += Get-ChildItem (Join-Path $game 'src\app'), (Join-Path $game 'src\game') -Filter *.cpp | Where-Object { $_.Name -notin 'Probe.cpp', 'Gallery.cpp' }
$sources += Get-ChildItem (Join-Path $game 'data') -File | Where-Object { $_.Extension -in '.tsv', '.txt' }
$missing = @{}
foreach ($f in $sources) {
    $text = [IO.File]::ReadAllText($f.FullName, [Text.Encoding]::UTF8)
    if ($f.Extension -eq '.cpp') {
        # only string literals, not comments
        $parts = [regex]::Matches($text, '"(?:[^"\\\r\n]|\\.)*"') | ForEach-Object { $_.Value }
    } else { $parts = @($text) }
    $lineOf = @{}
    foreach ($p in $parts) {
        $i = 0
        while ($i -lt $p.Length) {
            $cp = [char]::ConvertToUtf32($p, $i)
            $i += if ([char]::IsSurrogatePair($p, $i)) { 2 } else { 1 }
            if ($cp -lt 0x20 -or $cp -eq 0x5C) { continue }
            if (-not $map.ContainsKey($cp)) {
                $ch = [char]::ConvertFromUtf32($cp)
                $key = 'U+{0:X4} {1}' -f $cp, $ch
                if (-not $missing.ContainsKey($key)) { $missing[$key] = New-Object System.Collections.Generic.HashSet[string] }
                [void]$missing[$key].Add($f.Name)
            }
        }
    }
}
if ($missing.Count -eq 0) { "glyph check OK: every character is in the font" }
else {
    "MISSING GLYPHS ($($missing.Count)):"
    foreach ($k in ($missing.Keys | Sort-Object)) { "  $k   in " + (($missing[$k] | Sort-Object) -join ', ') }
}
