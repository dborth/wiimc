@echo off
del wiimc.dol
del wiimc.elf
cd source\mplayer
make
if not %errorlevel% == 0 goto error

cd ..\..
make

goto end
 
:error
cd ..\..

:end