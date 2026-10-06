cls
setlocal enabledelayedexpansion
set "programName=assignment02"
del !programName!.exe
gcc -I .\include !programName!.c include\glad\glad.c libglfw3_mingw.a -o "!programName!" -lgdi32 -lopengl32 -luser32
/wait
pause 

if exist ".\!programName!.exe" ".\!programName!.exe"
:: pause