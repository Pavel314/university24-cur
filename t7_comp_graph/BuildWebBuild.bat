@echo off
chcp 65001 > nul
set SOKOL_REPO=G:\MyProg\C##\Sokol.NET
set WEB_CSPROJ=t7_comp_graphWeb.csproj
set BUILD_CONFIG=Release

set RAW_DIR=%~dp0
set PROJECT_DIR=%RAW_DIR:~0,-1%

echo =======================================================
echo [1/3] Preparing shaders and web assets (Prepare)...
echo =======================================================
cd /d "%SOKOL_REPO%"
dotnet run --project tools/SokolApplicationBuilder -- --task prepare --architecture web --path "%PROJECT_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Resource preparation phase failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =======================================================
echo [2/3] Compiling .NET Web Assembly (Build)...
echo =======================================================
cd /d "%PROJECT_DIR%"
dotnet publish %WEB_CSPROJ% -c %BUILD_CONFIG%
REM Note. We are prefer dotnet publish instead of dotnet build to reduce the output size

if %ERRORLEVEL% NEQ 0 (
    echo .csproj project compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =======================================================
echo [3/3] Launching native web server (dotnet-serve)...
echo =======================================================
cd /d "%PROJECT_DIR%\bin\%BUILD_CONFIG%\net10.0\browser-wasm\AppBundle"

echo.
echo Server successfully started at http://localhost:8080
echo The browser should open automatically.
echo To stop the server, press Ctrl+C in this window.
echo.

dotnet run --project "%SOKOL_REPO%\tools\dotnet-serve\src\dotnet-serve\dotnet-serve.csproj" -- ^
  -d . ^
  -p 8080 ^
  -a 0.0.0.0 ^
  -h "Cross-Origin-Opener-Policy: same-origin" ^
  -h "Cross-Origin-Embedder-Policy: require-corp" ^
  -h "Cache-Control: no-store, no-cache, must-revalidate, max-age=0" ^
  -h "Pragma: no-cache" ^
  -o

pause