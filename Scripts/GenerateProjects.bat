@echo off
pushd %~dp0..
call "Scripts/premake5.exe" vs2022
popd
pause