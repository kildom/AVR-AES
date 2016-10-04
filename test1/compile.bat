@echo off

@SET project=test1

@PATH=D:\web\bin\php;%PATH%
@PATH=C:\Program Files (x86)\Atmel\Atmel Studio 6.0\avrdbg;%PATH%
@PATH=C:\Program Files (x86)\Atmel\Atmel Studio 6.0\make;%PATH%
@PATH=C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\atmel\avrgcc\3.4.0.65\AVRToolchain\bin;%PATH%
@PATH=C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\atmel\armgcc\3.3.1.83\ARMGCCToolchain\bin;%PATH%
@PATH=C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\utils\bin;%PATH%
REM @PATH=*JRE*;%PATH%

cd /d %~dp0

rem cmd
rem exit

del /q input.map input.od input.txt input.hex >nul 2>nul

cd /d %~dp0..\Release

del /q %project%.hex >nul 2>nul

make.exe clean all > %~dp0make-out.txt 2> %~dp0make-err.txt

IF NOT EXIST %project%.hex GOTO endofcmp

copy /y %project%.map %~dp0input.map >nul
copy /y %project%.hex %~dp0input.hex >nul

cd /d %~dp0

avr-objdump -zhD %~dp0..\Release\%project%.elf > %~dp0input.od

php.exe odpostparse.php input.od > nul 2> nul

java -cp ..\..\avrora\avrora.jar avrora.Main -colors=false -monitors=ioregs -seconds=2 -ioregs=PORTA,PORTB input.od > %~dp0\input.txt

:endofcmp
