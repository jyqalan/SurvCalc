
rem Make
cmake -G "MinGW Makefiles"
IF "%1"=="" (mingw32-make all) ELSE (mingw32-make %1)

rem Strip and place in local directories
strip survcalc.exe

rem make R libraries
cd R-libraries
call build.bat > build.log
cd ..


survcalc -h


