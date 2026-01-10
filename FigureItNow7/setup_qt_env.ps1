# PowerShell script: Automatically detect and set Qt environment variables
# Also generates .vscode configuration files with detected paths
# Usage: Run in VSCode terminal: .\setup_qt_env.ps1

Write-Host "Detecting Qt installation..." -ForegroundColor Green

# Common Qt installation paths (auto-detect all versions)
$qtPaths = @(
    "$env:QTDIR",
    "C:\Qt\*\msvc*_64",
    "C:\Qt\*\mingw*_64",
    "C:\Qt\*\mingw_64",
    "C:\Program Files\Qt\*\msvc*_64",
    "C:\Program Files\Qt\*\mingw*_64",
    "C:\Program Files\Qt\*\mingw_64"
)

$foundQt = $false
$qtPath = ""
$mingwPath = ""

foreach ($path in $qtPaths) {
    if ($path -and (Test-Path $path)) {
        # If path contains wildcards, expand it
        if ($path -match '\*') {
            $expandedPaths = Get-ChildItem -Path $path -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
            if ($expandedPaths) {
                $path = $expandedPaths.FullName
            }
        }
        
            # Check if it's a valid Qt installation
            if ((Test-Path "$path\bin\qmake.exe") -or (Test-Path "$path\bin\qmake6.exe")) {
                Write-Host "Found Qt installation: $path" -ForegroundColor Yellow
                
                # Check if it's MinGW version
                if ($path -match "mingw") {
                    Write-Host "Detected MinGW version Qt" -ForegroundColor Green
                    
                    # Find MinGW toolchain path (might be in Qt Tools directory)
                    $mingwPaths = @(
                        "C:\Qt\Tools\mingw*_64\bin",
                        "C:\Qt\Tools\mingw*\bin",
                        "$path\bin"
                    )
                    
                    $foundMinGW = $false
                    foreach ($mp in $mingwPaths) {
                        $mingwCandidate = $mp
                        if ($mingwCandidate -match '\*') {
                            $expandedMinGW = Get-ChildItem -Path $mingwCandidate -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
                            if ($expandedMinGW) {
                                $mingwCandidate = $expandedMinGW.FullName
                            }
                        }
                        
                        if ((Test-Path "$mingwCandidate\g++.exe") -and (Test-Path "$mingwCandidate\mingw32-make.exe")) {
                            Write-Host "Found MinGW toolchain: $mingwCandidate" -ForegroundColor Green
                            $mingwPath = $mingwCandidate
                            $env:MINGW_PATH = $mingwCandidate
                            $env:PATH = "$mingwCandidate;$env:PATH"
                            $foundMinGW = $true
                            break
                        }
                    }
                    
                    if (-not $foundMinGW) {
                        Write-Host "Warning: Complete MinGW toolchain not found" -ForegroundColor Yellow
                    }
                }
                
                # Set environment variables
                $qtPath = $path
                $env:QTDIR = $path
                $env:Qt6_DIR = "$path\lib\cmake\Qt6"
                $env:PATH = "$path\bin;$env:PATH"
                
                Write-Host "QTDIR set: $env:QTDIR" -ForegroundColor Cyan
                Write-Host "Qt6_DIR set: $env:Qt6_DIR" -ForegroundColor Cyan
                Write-Host "PATH updated" -ForegroundColor Cyan
                
                # Check qmake version
                if (Test-Path "$path\bin\qmake.exe") {
                    $qmakeVersion = & "$path\bin\qmake.exe" -query QT_VERSION 2>$null
                    if ($qmakeVersion) {
                        Write-Host "Qt version: $qmakeVersion" -ForegroundColor Green
                    }
                }
                
                $foundQt = $true
                break
            }
    }
}

if (-not $foundQt) {
    Write-Host "Qt6 installation not found!" -ForegroundColor Red
    Write-Host "Please ensure Qt6 is installed, or manually set environment variables:" -ForegroundColor Yellow
    Write-Host '$env:QTDIR = "C:\Qt\6.x.x\msvcxxxx_64"' -ForegroundColor White
    Write-Host '$env:PATH = "$env:QTDIR\bin;$env:PATH"' -ForegroundColor White
    exit 1
}

# Check if CMake is installed
try {
    $cmakeVersion = cmake --version 2>$null
    if ($cmakeVersion) {
        Write-Host "CMake is installed" -ForegroundColor Green
    }
} catch {
    Write-Host "Warning: CMake not found, please install CMake" -ForegroundColor Yellow
}

# Generate .vscode/settings.json
Write-Host "Generating .vscode/settings.json..." -ForegroundColor Yellow

