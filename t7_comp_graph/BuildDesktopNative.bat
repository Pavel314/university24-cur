@echo off
chcp 65001 > nul

set SOKOL_REPO=G:\MyProg\C##\Sokol.NET
set DESKTOP_CSPROJ=t7_comp_graph.csproj
set BUILD_CONFIG=Release

set RAW_DIR=%~dp0
set PROJECT_DIR=%RAW_DIR:~0,-1%

echo =======================================================
echo [1/3] Preparing shaders and desktop assets (Prepare)...
echo =======================================================
cd /d "%SOKOL_REPO%"

dotnet run --project tools/SokolApplicationBuilder -- --task prepare --architecture x64 --path "%PROJECT_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Resource preparation phase failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =======================================================
echo [2/3] Compiling Desktop NativeAOT (Publish)...
echo =======================================================
cd /d "%PROJECT_DIR%"

:: очистка JIT-кэша VS
:: if exist obj rmdir /s /q obj
:: if exist bin rmdir /s /q bin

dotnet restore %DESKTOP_CSPROJ% -p:TargetFramework=net10.0
dotnet publish %DESKTOP_CSPROJ% -p:TargetFramework=net10.0 -c %BUILD_CONFIG% -r win-x64 --self-contained -p:PublishAot=true --no-restore

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] NativeAOT project compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =======================================================
echo [3/3] Launching compiled NativeAOT Desktop application...
echo =======================================================
"%PROJECT_DIR%\bin\%BUILD_CONFIG%\net10.0\win-x64\publish\%DESKTOP_CSPROJ:.csproj=.exe%"

pause
