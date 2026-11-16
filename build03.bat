cls
setlocal enabledelayedexpansion
set "programName=assignment03"
del !programName!.exe
g++ -I .\include !programName!.cpp include\glad\glad.c libglfw3_mingw.a -o "!programName!" -lgdi32 -lopengl32 -luser32
/wait
pause 

if exist ".\!programName!.exe" ".\!programName!.exe"
:: pause