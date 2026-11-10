<#
.SYNOPSIS
    Build the Windows installer for FigureItNow7 using Qt Installer Framework.

.DESCRIPTION
    Pipeline: clean -> Release build -> cmake --install into staging -> windeployqt ->
              verify runtime -> render IFW config -> binarycreator -> output to dist/.

    Default mode is "offline": a self-contained installer that embeds all payload and
    never contacts an online repository. Once a GitHub Pages update repository exists,
    switch to "hybrid" (offline install + online updates) with -Mode hybrid -RepoUrl <url>.

.PARAMETER Mode
    offline : fully offline installer (binarycreator --offline-only).
    hybrid  : installs offline, and the maintenance tool can update online (binarycreator --hybrid).

.PARAMETER RepoUrl
    Base URL of the online update repository. Required for hybrid mode only.
    IFW appends /Updates.xml to it. Example: https://yuylong.github.io/FigureItNow/updates

.PARAMETER IfwRoot
    The bin directory of Qt Installer Framework, e.g. C:\Qt\Tools\QtInstallerFramework\4.11.0\bin.
    When omitted, binarycreator.exe is searched under C:\Qt\Tools and Program Files,
    and the QTIFWDIR environment variable is also consulted.

.PARAMETER QtDir
    Qt installation directory, e.g. C:\Qt\6.8.0\mingw_64. Auto-detected when omitted.

.PARAMETER Clean
    Delete build-release and packaging\build-ifw before building.

.PARAMETER Translations
    Comma-separated Qt built-in translation languages to ship, default zh_CN.
    Pass an empty string to ship no translations at all.

.PARAMETER Full
    Deploy every Qt graphics backend. By default roughly 50 MB of components
    (opengl32sw, D3Dcompiler_47, dxcompiler, dxil) are excluded because this is a
    raster-rendered Qt Widgets application. Use this switch if rendering breaks
    on a specific machine.

.PARAMETER SkipBuild
    Skip CMake configure/build and reuse the existing build-release directory.

.PARAMETER SkipPackage
    Stop after the staging directory, without invoking binarycreator. Useful for
    validating everything except the final packaging step when IFW is not installed.

.EXAMPLE
    .\build_installer.ps1

.EXAMPLE
    .\build_installer.ps1 -Clean -SkipBuild

.EXAMPLE
    .\build_installer.ps1 -Mode hybrid -RepoUrl https://yuylong.github.io/FigureItNow/updates
