@echo off
cd /d "c:\Users\prana\Downloads\Approx_Term_Project\Bin-Packing-Approximation-Analysis"
echo Compiling...
g++ -std=c++17 -O2 -Wall -Wextra -I. -o runner.exe runner.cpp
if %errorlevel% neq 0 (
    echo Compilation FAILED with error code %errorlevel%
    exit /b %errorlevel%
)
echo Compilation SUCCESSFUL
echo.
echo Running experiments...
.\runner.exe results.csv
if %errorlevel% neq 0 (
    echo Execution FAILED with error code %errorlevel%
    exit /b %errorlevel%
)
echo Execution SUCCESSFUL
