cls
rem Revert version.h
svn revert src/version.h

rem Make
cmake -G "MinGW Makefiles"
IF "%1"=="" (mingw32-make all) ELSE (mingw32-make %1)

rem Revert version.h
rm -f src/version.h.old
svn revert src/version.h

rem Strip and place in local directories
strip survcalc.exe

rem make R libraries
cd R-libraries
call build.bat > build.log
cd ..

rem Make the installer
rm -f Setup*.exe
cd installer
call makeInstall.bat > makeInstall.log
cd ..

survcalc -h

copy Doc\SurvCalc.pdf \\niwa.local\groups\Wellington\NIWAFisheries\bin\ /Y
copy Setup_SurvCalc.exe \\niwa.local\groups\Wellington\NIWAFisheries\bin\ /Y
copy SurvCalc.exe \\niwa.local\groups\Wellington\NIWAFisheries\bin\ /Y

