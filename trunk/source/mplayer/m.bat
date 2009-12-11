@echo off
del mplayer.elf
make
if not %errorlevel% == 0 goto end

wiiload mplayer.dol

:end
