@echo off
del wiimc.dol
del wiimc.elf
cd source\mplayer
make libmplayerwii.a
if not %errorlevel% == 0 goto error

cd ..\..
make

goto end
 
:error
cd ..\..

:end