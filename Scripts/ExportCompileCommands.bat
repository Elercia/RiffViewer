@ECHO OFF
pushd %~dp0\
call bin\premake5.exe export-compile-commands .

xcopy /F /Y "projects\export-compile-commands\compile_commands\debug_win64.json" "..\compile_commands.json"
popd
PAUSE