# Convert paths to forward slash format
$qtPathUnix = $qtPath.Replace('\', '/')
$mingwPathUnix = $mingwPath.Replace('\', '/')

$settingsJson = @{
    "cmake.configureOnOpen" = $true
    "cmake.generator" = "MinGW Makefiles"
    "cmake.buildDirectory" = "`${workspaceFolder}/build"
    "cmake.autoSelectActiveFolder" = $false
    "cmake.environment" = @{
        "QTDIR" = $qtPathUnix
        "Qt6_DIR" = "$qtPathUnix/lib/cmake/Qt6"
        "MINGW_PATH" = $mingwPathUnix
    }
    "cmake.configureArgs" = @(
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_CXX_STANDARD=17",
        "-DCMAKE_C_COMPILER=$mingwPathUnix/gcc.exe",
        "-DCMAKE_CXX_COMPILER=$mingwPathUnix/g++.exe",
        "-DCMAKE_MAKE_PROGRAM=$mingwPathUnix/mingw32-make.exe"
    )
    "cmake.buildArgs" = @("--parallel", "4")
    "cmake.preferredGenerators" = @("MinGW Makefiles", "Ninja", "Unix Makefiles")
    "cmake.allowUnsupportedPlatforms" = $true
    "cmake.showAdvanced" = $true
    "files.associations" = @{
        "*.h" = "cpp"
        "*.cpp" = "cpp"
        "*.ui" = "plaintext"
        "*.qrc" = "xml"
    }
    "C_Cpp.default.configurationProvider" = "ms-vscode.cmake-tools"
    "C_Cpp.default.cppStandard" = "c++17"
    "C_Cpp.default.cStandard" = "c11"
    "terminal.integrated.env.windows" = @{
        "QTDIR" = $qtPathUnix
        "Qt6_DIR" = "$qtPathUnix/lib/cmake/Qt6"
        "MINGW_PATH" = $mingwPathUnix
        "PATH" = "$mingwPathUnix;$qtPathUnix/bin;`${env:PATH}"
    }
} | ConvertTo-Json -Depth 10

$vscodeDir = ".vscode"
if (-not (Test-Path $vscodeDir)) {
    New-Item -ItemType Directory -Path $vscodeDir | Out-Null
}

$settingsJson | Out-File -FilePath "$vscodeDir/settings.json" -Encoding UTF8
Write-Host "Generated: $vscodeDir/settings.json" -ForegroundColor Green

# Generate .vscode/launch.json
Write-Host "Generating .vscode/launch.json..." -ForegroundColor Yellow

$launchJson = @{
    "version" = "0.2.0"
    "configurations" = @(
        @{
            "name" = "Debug"
            "type" = "cppdbg"
            "request" = "launch"
            "program" = "`${workspaceFolder}/build/FigureItNow7.exe"
            "args" = @()
            "stopAtEntry" = $false
            "cwd" = "`${workspaceFolder}"
            "environment" = @(
                @{
                    "name" = "QTDIR"
                    "value" = $qtPathUnix
                },
                @{
                    "name" = "MINGW_PATH"
                    "value" = $mingwPathUnix
                },
                @{
                    "name" = "PATH"
                    "value" = "$mingwPathUnix;$qtPathUnix/bin;`${env:PATH}"
                }
            )
            "externalConsole" = $false
            "MIMode" = "gdb"
            "miDebuggerPath" = "$mingwPathUnix/gdb.exe"
            "setupCommands" = @(
                @{
                    "description" = "Enable pretty-printing for gdb"
                    "text" = "-enable-pretty-printing"
                    "ignoreFailures" = $true
                }
            )
            "preLaunchTask" = "CMake: Build Debug"
        },
        @{
            "name" = "Release"
            "type" = "cppdbg"
            "request" = "launch"
            "program" = "`${workspaceFolder}/build/FigureItNow7.exe"
            "args" = @()
            "stopAtEntry" = $false
            "cwd" = "`${workspaceFolder}"
            "environment" = @(
                @{
                    "name" = "QTDIR"
                    "value" = $qtPathUnix
                },
                @{
                    "name" = "MINGW_PATH"
                    "value" = $mingwPathUnix
                },
                @{
                    "name" = "PATH"
                    "value" = "$mingwPathUnix;$qtPathUnix/bin;`${env:PATH}"
                }
            )
            "externalConsole" = $false
            "MIMode" = "gdb"
            "miDebuggerPath" = "$mingwPathUnix/gdb.exe"
            "setupCommands" = @(
                @{
                    "description" = "Enable pretty-printing for gdb"
                    "text" = "-enable-pretty-printing"
                    "ignoreFailures" = $true
                }
            )
            "preLaunchTask" = "CMake: Build Release"
        }
    )
} | ConvertTo-Json -Depth 10

$launchJson | Out-File -FilePath "$vscodeDir/launch.json" -Encoding UTF8
Write-Host "Generated: $vscodeDir/launch.json" -ForegroundColor Green

# Generate .vscode/tasks.json
Write-Host "Generating .vscode/tasks.json..." -ForegroundColor Yellow

$tasksJson = @{
    "version" = "2.0.0"
    "tasks" = @(
        @{
            "label" = "CMake: Build Debug"
            "type" = "cmake"
            "command" = "build"
            "targets" = @("all")
            "group" = "build"
            "problemMatcher" = @("`$gcc")
            "detail" = "CMake template build task"
        },
        @{
            "label" = "CMake: Build Release"
            "type" = "cmake"
            "command" = "build"
            "targets" = @("all")
            "group" = @{
                "kind" = "build"
                "isDefault" = $true
            }
            "problemMatcher" = @("`$gcc")
            "detail" = "CMake template build task"
        },
        @{
            "label" = "CMake: Configure"
            "type" = "cmake"
            "command" = "configure"
            "group" = "build"
            "problemMatcher" = @("`$gcc")
            "detail" = "CMake template configure task"
        },
        @{
            "label" = "CMake: Install"
            "type" = "cmake"
            "command" = "install"
            "targets" = @("install")
            "group" = "build"
            "problemMatcher" = @("`$gcc")
            "detail" = "CMake template install task"
        },
        @{
            "label" = "CMake: Clean"
            "type" = "cmake"
            "command" = "clean"
            "group" = "build"
            "problemMatcher" = @("`$gcc")
            "detail" = "CMake template clean task"
        }
    )
} | ConvertTo-Json -Depth 10

$tasksJson | Out-File -FilePath "$vscodeDir/tasks.json" -Encoding UTF8
Write-Host "Generated: $vscodeDir/tasks.json" -ForegroundColor Green

Write-Host "`nQt environment setup completed!" -ForegroundColor Green
Write-Host "You can now configure CMake project in VSCode or press F5 to debug." -ForegroundColor Cyan