#>
[CmdletBinding()]
param(
    [ValidateSet('offline', 'hybrid')]
    [string] $Mode = 'offline',

    [string] $RepoUrl = '',

    [string] $IfwRoot = '',

    [string] $QtDir = '',

    [string] $Translations = 'zh_CN',

    [switch] $Full,

    [switch] $Clean,

    [switch] $SkipBuild,

    [switch] $SkipPackage
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# ---------------------------------------------------------------------------
# Constants and paths
# ---------------------------------------------------------------------------
# Package identifier: individuals use io.github.<user>.<app>, which maps onto the
# GitHub Pages namespace. Must match <Name> in package.xml and must never change
# after the first public release, otherwise installed users stop receiving updates.
$PackageId   = 'io.github.yuylong.figureitnow7'
$AppExeName  = 'FigureItNow7.exe'

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path      # packaging/
$ProjectDir  = Split-Path -Parent $ScriptDir                        # FigureItNow7/
$RepoRoot    = Split-Path -Parent $ProjectDir                       # repository root (holds examples/)

$BuildDir    = Join-Path $ProjectDir 'build-release'
$WorkDir     = Join-Path $ScriptDir  'build-ifw'
$RenderDir   = Join-Path $WorkDir    'config'
$PackagesDir = Join-Path $WorkDir    'packages'
$DataDir     = Join-Path $PackagesDir (Join-Path $PackageId 'data')
$MetaDir     = Join-Path $PackagesDir (Join-Path $PackageId 'meta')
$DistDir     = Join-Path $ProjectDir 'dist'
$RepoOutDir  = Join-Path $ScriptDir  'repository'

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------
function Write-Step {
    param([string] $Message)
    Write-Host ''
    Write-Host "==> $Message" -ForegroundColor Cyan
}

function Write-Ok {
    param([string] $Message)
    Write-Host "    [OK] $Message" -ForegroundColor Green
}

function Write-Warn {
    param([string] $Message)
    Write-Host "    [!!] $Message" -ForegroundColor Yellow
}

function Fail {
    param([string] $Message)
    Write-Host ''
    Write-Host "ERROR: $Message" -ForegroundColor Red
    exit 1
}

# The version rule lives in cmake/BuildVersion.cmake, which is also what the build itself uses. Ask
# CMake for the value instead of parsing git here, so the installer, the compiled-in version and the
# About dialog cannot drift apart.
# v0.5-alpha-164-g7685e8b-dirty  ->  0.5.164
function Get-AppVersion {
    param([string] $CmakeExe)

    $versionScript = Join-Path $ProjectDir 'cmake\BuildVersion.cmake'
    if (-not (Test-Path $versionScript)) {
        Fail "Version module not found: $versionScript"
    }

    $reportFile = Join-Path ([System.IO.Path]::GetTempPath()) "figureitnow-version-$PID.txt"
    try {
        # Capture the output: it must not leak into this function's return value.
        $output = & $CmakeExe -D "FIN_VERSION_SOURCE_DIR=$ProjectDir" -D "FIN_VERSION_REPORT=$reportFile" `
                              -P $versionScript 2>&1
        if ($LASTEXITCODE -ne 0 -or -not (Test-Path $reportFile)) {
            Write-Host $output
            Fail 'Could not compute the build version from cmake/BuildVersion.cmake'
        }

        $values = @{}
        foreach ($line in (Get-Content -Path $reportFile -Encoding UTF8)) {
            if ($line -match '^([A-Z_]+)=(.*)$') { $values[$Matches[1]] = $Matches[2] }
        }
    } finally {
        Remove-Item -Path $reportFile -Force -ErrorAction SilentlyContinue
    }

    if (-not $values.ContainsKey('APP_VERSION_NUMERIC')) {
        Fail 'cmake/BuildVersion.cmake did not report APP_VERSION_NUMERIC'
    }
    if ($values['APP_VERSION_NUMERIC'] -eq '0.0.0') {
        Write-Warn "Could not derive a numeric version from '$($values['APP_VERSION'])'; using 0.0.0"
    }

    return $values['APP_VERSION_NUMERIC']
}

# Search the given roots for an executable and return the directory that holds it.
function Find-ToolDir {
    param(
        [string] $ExeName,
        [string[]] $Roots
    )
    foreach ($root in $Roots) {
        if (-not (Test-Path $root)) { continue }
        $hit = Get-ChildItem -Path $root -Recurse -Filter $ExeName -ErrorAction SilentlyContinue |
               Select-Object -First 1
        if ($hit) { return $hit.DirectoryName }
    }
    return $null
}

function Resolve-QtDir {
    if ($QtDir) {
        if (Test-Path (Join-Path $QtDir 'bin\windeployqt.exe')) { return $QtDir }
        Fail "bin\windeployqt.exe not found under the given -QtDir: $QtDir"
    }
    if ($env:QTDIR -and (Test-Path (Join-Path $env:QTDIR 'bin\windeployqt.exe'))) {
        return $env:QTDIR
    }

    # Only MinGW Qt packages are considered: the build below forces "MinGW Makefiles" + gcc/g++, so an
    # MSVC Qt would fail at configure/link time (or produce an ABI-broken package).
    $patterns = @('C:\Qt\*\mingw*_64', 'C:\Qt\*\mingw_64')
    foreach ($pattern in $patterns) {
        $candidates = Get-ChildItem -Path $pattern -Directory -ErrorAction SilentlyContinue |
                      Sort-Object Name -Descending
        foreach ($candidate in $candidates) {
            if (Test-Path (Join-Path $candidate.FullName 'bin\windeployqt.exe')) {
                return $candidate.FullName
            }
        }
    }
    return $null
}

function Resolve-MingwBin {
    if ($env:MINGW_PATH -and (Test-Path (Join-Path $env:MINGW_PATH 'g++.exe'))) {
        return $env:MINGW_PATH
    }
    $hit = Get-ChildItem -Path 'C:\Qt\Tools\mingw*_64\bin\g++.exe' -ErrorAction SilentlyContinue |
           Sort-Object FullName -Descending | Select-Object -First 1
    if ($hit) { return $hit.DirectoryName }

    $onPath = Get-Command 'g++.exe' -ErrorAction SilentlyContinue
    if ($onPath) { return (Split-Path -Parent $onPath.Source) }
    return $null
}

function Resolve-IfwBin {
    if ($IfwRoot) {
        if (Test-Path (Join-Path $IfwRoot 'binarycreator.exe')) { return $IfwRoot }
        Fail "binarycreator.exe not found under the given -IfwRoot: $IfwRoot"
    }
    if ($env:QTIFWDIR -and (Test-Path (Join-Path $env:QTIFWDIR 'binarycreator.exe'))) {
        return $env:QTIFWDIR
    }
    return Find-ToolDir -ExeName 'binarycreator.exe' -Roots @(
        'C:\Qt\Tools',
        'C:\Program Files\Qt',
        'C:\Program Files (x86)\Qt'
    )
}

function Write-Utf8NoBom {
    param(
        [string] $Path,
        [string] $Content
    )
    $encoding = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Path, $Content, $encoding)
}

# ---------------------------------------------------------------------------
# 0. Argument validation and version
# ---------------------------------------------------------------------------
if ($Mode -eq 'hybrid' -and -not $RepoUrl) {
    Fail 'hybrid mode requires -RepoUrl to point at the update repository'
}
$RepoUrl = $RepoUrl.TrimEnd('/')

# CMake derives the version and drives the build, so resolve it before the version step.
$CmakeCmd = Get-Command 'cmake' -ErrorAction SilentlyContinue
if (-not $CmakeCmd) {
    Fail 'cmake not found. Install CMake and make sure it is on PATH.'
}
$CmakeExe = $CmakeCmd.Source

$AppVersion  = Get-AppVersion -CmakeExe $CmakeExe
$ReleaseDate = Get-Date -Format 'yyyy-MM-dd'

Write-Host ''
Write-Host 'FigureItNow7 installer build' -ForegroundColor White
Write-Host "    Mode        : $Mode"
Write-Host "    Version     : $AppVersion"
Write-Host "    Date        : $ReleaseDate"
Write-Host "    Project dir : $ProjectDir"
if ($Mode -eq 'hybrid') {
    Write-Host "    Update repo : $RepoUrl"
}

# ---------------------------------------------------------------------------
# 1. Environment detection
# ---------------------------------------------------------------------------
Write-Step 'Detecting build environment'

$QtPath = Resolve-QtDir
if (-not $QtPath) {
    Fail 'Qt not found. Install Qt 6.x (MinGW build), pass -QtDir, or set QTDIR.'
}
# Qt's MSVC packages ship lib\Qt6Core.lib; only MinGW packages can be linked with gcc/g++ below.
if (Test-Path (Join-Path $QtPath 'lib\Qt6Core.lib')) {
    Fail "Qt at '$QtPath' is an MSVC build; this script only supports MinGW Qt."
}
Write-Ok "Qt: $QtPath"

$WindeployQt = Join-Path $QtPath 'bin\windeployqt.exe'

$MingwBin = Resolve-MingwBin
if (-not $MingwBin) {
    Fail 'MinGW toolchain (g++.exe) not found. Set MINGW_PATH or add MinGW via the Qt installer.'
}
Write-Ok "MinGW: $MingwBin"

Write-Ok ("CMake: " + ((& $CmakeExe --version | Select-Object -First 1)))

$IfwBin = $null
if (-not $SkipPackage) {
    $IfwBin = Resolve-IfwBin
    if (-not $IfwBin) {
        Fail @'
Qt Installer Framework (binarycreator.exe) not found.

Install it with one of:
  1) Qt Online Installer -> Qt > Build tools > Qt Installer Framework
  2) MSYS2: pacman -S mingw-w64-x86_64-qt-installer-framework

Then pass -IfwRoot pointing at its bin directory, or set QTIFWDIR.
To validate everything but packaging first, run with -SkipPackage.
'@
    }
    Write-Ok "IFW: $IfwBin"
} else {
    Write-Warn '-SkipPackage given; skipping IFW detection and packaging'
}

# ---------------------------------------------------------------------------
# 2. Optional cleanup
# ---------------------------------------------------------------------------
if ($Clean) {
    Write-Step 'Cleaning previous output'
    foreach ($dir in @($BuildDir, $WorkDir)) {
        if (Test-Path $dir) {
            Remove-Item $dir -Recurse -Force
            Write-Ok "Removed $dir"
        }
    }
}

# ---------------------------------------------------------------------------
# 3. Build Release
# ---------------------------------------------------------------------------
if ($SkipBuild) {
    Write-Step 'Skipping build (-SkipBuild)'
    if (-not (Test-Path (Join-Path $BuildDir 'CMakeCache.txt'))) {
        Fail "No reusable build directory: $BuildDir"
    }
} else {
    Write-Step 'Configuring and building Release'

    $gcc   = Join-Path $MingwBin 'gcc.exe'
    $gxx   = Join-Path $MingwBin 'g++.exe'
    $make  = Join-Path $MingwBin 'mingw32-make.exe'
    if (-not (Test-Path $make)) {
        $ninja = Get-Command 'ninja.exe' -ErrorAction SilentlyContinue
        if ($ninja) { $make = $ninja.Source }
    }
    if (-not (Test-Path $make)) {
        Fail "Neither mingw32-make.exe nor ninja found (looked in $MingwBin)"
    }

    # Pass configure flags as an array: the -DXXX=$var form is not expanded by PowerShell.
    $configureArgs = @(
        '-S', $ProjectDir,
        '-B', $BuildDir,
        '-G', 'MinGW Makefiles',
        '-DCMAKE_BUILD_TYPE=Release',
        "-DCMAKE_C_COMPILER=$gcc",
        "-DCMAKE_CXX_COMPILER=$gxx",
        "-DCMAKE_MAKE_PROGRAM=$make",
        "-DCMAKE_PREFIX_PATH=$QtPath"
    )
    & $CmakeExe @configureArgs
    if ($LASTEXITCODE -ne 0) { Fail 'CMake configure failed' }

    & $CmakeExe --build $BuildDir --config Release --parallel
    if ($LASTEXITCODE -ne 0) { Fail 'Build failed' }
    Write-Ok 'Build finished'
}

$BuiltExe = Join-Path $BuildDir $AppExeName
if (-not (Test-Path $BuiltExe)) {
    Fail "Built executable not found: $BuiltExe"
}

# ---------------------------------------------------------------------------
# 4. Install into staging
# ---------------------------------------------------------------------------
Write-Step 'Installing into staging directory'

if (Test-Path $DataDir) {
    Remove-Item $DataDir -Recurse -Force
}
New-Item -ItemType Directory -Path $DataDir    -Force | Out-Null
New-Item -ItemType Directory -Path $MetaDir    -Force | Out-Null
New-Item -ItemType Directory -Path $RenderDir  -Force | Out-Null
New-Item -ItemType Directory -Path $DistDir    -Force | Out-Null

& $CmakeExe --install $BuildDir --prefix $DataDir --config Release
if ($LASTEXITCODE -ne 0) { Fail 'cmake --install failed' }
Write-Ok "Staging: $DataDir"

# ---------------------------------------------------------------------------
# 5. Collect the Qt runtime with windeployqt
# ---------------------------------------------------------------------------
Write-Step 'Deploying Qt runtime (windeployqt)'

$deployArgs = @(
    '--release',
    '--compiler-runtime'
)

# This is a pure Qt Widgets + QGraphicsScene application with raster rendering, so it
# does not need the software OpenGL fallback or the D3D/DXC shader compilers. Those
# back ends total roughly 50 MB and are excluded by default; use -Full to ship them.
if (-not $Full) {
    $deployArgs += @(
        '--no-opengl-sw',
        '--no-system-d3d-compiler',
        '--no-system-dxc-compiler'
    )
}

$deployArgs += @('--dir', $DataDir)

if ($Translations) {
    $deployArgs += @('--translations', $Translations)
} else {
    $deployArgs += '--no-translations'
}
$deployArgs += (Join-Path $DataDir $AppExeName)

& $WindeployQt @deployArgs
if ($LASTEXITCODE -ne 0) { Fail 'windeployqt failed' }
Write-Ok 'windeployqt finished'

# ---------------------------------------------------------------------------
# 6. Verify runtime libraries and plugins
# ---------------------------------------------------------------------------
Write-Step 'Verifying staging completeness'

$MinGwDlls = @('libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll')
foreach ($dll in $MinGwDlls) {
    $target = Join-Path $DataDir $dll
    if (-not (Test-Path $target)) {
        $source = Join-Path $MingwBin $dll
        if (Test-Path $source) {
            Copy-Item $source $DataDir -Force
            Write-Warn "$dll was not deployed by windeployqt; copied from MinGW"
        } else {
            Fail "Missing $dll and it is not present in $MingwBin; cannot produce a runnable package"
        }
    }
}

$Required = @(
    $AppExeName,
    'platforms\qwindows.dll',
    'libgcc_s_seh-1.dll',
    'libstdc++-6.dll',
    'libwinpthread-1.dll'
)
foreach ($rel in $Required) {
    $path = Join-Path $DataDir $rel
    if (-not (Test-Path $path)) { Fail "Staging is missing a required file: $rel" }
}
Write-Ok 'All required files present'

$stat = Get-ChildItem $DataDir -Recurse -File | Measure-Object -Property Length -Sum
Write-Ok ("Staging: {0} files, {1} MB" -f $stat.Count, [math]::Round($stat.Sum / 1MB, 1))

# ---------------------------------------------------------------------------
# 7. Bundled resources: examples / LICENSE / README
# ---------------------------------------------------------------------------
Write-Step 'Copying bundled resources'

$ExamplesDir = Join-Path $RepoRoot 'examples'
if (Test-Path $ExamplesDir) {
    Copy-Item $ExamplesDir (Join-Path $DataDir 'examples') -Recurse -Force
    $count = (Get-ChildItem (Join-Path $DataDir 'examples') -Recurse -File | Measure-Object).Count
    Write-Ok "examples/ ($count files)"
} else {
    Write-Warn "$ExamplesDir not found; skipping"
}

foreach ($doc in @('README.md', 'LICENSE')) {
    $source = Join-Path $RepoRoot $doc
    if (Test-Path $source) {
        Copy-Item $source $DataDir -Force
        Write-Ok $doc
    } else {
        Write-Warn "Repository root has no $doc; it will not be included in the installer"
    }
}

# ---------------------------------------------------------------------------
# 8. Render IFW configuration
# ---------------------------------------------------------------------------
Write-Step 'Rendering IFW configuration'

$repoBlock = ''
if ($Mode -eq 'hybrid') {
    $repoBlock = @"
    <RemoteRepositories>
        <Repository>
            <Url>$RepoUrl/</Url>
        </Repository>
    </RemoteRepositories>
"@
}

function Render-Template {
    param(
        [string] $SourcePath,
        [string] $TargetPath,
        [hashtable] $Tokens
    )
    $content = Get-Content -Path $SourcePath -Raw -Encoding UTF8
    foreach ($key in $Tokens.Keys) {
        $content = $content.Replace($key, $Tokens[$key])
    }
    Write-Utf8NoBom -Path $TargetPath -Content $content
}

$tokens = @{
    '@APP_VERSION@'  = $AppVersion
    '@RELEASE_DATE@' = $ReleaseDate
    '@REPO_BLOCK@'   = $repoBlock
}

Render-Template -SourcePath (Join-Path $ScriptDir 'config\config.xml') `
                -TargetPath (Join-Path $RenderDir 'config.xml') `
                -Tokens $tokens

Render-Template -SourcePath (Join-Path $ScriptDir "packages\$PackageId\meta\package.xml") `
                -TargetPath (Join-Path $MetaDir 'package.xml') `
                -Tokens $tokens

Copy-Item (Join-Path $ScriptDir "packages\$PackageId\meta\license.txt") $MetaDir -Force

if ($Mode -eq 'hybrid') {
    # In hybrid mode let users add extra repository URLs in the maintenance tool.
    $configPath = Join-Path $RenderDir 'config.xml'
    $config = Get-Content -Path $configPath -Raw -Encoding UTF8
    $config = $config.Replace('<AllowRepositoriesForOfflineInstaller>false</AllowRepositoriesForOfflineInstaller>',
                              '<AllowRepositoriesForOfflineInstaller>true</AllowRepositoriesForOfflineInstaller>')
    $config = $config.Replace('<RepositorySettingsPageVisible>false</RepositorySettingsPageVisible>',
                              '<RepositorySettingsPageVisible>true</RepositorySettingsPageVisible>')
    Write-Utf8NoBom -Path $configPath -Content $config
}

Write-Ok "config.xml / package.xml rendered (version $AppVersion)"

if ($SkipPackage) {
    Write-Step 'Skipping packaging (-SkipPackage)'

    $manualFlags = '--offline-only'
    if ($Mode -eq 'hybrid') { $manualFlags = "--hybrid -i $PackageId" }

    Write-Host ''
    Write-Host 'Staging directory is ready:' -ForegroundColor White
    Write-Host "    $DataDir"
    Write-Host 'To package manually, run:' -ForegroundColor White
    Write-Host "    binarycreator $manualFlags -t <IFW>\installerbase.exe -p `"$PackagesDir`" -c `"$(Join-Path $RenderDir 'config.xml')`" `"$DistDir\FigureItNow7-$AppVersion-win64-setup.exe`""
    exit 0
}

# ---------------------------------------------------------------------------
# 9. Build the installer
# ---------------------------------------------------------------------------
Write-Step 'Building installer'

$InstallerBase = Join-Path $IfwBin 'installerbase.exe'
$BinaryCreator = Join-Path $IfwBin 'binarycreator.exe'
if (-not (Test-Path $InstallerBase)) { Fail "installerbase.exe not found: $InstallerBase" }

$OutFile = Join-Path $DistDir "FigureItNow7-$AppVersion-win64-setup.exe"

if ($Mode -eq 'hybrid') {
    & $BinaryCreator --hybrid -t $InstallerBase `
        -p $PackagesDir `
        -c (Join-Path $RenderDir 'config.xml') `
        -i $PackageId `
        $OutFile
} else {
    & $BinaryCreator --offline-only -t $InstallerBase `
        -p $PackagesDir `
        -c (Join-Path $RenderDir 'config.xml') `
        $OutFile
}
if ($LASTEXITCODE -ne 0) { Fail 'binarycreator failed' }
if (-not (Test-Path $OutFile)) { Fail "Installer was not produced: $OutFile" }

$sizeMb = [math]::Round((Get-Item $OutFile).Length / 1MB, 1)
Write-Ok "Installer: $OutFile ($sizeMb MB)"

# ---------------------------------------------------------------------------
# 10. Hybrid mode: generate the online update repository
# ---------------------------------------------------------------------------
if ($Mode -eq 'hybrid') {
    Write-Step 'Generating online update repository'

    $Repogen = Join-Path $IfwBin 'repogen.exe'
    if (-not (Test-Path $Repogen)) { Fail "repogen.exe not found: $Repogen" }

    $repogenArgs = @('-p', $PackagesDir)
    if (Test-Path $RepoOutDir) {
        $repogenArgs += '--update-new-components'
    }
    $repogenArgs += $RepoOutDir

    & $Repogen @repogenArgs
    if ($LASTEXITCODE -ne 0) { Fail 'repogen failed' }
    Write-Ok "Update repository: $RepoOutDir"

    Write-Host ''
    Write-Warn 'Updates.xml and the component archives must be uploaded together, otherwise checksum validation fails'
    Write-Host "    Example: git push origin `"$RepoOutDir`:gh-pages`"  (or copy the contents to /docs/updates/)"
}

Write-Host ''
Write-Host 'Done.' -ForegroundColor Green
