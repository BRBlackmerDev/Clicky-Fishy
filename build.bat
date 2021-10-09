@echo off

set projName=fish
pushd C:\Users\%username%\Desktop\Projects\coding\%projName%

if %errorlevel% EQU 0 ( call build_win32.bat )

popd
