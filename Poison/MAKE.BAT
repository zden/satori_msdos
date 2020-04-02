@echo	off
tasm poison.asm -m9
if errorlevel 1 goto koniec
tlink poison.obj module\module.obj, poison.exe
:koniec