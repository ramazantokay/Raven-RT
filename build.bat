@echo off

IF "%~1" == "" GOTO PrintHelp

vendor\premake5.exe %1
GOTO Done

:PrintHelp

echo.
echo Enter 'build.bat action' where action is one of the following:
echo.
echo   clean             Remove all binaries and intermediate binaries and project files.
echo   vs2022            Generate Visual Studio 2022 project files

GOTO Done

:Done
PAUSE