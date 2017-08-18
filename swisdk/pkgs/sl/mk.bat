@echo off
::
:: mk.bat - Batch file to make the SL package
::
:: $Id: mk.bat,v 1.1 2008/03/06 20:39:20 rfiler Exp $
::

:: Check if user asking for help
if "%1" == "help" goto help1
if "%1" == "HELP" goto help1

:: setup needed environment variable
cd ..\..
call .\tools\setroot.bat
cd pltfm\sl

:: Check for tools in the path
call ..\..\tools\setenv.bat


:: Call nmake 
call submake.bat sl %1 %2 %3 %4 

:: Remove our added junk from the environment
call ..\..\tools\restenv.bat
goto done

:help1
echo Useage: mk <target>
echo 	where optional <target> can be clean, depend, intermediate, 
echo	all or any of the other possible makefile targets.
echo.
echo 	Generating a local build.log file, call mk with:
echo	ftee mk <target> > build.log
echo.
echo	note: ftee can be found in the fw/tools directory

:done                                 
