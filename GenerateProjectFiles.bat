echo off

echo Compiling Shaders...
call %~dp0/Scripts/compile_shaders.bat

echo Generating Visual Studio Solution...
"%~dp0/Binaries/premake5.exe" vs2019 --file="%~dp0/Scripts/premake_vs2022.lua"