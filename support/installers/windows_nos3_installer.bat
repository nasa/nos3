REM - This script assumes it is located
REM - one level above the repo.

title NASA Operational Simulator for Small Satellites (NOS3)

@echo off
cls

SET CURRENTDIR="%cd%"
cd %CURRENTDIR%\..\

echo.
echo Welcome to the Nasa Operational Simulator for Small Satellites (NOS3) Installer!
echo.
echo ----------------
echo -- Disclaimer --
echo ---------------- 
echo This software is provided ''as is'' without any warranty of any, kind either express, implied, or statutory, including, but not limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or any warranty that the software will be error free.  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages, arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty, contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software, documentation or services provided hereunder.
echo.
echo Press 'Y' to confirm you have read the disclaimer...
choice
If Errorlevel 2 Goto QUIT
If Errorlevel 1 Goto VAGRANT

:VAGRANT
echo.
echo Beginning install...
echo.
echo This process may take time, but will complete automatically.
echo Please wait to use NOS3 until this script is completed.
echo.
vagrant up > %CURRENTDIR%\..\nos3_install.log
vagrant reload

:QUIT
echo.
echo Exiting the NOS3 installer now! NOS3 is ready for use!