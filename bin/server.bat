@echo off
if exist "%~dp0SpaceGame.exe" (set "DEBUG=") else (set "DEBUG=_d")
if [%1] == [] (set "OPT1=-s") else (set "OPT1=")
start "" "%~dp0SpaceGame%DEBUG%" %OPT1% %*